
#pragma once

#include <inttypes.h>

void XFS5152CEBegin(int8_t rxpin, int8_t txpin);

void speakASCIIOrGB2312(const char* msg);

void waitForSpeech(unsigned long timeout = 60000);

uint8_t waitForAck(unsigned long timeout = 60000);

uint8_t checkStatus(unsigned long timeout = 60000);

void speakUnicode(const uint8_t* msg, uint32_t len);

void waitForXFS5152Idle(unsigned long timeout = 60000);

uint8_t sleepXFS5152(unsigned long timeout = 60000);

uint8_t wakeupXFS5152(unsigned long timeout = 60000);

void beginSpeech(const String &utf8);

void beginSpeechASCII(const String &ascii);

void endSpeech();

void speakInUtf8(const String &utf8);

void speakInASCII(const String &ascii);
