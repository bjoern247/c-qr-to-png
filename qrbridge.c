 /* 
 * This "library" bridges the qrcodegen library and the tinyPNGoutput library by nayuki. This is inspired/ported by the cpp implementation of user https://github.com/RaymiiOrg
 * This was written because our old codebase is not able to include external c code that uses streams. 
 * This program just uses pure c code to generate the QR code and write it as a png file.
 * It uses the C-versions of qrcodegen library by nayuki to generate the QR code and the tinyPNGoutput library by nayuki to write the png file.  
 */

#include "qrbridge.h"

int generateQrCode(char * qrUrl, char * fn) {
  int imgSize = 300;
  int minModulePixelSize = 3;
  writeToPNG(imgSize, minModulePixelSize, qrUrl, fn);
}

int writeToPNG(int imgSize, int minModulePixelSize, char * text, char * fn) {
  if ((text != NULL) && (text[0] == '\0')) {
    return 0;
  } 

  char tmpBuffer[qrcodegen_BUFFER_LEN_MAX];
  char qrArray[qrcodegen_BUFFER_LEN_MAX];

  enum qrcodegen_Ecc errCorLvl = qrcodegen_Ecc_LOW;

  qrcodegen_encodeText(text, tmpBuffer, qrArray, errCorLvl, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
  int result = _writeToPNG(qrArray, imgSize, minModulePixelSize, fn);

  return result;
}

int _writeToPNG(char * _qrData, int _size, int _minModulePixelSize, char * fn) {
  FILE *fp; // file pointer
  fp = fopen(fn, "wb");
  if (fp == NULL) {
    return 0;
  }
  int pngWH = _imgSizeWithBorder(_qrData, _size);
  struct TinyPngOut tpo;
  TinyPngOut_init(&tpo, pngWH, pngWH, fp);
  int qrSize = qrcodegen_getSize(_qrData);
  int qrSizeWithBorder = qrcodegen_getSize(_qrData) + 2;
  if (qrSizeWithBorder > _size) {
    return 0; // Error if qrcode doesn't fit
  }

  int qrSizeFitsInMaxImgSizeTimes = _size / qrSizeWithBorder;
  int pixelsWHPerModule = qrSizeFitsInMaxImgSizeTimes;

  if (qrSizeFitsInMaxImgSizeTimes < _minModulePixelSize) {
    return 0; // image would be to small to scan
  }

  uint8_t tmpData[1000000];

  const uint8_t blackPixel = 0x00;
  const uint8_t whitePixel = 0xFF;

  /* The below loop converts the qrData to RGB8.8.8 pixels and writes it with
  * the tinyPNGoutput library. since we probably have requested a larger
  * qr module pixel size we must transform the qrData modules to be larger
  * pixels (than just 1x1). 
  */

  int v = 0;
  // Top border
  for (int i = 0; i < pngWH; i++) { // row
    for (int j = 0; j < pixelsWHPerModule; j++) {
      tmpData[v++] = whitePixel;
      tmpData[v++] = whitePixel;
      tmpData[v++] = whitePixel;
    }
  }

  TinyPngOut_write(&tpo, (const uint8_t *)&tmpData, v/3);
  v = 0;

  for (int qrModuleAtY = 0; qrModuleAtY < qrSize; qrModuleAtY++) {
    for (int col = 0; col < pixelsWHPerModule; col++) {
      // Left border
      for (int i = 0; i < qrSizeFitsInMaxImgSizeTimes; ++i) {
        tmpData[v++] = whitePixel;
        tmpData[v++] = whitePixel;
        tmpData[v++] = whitePixel;
      }
      for (int qrModuleAtX = 0; qrModuleAtX < (qrSize); qrModuleAtX++) {
        for (int row = 0; row < qrSizeFitsInMaxImgSizeTimes; ++row) {
          if (qrcodegen_getModule(_qrData, qrModuleAtX, qrModuleAtY)) {
            tmpData[v++] = blackPixel;
            tmpData[v++] = blackPixel;
            tmpData[v++] = blackPixel;
          } else {
            tmpData[v++] = whitePixel;
            tmpData[v++] = whitePixel;
            tmpData[v++] = whitePixel;
          }
        }
      }
      // border rechts
      for (int i = 0; i < qrSizeFitsInMaxImgSizeTimes; ++i) {
        tmpData[v++] = whitePixel;
        tmpData[v++] = whitePixel;
        tmpData[v++] = whitePixel;
      }

      // write the entire row
      TinyPngOut_write(&tpo, (const uint8_t *)&tmpData, v/3);
      v = 0;
    }
  }

  v = 0;
  // border drunter
  for (int i = 0; i < pngWH; i++) {
    for (int j = 0; j < pixelsWHPerModule; j++) {
      tmpData[v++] = whitePixel;
      tmpData[v++] = whitePixel;
      tmpData[v++] = whitePixel;
    }
  }

  TinyPngOut_write(&tpo, (const uint8_t *)&tmpData, v/3);
  v = 0;
  fclose(fp);
}

uint32_t _imgSizeWithBorder(char * _qrData, int _size) {
  return (_size / (qrcodegen_getSize(_qrData) + 2)) * (qrcodegen_getSize(_qrData) + 2);
}

uint32_t _imgSize(char * _qrData, int _size) {
  return (_size / qrcodegen_getSize(_qrData)) * qrcodegen_getSize(_qrData);
}
