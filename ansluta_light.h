#include "esphome.h"
#include <SPI.h>
#include "cc2500_REG.h"
#include "cc2500_VAL.h"
#include "cc2500_CMD.h"

#define CS 15
#define Light_OFF 0x01    // Command to turn the light off
#define Light_ON_50 0x02  // Command to turn the light on 50%
#define Light_ON_100 0x03 // Command to turn the light on 100%

class Ansluta : public Component, public CustomMQTTDevice
{
private:
  std::string topic_prefix = "ikea_ansluta/light";
public:
  void setup() override
  {
    pinMode(CS, OUTPUT);

    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV64);
    digitalWrite(CS, HIGH);

    sendStrobe(CC2500_SRES); //0x30 SRES Reset chip.
    initCC2500();

    writeReg(0x3E, 0xFF); //Maximum transmit power - write 0xFF to 0x3E (PATABLE)

    subscribe(topic_prefix+"/+/set", &Ansluta::on_set);
  }

  void loop() {
  }

  char hexdigit(char hex)
  {
    return (hex <= '9') ? hex - '0' : toupper(hex) - 'A' + 10;
  }

  char hexbyte(const char *hex)
  {
    return (hexdigit(*hex) << 4) | hexdigit(*(hex + 1));
  }

  void getRemoteAddressFromTopic(const std::string &topic, char &addrA, char &addrB)
  {
    addrA = hexbyte(topic.substr(topic_prefix.length()+1, 2).c_str());
    addrB = hexbyte(topic.substr(topic_prefix.length()+3, 2).c_str());
  }

  void on_set(const std::string &topic, const std::string &payload)
  {
    ESP_LOGD("ansluta", "Topic: '%s', Payload: '%s'", topic.c_str(), payload.c_str());
    char addrA;
    char addrB;
    getRemoteAddressFromTopic(topic, addrA, addrB);
    ESP_LOGD("ansluta", "Remote address: %#2x %#2x", addrA, addrB);
  }

  void sendStrobe(char strobe)
  {
    digitalWrite(CS, LOW);
    delayMicroseconds(1);
    SPI.write(strobe);
    digitalWrite(CS, HIGH);
    delayMicroseconds(2);
  }

  void writeReg(char addr, char value)
  {
    digitalWrite(CS, LOW);
    delayMicroseconds(1); // can't wait for digitalRead(MISO)==HIGH! Don't work in SPI mode
    SPI.transfer(addr);
    delayMicroseconds(1);
    SPI.transfer(value);
    delayMicroseconds(1);
    digitalWrite(CS, HIGH);
  }

  void initCC2500()
  {
    writeReg(REG_IOCFG2, VAL_IOCFG2);
    writeReg(REG_IOCFG0, VAL_IOCFG0);
    writeReg(REG_PKTLEN, VAL_PKTLEN);
    writeReg(REG_PKTCTRL1, VAL_PKTCTRL1);
    writeReg(REG_PKTCTRL0, VAL_PKTCTRL0);
    writeReg(REG_ADDR, VAL_ADDR);
    writeReg(REG_CHANNR, VAL_CHANNR);
    writeReg(REG_FSCTRL1, VAL_FSCTRL1);
    writeReg(REG_FSCTRL0, VAL_FSCTRL0);
    writeReg(REG_FREQ2, VAL_FREQ2);
    writeReg(REG_FREQ1, VAL_FREQ1);
    writeReg(REG_FREQ0, VAL_FREQ0);
    writeReg(REG_MDMCFG4, VAL_MDMCFG4);
    writeReg(REG_MDMCFG3, VAL_MDMCFG3);
    writeReg(REG_MDMCFG2, VAL_MDMCFG2);
    writeReg(REG_MDMCFG1, VAL_MDMCFG1);
    writeReg(REG_MDMCFG0, VAL_MDMCFG0);
    writeReg(REG_DEVIATN, VAL_DEVIATN);
    writeReg(REG_MCSM2, VAL_MCSM2);
    writeReg(REG_MCSM1, VAL_MCSM1);
    writeReg(REG_MCSM0, VAL_MCSM0);
    writeReg(REG_FOCCFG, VAL_FOCCFG);
    writeReg(REG_BSCFG, VAL_BSCFG);
    writeReg(REG_AGCCTRL2, VAL_AGCCTRL2);
    writeReg(REG_AGCCTRL1, VAL_AGCCTRL1);
    writeReg(REG_AGCCTRL0, VAL_AGCCTRL0);
    writeReg(REG_WOREVT1, VAL_WOREVT1);
    writeReg(REG_WOREVT0, VAL_WOREVT0);
    writeReg(REG_WORCTRL, VAL_WORCTRL);
    writeReg(REG_FREND1, VAL_FREND1);
    writeReg(REG_FREND0, VAL_FREND0);
    writeReg(REG_FSCAL3, VAL_FSCAL3);
    writeReg(REG_FSCAL2, VAL_FSCAL2);
    writeReg(REG_FSCAL1, VAL_FSCAL1);
    writeReg(REG_FSCAL0, VAL_FSCAL0);
    writeReg(REG_RCCTRL1, VAL_RCCTRL1);
    writeReg(REG_RCCTRL0, VAL_RCCTRL0);
    writeReg(REG_FSTEST, VAL_FSTEST);
    writeReg(REG_TEST2, VAL_TEST2);
    writeReg(REG_TEST1, VAL_TEST1);
    writeReg(REG_TEST0, VAL_TEST0);
    writeReg(REG_DAFUQ, VAL_DAFUQ);
  }
};
