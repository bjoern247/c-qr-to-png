// Created by Björn Kneisel on 14.12.2022 for poe GmbH & Co. KG

#include "qrcodegen.h"
#include "TinyPngOut.h"
#include <stdio.h>

int generateQrCode(char * qrUrl, char * fn);

int writeToPNG(int, int, char *, char *);

int _writeToPNG(char *, int, int, char *);

uint32_t _imgSizeWithBorder(char *, int);

uint32_t _imgSize(char *, int);