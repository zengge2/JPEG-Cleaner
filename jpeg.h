#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define SOI	0xD8
#define SOF	0xC0
#define SOS	0xDA

//#define S_OK 0
#define S_FAIL -1

#define BYTEtoWORD(x) (((x)[0]<<8)|(x)[1])

typedef struct {
	UINT uiWidth;
	UINT uiHeight;
} JPEGINFO_t;

INT GetJPEG_ImageSize(CHAR *pcFileName, UINT *puiWidth, UINT *puiHeight);

INT FileSize(FILE *pFile);

INT JPEG_Read(FILE *pFileJPEG, UCHAR *pucFileBuffer, UINT *puiFileBufferSize);

INT JPEG_Open(CHAR *pcFileName, JPEGINFO_t *ptJPEGINFO);

void JPEG_ParaseSOF(JPEGINFO_t *ptJPEGINFO, const UCHAR *pucStream);

INT JPEG_MarkerParse(JPEGINFO_t *ptJPEGINFO, const UCHAR *pucStream,
                     UINT *puiStreamSize);

INT JPEG_HeaderParser(JPEGINFO_t *ptJPEGINFO ,const UCHAR *pucFileBuffer,
                      UINT *puiFileBufferSize, UINT *puiImageWidth,
                      UINT* puiImageHeight);
