#include "esphome/core/log.h"
#include "ikea_ansluta.h"
#include "cc2500.h"

namespace esphome {
namespace ikea_ansluta {
static const char *TAG = "ikea_ansluta";

void IkeaAnsluta::setup() {
  this->spi_setup();
  this->enable();
  this->send_strobe_(CC2500_SRES);
  // Init CC2500
  this->write_reg_(REG_IOCFG2, VAL_IOCFG2);
  this->write_reg_(REG_IOCFG0, VAL_IOCFG0);
  this->write_reg_(REG_PKTLEN, VAL_PKTLEN);
  this->write_reg_(REG_PKTCTRL1, VAL_PKTCTRL1);
  this->write_reg_(REG_PKTCTRL0, VAL_PKTCTRL0);
  this->write_reg_(REG_ADDR, VAL_ADDR);
  this->write_reg_(REG_CHANNR, VAL_CHANNR);
  this->write_reg_(REG_FSCTRL1, VAL_FSCTRL1);
  this->write_reg_(REG_FSCTRL0, VAL_FSCTRL0);
  this->write_reg_(REG_FREQ2, VAL_FREQ2);
  this->write_reg_(REG_FREQ1, VAL_FREQ1);
  this->write_reg_(REG_FREQ0, VAL_FREQ0);
  this->write_reg_(REG_MDMCFG4, VAL_MDMCFG4);
  this->write_reg_(REG_MDMCFG3, VAL_MDMCFG3);
  this->write_reg_(REG_MDMCFG2, VAL_MDMCFG2);
  this->write_reg_(REG_MDMCFG1, VAL_MDMCFG1);
  this->write_reg_(REG_MDMCFG0, VAL_MDMCFG0);
  this->write_reg_(REG_DEVIATN, VAL_DEVIATN);
  this->write_reg_(REG_MCSM2, VAL_MCSM2);
  this->write_reg_(REG_MCSM1, VAL_MCSM1);
  this->write_reg_(REG_MCSM0, VAL_MCSM0);
  this->write_reg_(REG_FOCCFG, VAL_FOCCFG);
  this->write_reg_(REG_BSCFG, VAL_BSCFG);
  this->write_reg_(REG_AGCCTRL2, VAL_AGCCTRL2);
  this->write_reg_(REG_AGCCTRL1, VAL_AGCCTRL1);
  this->write_reg_(REG_AGCCTRL0, VAL_AGCCTRL0);
  this->write_reg_(REG_WOREVT1, VAL_WOREVT1);
  this->write_reg_(REG_WOREVT0, VAL_WOREVT0);
  this->write_reg_(REG_WORCTRL, VAL_WORCTRL);
  this->write_reg_(REG_FREND1, VAL_FREND1);
  this->write_reg_(REG_FREND0, VAL_FREND0);
  this->write_reg_(REG_FSCAL3, VAL_FSCAL3);
  this->write_reg_(REG_FSCAL2, VAL_FSCAL2);
  this->write_reg_(REG_FSCAL1, VAL_FSCAL1);
  this->write_reg_(REG_FSCAL0, VAL_FSCAL0);
  this->write_reg_(REG_RCCTRL1, VAL_RCCTRL1);
  this->write_reg_(REG_RCCTRL0, VAL_RCCTRL0);
  this->write_reg_(REG_FSTEST, VAL_FSTEST);
  this->write_reg_(0x3E, 0xFF);
  this->disable();
}

void IkeaAnsluta::dump_config() { LOG_PIN("  CS Pin: ", this->cs_); }

void IkeaAnsluta::loop() {
  this->enable();
  // Sniffing slows down command sending. Therefore we only sniff if we are
  // not sending commands or for every n command sent
  if (this->commands_to_send_.empty()) {
    this->sniff_();
  } else if (this->commands_sent_ % this->sniff_after_commands_sent_.value_or(5) == 0) {
    this->sniff_();
    this->commands_sent_ = 0;
  }

  for (auto it = this->commands_to_send_.begin(); it != this->commands_to_send_.end();) {
    this->send_command_(it->first, it->second.command);
    it->second.times_sent++;
    this->commands_sent_++;
    if (it->second.times_sent == this->send_command_times_.value_or(50)) {
      ESP_LOGD(TAG, "Done sending command %#04x to address %#04x", (uint8_t) it->second.command, it->first);
      it = this->commands_to_send_.erase(it);
    } else {
      ++it;
    }
  }
  this->disable();
}

void IkeaAnsluta::sniff_() {
  std::vector<uint8_t> packet;
  this->read_packet_(packet);
  if (this->valid_packet_(packet)) {
    uint16_t address = (packet[2] << 8) + packet[3];
    IkeaAnslutaCommand command = (IkeaAnslutaCommand) packet[4];
    ESP_LOGD(TAG, "Sniffed command %#02x from remote %#04x", (uint8_t) command, address);
    for (auto &listener : this->listeners_)
      if (listener.remote_address == address)
        listener.on_command(command);
  }
}

bool IkeaAnsluta::valid_packet_(const std::vector<uint8_t> &packet) {
  return packet.size() == 6 && packet.front() == 0x55 && packet.at(1) == 0x01 &&
         this->valid_cmd_((IkeaAnslutaCommand) packet.at(4)) && packet.back() == 0xAA;
}

bool IkeaAnsluta::valid_cmd_(IkeaAnslutaCommand cmd) {
  return cmd == IkeaAnslutaCommand::OFF || cmd == IkeaAnslutaCommand::ON_50 || cmd == IkeaAnslutaCommand::ON_100 ||
         cmd == IkeaAnslutaCommand::PAIR;
}

void IkeaAnsluta::register_listener(uint16_t remote_address, const std::function<void(IkeaAnslutaCommand)> &func) {
  auto listener = IkeaAnslutaListener{
      .remote_address = remote_address,
      .on_command = func,
  };
  this->listeners_.push_back(listener);
}

void IkeaAnsluta::read_packet_(std::vector<uint8_t> &buffer) {
  this->send_strobe_(CC2500_SRX);
  this->write_reg_(REG_IOCFG1, 0x01);
  delay(20);

  uint8_t len = this->read_reg_(CC2500_FIFO);

  if (len == 6) {
    for (int i = 0; i < 6; i++) {
      buffer.push_back(this->read_reg_(CC2500_FIFO));
    }
  }

  this->send_strobe_(CC2500_SIDLE);
  this->send_strobe_(CC2500_SFRX);
}

void IkeaAnsluta::send_strobe_(uint8_t strobe) {
  this->cs_->digital_write(false);
  delayMicroseconds(1);
  this->write_byte(strobe);
  this->cs_->digital_write(true);
  delayMicroseconds(2000);
}

void IkeaAnsluta::write_reg_(uint8_t addr, uint8_t value) {
  this->cs_->digital_write(false);
  delayMicroseconds(1);
  this->write_byte(addr);
  delayMicroseconds(200);
  this->write_byte(value);
  delayMicroseconds(1);
  this->cs_->digital_write(true);
}

uint8_t IkeaAnsluta::read_reg_(uint8_t addr) {
  addr += 0x80;
  this->cs_->digital_write(false);
  delayMicroseconds(1);
  this->write_byte(addr);
  delay(10);
  uint8_t y = this->read_byte();
  delayMicroseconds(1);
  this->cs_->digital_write(true);
  return y;
}

void IkeaAnsluta::queue_command(uint16_t address, IkeaAnslutaCommand command) {
  if (!this->commands_to_send_.count(address)) {
    auto pair = std::make_pair(address, IkeaAnslutaCommandState{
                                            .command = command,
                                            .times_sent = 0,
                                        });

    this->commands_to_send_.insert(pair);
    return;
  }

  if (this->commands_to_send_[address].command != command) {
    this->commands_to_send_[address].command = command;
    this->commands_to_send_[address].times_sent = 0;
  }
}

void IkeaAnsluta::send_command_(uint16_t address, IkeaAnslutaCommand command) {
  this->send_strobe_(CC2500_SFTX);   // 0x3B
  this->send_strobe_(CC2500_SIDLE);  // 0x36
  this->cs_->digital_write(false);
  delayMicroseconds(1);
  this->write_byte(0x7F);
  this->write_byte(0x06);
  this->write_byte(0x55);
  this->write_byte(0x01);
  this->write_byte((uint8_t)(address >> 8));
  this->write_byte((uint8_t)(address & 0xFF));
  this->write_byte((uint8_t) command);
  this->write_byte(0xAA);
  this->write_byte(0xFF);
  this->cs_->digital_write(true);
  this->send_strobe_(CC2500_STX);
  delayMicroseconds(10);
}

}  // namespace ikea_ansluta
}  // namespace esphome
