#include "esphome.h"
#include "SPI.h"
#include "cc2500_REG.h"
#include "cc2500_VAL.h"
#include "cc2500_CMD.h"
#include "commands.h"

class Radio
{
public:
  void setup()
  {
    pinMode(SS, OUTPUT);

    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV64);
    digitalWrite(SS, HIGH);

    sendStrobe(CC2500_SRES); //0x30 SRES Reset chip.
    initCC2500();

    writeReg(0x3E, 0xFF); //Maximum transmit power - write 0xFF to 0x3E (PATABLE)
  }

  bool readPacket(char *buffer)
  {
    sendStrobe(CC2500_SRX);
    writeReg(REG_IOCFG1, 0x01);
    delay(20);

    char len = readReg(CC2500_FIFO);

    // I don't know why the packets I get are 6 bytes long. Missing the first
    // and last byte I've seen in other people's code. But honestly, I have no
    // idea what I'm doing. Just hacking something together based on the
    // work of others :-).
    if (len == 6)
    {
      for (int i = 0; i < 6; i++)
      {
        buffer[i] = readReg(CC2500_FIFO);
      }
    }

    sendStrobe(CC2500_SIDLE); // Exit RX / TX
    sendStrobe(CC2500_SFRX);  // Flush the RX FIFO buffer
    return len == 6;
  }

  bool validCmd(char cmd)
  {
    return cmd == LIGHT_ON_50 || cmd == LIGHT_ON_100 || cmd == LIGHT_OFF || cmd == LIGHT_PAIR;
  }

  bool sniffCommand(short &addr, char &cmd)
  {
    char packet[6];
    if (readPacket(packet) && packet[0] == 0x55 && packet[1] == 0x01 && packet[5] == 0xAA)
    {
      addr = ((unsigned char) packet[2] << 8) + (unsigned char) packet[3];
      cmd = packet[4];
      if (validCmd(cmd))
      {
        ESP_LOGD("radio", "Sniffed command %02x from remote %04x", cmd, addr);
        return true;
      }
    }
    return false;
  }

  void sendStrobe(char strobe)
  {
    digitalWrite(SS, LOW);
    delayMicroseconds(1);
    SPI.write(strobe);
    digitalWrite(SS, HIGH);
    delayMicroseconds(2);
  }

  void sendCommand(short address, char command)
  {
    for (int i = 0; i < 200; i++)
    {
      sendStrobe(CC2500_SFTX);  // 0x3B
      sendStrobe(CC2500_SIDLE); // 0x36

      digitalWrite(SS, LOW);
      delayMicroseconds(1); // can't wait for digitalRead(MISO)==HIGH! Don't work in SPI mode

      SPI.transfer(0x7F); // activate burst data
      delayMicroseconds(2);

      SPI.transfer(0x06); // send 6 data bytes
      delayMicroseconds(2);

      SPI.transfer(0x55); // ansluta data byte 1
      delayMicroseconds(2);

      SPI.transfer(0x01); // ansluta data byte 2
      delayMicroseconds(2);

      SPI.transfer((unsigned short) address >> 8); // ansluta data address byte A
      delayMicroseconds(2);

      SPI.transfer((unsigned short) address & 0xFF); // ansluta data address byte B
      delayMicroseconds(2);

      SPI.transfer(command); // ansluta data command 0x01=Light OFF 0x02=50% 0x03=100% 0xFF=Pairing
      delayMicroseconds(2);

      SPI.transfer(0xAA); // ansluta data byte 6
      delayMicroseconds(2);

      digitalWrite(SS, HIGH); // end transfer

      sendStrobe(CC2500_STX); // 0x35  transmit data in TX

      delayMicroseconds(1600);
    }
  }

  void writeReg(char addr, char value)
  {
    digitalWrite(SS, LOW);
    delayMicroseconds(1); // can't wait for digitalRead(MISO)==HIGH! Don't work in SPI mode
    SPI.transfer(addr);
    delayMicroseconds(200);
    SPI.transfer(value);
    delayMicroseconds(1);
    digitalWrite(SS, HIGH);
  }

  char readReg(char addr)
  {
    addr = addr + 0x80; // set r/w bit (bit7=1 read, bit7=0 write)

    digitalWrite(SS, LOW);
    delayMicroseconds(1); // can't wait for digitalRead(MISO)==HIGH! Don't work in SPI mode
    char x = SPI.transfer(addr);
    delay(10);
    char y = SPI.transfer(0);
    delayMicroseconds(1);
    digitalWrite(SS, HIGH);

    return y;
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
