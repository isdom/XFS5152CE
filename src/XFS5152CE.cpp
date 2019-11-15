
#include <Arduino.h>
#include "XFS5152CE.h"
#include "CharsetUtil.h"

HardwareSerial ss(2);


#ifdef DEBUG
#define DMSG(args...)       BLESerial.print(args)
#define DMSG_STR(str)       BLESerial.println(str)
#define DMSG_HEX(num)       BLESerial.print(' '); BLESerial.print((num>>4)&0x0F, HEX); BLESerial.print(num&0x0F, HEX)
#define DMSG_INT(num)       BLESerial.print(' '); BLESerial.print(num)
#else
#define DMSG(args...)
#define DMSG_STR(str)
#define DMSG_HEX(num)
#define DMSG_INT(num)
#endif

void XFS5152CEBegin(int8_t rxpin, int8_t txpin) {
  ss.begin(9600, SERIAL_8N1, rxpin, txpin);
}

void speakASCIIOrGB2312(const char* msg) {
  ss.write(0xFD);
  ss.write((byte)0x0);
  ss.write(2 + strlen(msg));
  ss.write(0x01);
  ss.write((byte)0x0);
  ss.write(msg);
}

void waitForSpeech(unsigned long timeout) {
  unsigned long start = millis();
  bool done = false;
  while ( ! done && (millis() - start) < timeout ) {
    while ( ss.available() ) {
      const uint8_t ack = ss.read();
      DMSG("waitfor speech ack: ");
      DMSG_HEX(ack);
      DMSG("\n");
      if ( ack == 0x4F ) {
        done = true;
        break;
      }
    }
  }
}

uint8_t waitForAck(unsigned long timeout) {
  unsigned long start = millis();
  while ( (millis() - start) < timeout ) {
    if ( ss.available() ) {
      return ss.read();
    }
  }
}

uint8_t checkStatus(unsigned long timeout) {
  ss.write(0xFD);
  ss.write((byte)0x0);
  ss.write(0x01);
  ss.write(0x21);
  return waitForAck(timeout);
}

void speakUnicode(const uint8_t* msg, uint32_t len) {
  ss.write(0xFD);
  ss.write((byte)0x0);
  ss.write(2 + len);
  ss.write(0x01);
  ss.write(0x03);
  ss.write(msg, len);
}

void waitForXFS5152Idle(unsigned long timeout) {
  uint8_t ack;
  
  do {
    ack = checkStatus(timeout);
    DMSG("check status ack: ");
    DMSG_HEX(ack);
    DMSG("\n");
  } while(ack != 0x4F);
}

uint8_t sleepXFS5152(unsigned long timeout) {
  ss.write(0xFD);
  ss.write((byte)0x0);
  ss.write(0x01);
  ss.write(0x88);
  return waitForAck(timeout);
}

uint8_t wakeupXFS5152(unsigned long timeout) {
  ss.write(0xFD);
  ss.write((byte)0x0);
  ss.write(0x01);
  ss.write(0xFF);
  return waitForAck(timeout);
}

void beginSpeech(const String &utf8) {
  String unicode = utf8_unicode(utf8);
  if (unicode.length() > 0) {
    wakeupXFS5152();
    waitForXFS5152Idle();
    speakUnicode((const uint8_t*)unicode.c_str(), unicode.length());
  }
}

void beginSpeechASCII(const String &ascii) {
  if (ascii.length() > 0) {
    wakeupXFS5152();
    waitForXFS5152Idle();
    speakASCIIOrGB2312((const char*)ascii.c_str());
  }
}

void endSpeech() {
  waitForSpeech();
  waitForXFS5152Idle();
  sleepXFS5152();
}

void speakInUtf8(const String &utf8) {
  beginSpeech(utf8);
  endSpeech();
}

void speakInASCII(const String &ascii) {
  beginSpeechASCII(ascii);
  endSpeech();
}
