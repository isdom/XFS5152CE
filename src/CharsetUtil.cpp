
#include <Arduino.h>
#include "CharsetUtil.h"

int enc_get_utf8_size(const unsigned char c) {
    if(c< 0x80) return 0;                // 0xxxxxxx 返回0
    if(c>=0x80 && c<0xC0) return -1;     // 10xxxxxx 返回-1
    if(c>=0xC0 && c<0xE0) return 2;      // 110xxxxx 返回2
    if(c>=0xE0 && c<0xF0) return 3;      // 1110xxxx 返回3
    if(c>=0xF0 && c<0xF8) return 4;      // 11110xxx 返回4
    if(c>=0xF8 && c<0xFC) return 5;      // 111110xx 返回5
    if(c>=0xFC) return 6;                // 1111110x 返回6
}

int enc_utf8_to_unicode_one(const unsigned char* utf8, unsigned char *unicode) {
    
    // b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...  
    char b1, b2, b3, b4, b5, b6;  
  
    *unicode = 0x0; // 把 *Unic 初始化为全零  
    int utfbytes = enc_get_utf8_size(*utf8);  
    unsigned char *pOutput = unicode;  
  
    switch ( utfbytes ) {  
        case 0:  
            *pOutput     = *utf8;
            return 1;
        case 2:  
            b1 = *utf8;  
            b2 = *(utf8 + 1);  
            if ( (b2 & 0xE0) != 0x80 )  
                return 0;  
            *pOutput     = (b1 << 6) + (b2 & 0x3F);  
            *(pOutput+1) = (b1 >> 2) & 0x07;
            return 2;
        case 3:  
            b1 = *utf8;  
            b2 = *(utf8 + 1);  
            b3 = *(utf8 + 2);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b2 << 6) + (b3 & 0x3F);  
            *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);
            return 2;
        case 4:  
            b1 = *utf8;  
            b2 = *(utf8 + 1);  
            b3 = *(utf8 + 2);  
            b4 = *(utf8 + 3);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b3 << 6) + (b4 & 0x3F);  
            *(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);  
            *(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);  
            return 3;
        case 5:  
            b1 = *utf8;  
            b2 = *(utf8 + 1);  
            b3 = *(utf8 + 2);  
            b4 = *(utf8 + 3);  
            b5 = *(utf8 + 4);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b4 << 6) + (b5 & 0x3F);  
            *(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);  
            *(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);  
            *(pOutput+3) = (b1 << 6);  
            return 4;
        case 6:  
            b1 = *utf8;  
            b2 = *(utf8 + 1);  
            b3 = *(utf8 + 2);  
            b4 = *(utf8 + 3);  
            b5 = *(utf8 + 4);  
            b6 = *(utf8 + 5);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)  
                    || ((b6 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b5 << 6) + (b6 & 0x3F);  
            *(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);  
            *(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);  
            *(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);
            return 4;
        default:  
            return 0;
    }
    return 0;
}

String utf8_unicode(const String &strutf8) {
  String strunicode = "";
  int idx = 0;
  while (idx < strutf8.length()) {
    char ch = strutf8.charAt(idx++);
    unsigned char oneutf8[6];
    const int utf8_len = enc_get_utf8_size(ch);
    int readed = 1;
    oneutf8[0] = ch;
    while (idx < strutf8.length() && readed < utf8_len)  {
      oneutf8[readed++] = strutf8.charAt(idx++);
    }
    unsigned char oneunicode[6] = {0};
    int unicode_len = enc_utf8_to_unicode_one(oneutf8, oneunicode);

    for (int i = 0; i < unicode_len;) {
      strunicode += (char)oneunicode[i++];
    }
  }
  return strunicode;  
}
