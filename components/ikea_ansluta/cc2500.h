#pragma once

#include "stdint.h"

namespace esphome {
namespace ikea_ansluta {
static const uint8_t CC2500_SRES = 0x30;
static const uint8_t CC2500_SRX = 0x34;
static const uint8_t CC2500_STX = 0x35;
static const uint8_t CC2500_SIDLE = 0x36;
static const uint8_t CC2500_SFRX = 0x3A;
static const uint8_t CC2500_SFTX = 0x3B;
static const uint8_t CC2500_FIFO = 0x3F;

static const uint8_t REG_IOCFG2 = 0x00;
static const uint8_t REG_IOCFG1 = 0x01;
static const uint8_t REG_IOCFG0 = 0x02;
static const uint8_t REG_PKTLEN = 0x06;
static const uint8_t REG_PKTCTRL1 = 0x07;
static const uint8_t REG_PKTCTRL0 = 0x08;
static const uint8_t REG_ADDR = 0x09;
static const uint8_t REG_CHANNR = 0x0A;
static const uint8_t REG_FSCTRL1 = 0x0B;
static const uint8_t REG_FSCTRL0 = 0x0C;
static const uint8_t REG_FREQ2 = 0x0D;
static const uint8_t REG_FREQ1 = 0x0E;
static const uint8_t REG_FREQ0 = 0x0F;
static const uint8_t REG_MDMCFG4 = 0x10;
static const uint8_t REG_MDMCFG3 = 0x11;
static const uint8_t REG_MDMCFG2 = 0x12;
static const uint8_t REG_MDMCFG1 = 0x13;
static const uint8_t REG_MDMCFG0 = 0x14;
static const uint8_t REG_DEVIATN = 0x15;
static const uint8_t REG_MCSM2 = 0x16;
static const uint8_t REG_MCSM1 = 0x17;
static const uint8_t REG_MCSM0 = 0x18;
static const uint8_t REG_FOCCFG = 0x19;
static const uint8_t REG_BSCFG = 0x1A;
static const uint8_t REG_AGCCTRL2 = 0x1B;
static const uint8_t REG_AGCCTRL1 = 0x1C;
static const uint8_t REG_AGCCTRL0 = 0x1D;
static const uint8_t REG_WOREVT1 = 0x1E;
static const uint8_t REG_WOREVT0 = 0x1F;
static const uint8_t REG_WORCTRL = 0x20;
static const uint8_t REG_FREND1 = 0x21;
static const uint8_t REG_FREND0 = 0x22;
static const uint8_t REG_FSCAL3 = 0x23;
static const uint8_t REG_FSCAL2 = 0x24;
static const uint8_t REG_FSCAL1 = 0x25;
static const uint8_t REG_FSCAL0 = 0x26;
static const uint8_t REG_RCCTRL1 = 0x27;
static const uint8_t REG_RCCTRL0 = 0x28;
static const uint8_t REG_FSTEST = 0x29;

static const uint8_t VAL_IOCFG2 = 0x29;
static const uint8_t VAL_IOCFG0 = 0x06;
static const uint8_t VAL_PKTLEN = 0xFF;
static const uint8_t VAL_PKTCTRL1 = 0x04;
static const uint8_t VAL_PKTCTRL0 = 0x05;
static const uint8_t VAL_ADDR = 0x01;
static const uint8_t VAL_CHANNR = 0x10;
static const uint8_t VAL_FSCTRL1 = 0x09;
static const uint8_t VAL_FSCTRL0 = 0x00;
static const uint8_t VAL_FREQ2 = 0x5D;
static const uint8_t VAL_FREQ1 = 0x93;
static const uint8_t VAL_FREQ0 = 0xB1;
static const uint8_t VAL_MDMCFG4 = 0x2D;
static const uint8_t VAL_MDMCFG3 = 0x3B;
static const uint8_t VAL_MDMCFG2 = 0x73;
static const uint8_t VAL_MDMCFG1 = 0xA2;
static const uint8_t VAL_MDMCFG0 = 0xF8;
static const uint8_t VAL_DEVIATN = 0x01;
static const uint8_t VAL_MCSM2 = 0x07;
static const uint8_t VAL_MCSM1 = 0x30;
static const uint8_t VAL_MCSM0 = 0x18;
static const uint8_t VAL_FOCCFG = 0x1D;
static const uint8_t VAL_BSCFG = 0x1C;
static const uint8_t VAL_AGCCTRL2 = 0xC7;
static const uint8_t VAL_AGCCTRL1 = 0x00;
static const uint8_t VAL_AGCCTRL0 = 0xB2;
static const uint8_t VAL_WOREVT1 = 0x87;
static const uint8_t VAL_WOREVT0 = 0x6B;
static const uint8_t VAL_WORCTRL = 0xF8;
static const uint8_t VAL_FREND1 = 0xB6;
static const uint8_t VAL_FREND0 = 0x10;
static const uint8_t VAL_FSCAL3 = 0xEA;
static const uint8_t VAL_FSCAL2 = 0x0A;
static const uint8_t VAL_FSCAL1 = 0x00;
static const uint8_t VAL_FSCAL0 = 0x11;
static const uint8_t VAL_RCCTRL1 = 0x41;
static const uint8_t VAL_RCCTRL0 = 0x00;
static const uint8_t VAL_FSTEST = 0x59;
}  // namespace ikea_ansluta
}  // namespace esphome
