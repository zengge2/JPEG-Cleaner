#include "jpeg.h"
#include <iostream>

using namespace std;

INT FileSize(FILE *pFile) {
	long pos;
	fseek(pFile, 0, SEEK_END);
	pos = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	return pos;
}

INT JPEG_Read(FILE *pFileJPEG, UCHAR *pucFileBuffer, UINT *puiFileBufferSize) {
	if (fread(pucFileBuffer, *puiFileBufferSize, 1, pFileJPEG) == 0) {
		return -1;
	}
	return 0;
}

INT JPEG_Open(CHAR *pcFileName, JPEGINFO_t *ptJPEGINFO) {
	UINT uiImageWidth, uiImageHeight;
	UCHAR *pucFileBuffer;
	UINT uiFileBufferSize;
	FILE *pFileJPEG;
	pFileJPEG = fopen(pcFileName,"rb");
	if (pFileJPEG == NULL) {
		return -1;
	}
	uiFileBufferSize = FileSize(pFileJPEG);
	pucFileBuffer = (UCHAR*)malloc(sizeof(UCHAR)*(uiFileBufferSize)) ;
	JPEG_Read(pFileJPEG, pucFileBuffer, &uiFileBufferSize);
	fclose(pFileJPEG);
	if (JPEG_HeaderParser(ptJPEGINFO, (const UCHAR*)pucFileBuffer,
	                      &uiFileBufferSize,&uiImageWidth,&uiImageHeight) < 0) {
		return -2;
	}
	return 0;
}

INT GetJPEG_ImageSize(CHAR *pcFileName, UINT *puiWidth, UINT *puiHeight) {
	JPEGINFO_t tJPEGINFO;
	if (JPEG_Open(pcFileName,&tJPEGINFO) < 0) {
		return -1;
	}
	*puiWidth  = tJPEGINFO.uiWidth;
	*puiHeight = tJPEGINFO.uiHeight;
	return 0;
}

void JPEG_ParaseSOF(JPEGINFO_t *ptJPEGINFO, const UCHAR *pucStream) {

	ptJPEGINFO->uiHeight = BYTEtoWORD(pucStream+3);
	ptJPEGINFO->uiWidth  = BYTEtoWORD(pucStream+5);

}
INT JPEG_MarkerParse(JPEGINFO_t *ptJPEGINFO, const UCHAR *pucStream,
                     UINT *puiStreamSize) {

	UINT uiMarkerSOS = 0, uiCheckFormat = 1, uiChuckLen;
	INT iMaker;
	while (!uiMarkerSOS && uiCheckFormat && puiStreamSize > 0) {
		if (*pucStream != 0xFF) {
			uiCheckFormat=0;
		}
		while (*pucStream == 0xFF && puiStreamSize >0) {
			pucStream++;
			(*puiStreamSize)--;
		}
		iMaker = *pucStream++;
		(*puiStreamSize)--;
		uiChuckLen = BYTEtoWORD(pucStream);
		switch (iMaker) {
			case SOF:
				JPEG_ParaseSOF(ptJPEGINFO,pucStream);
				break;
			case SOS:
				uiMarkerSOS = 1;
				break;
		}
		pucStream = pucStream + uiChuckLen;
	}
	if (!uiCheckFormat || puiStreamSize == 0) {
		return -2;
	}
	return 0;
}

INT JPEG_HeaderParser(JPEGINFO_t *ptJPEGINFO ,const UCHAR *pucFileBuffer,
                      UINT *puiFileBufferSize, UINT *puiImageWidth,
                      UINT* puiImageHeight) {
	const UCHAR *pucStartStream = pucFileBuffer+2;
	*puiFileBufferSize = *puiFileBufferSize-2;
	if (pucFileBuffer[0] != 0xFF || pucFileBuffer[1] != SOI) {
		return -2;
	}
	if (JPEG_MarkerParse(ptJPEGINFO, pucStartStream, puiFileBufferSize) < 0) {
		return -2;
	}
	return 0;
}

bool isExt(string path) {
	int pos=path.find_last_of('.');
	string n=path.substr(pos+1,path.size()-1);
	if(strcmp(n.c_str(),"jpeg")==0
	||strcmp(n.c_str(),"jpg")==0
	||strcmp(n.c_str(),"JPG")==0
	||strcmp(n.c_str(),"JPEG")==0)
		return true;
	else return false;
}

void FindDir(char *lpPath) {
	char szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	strcpy(szFind,lpPath);
	strcat(szFind,"\\*.*");
	HANDLE hFind=::FindFirstFile(szFind,&FindFileData);
	if(INVALID_HANDLE_VALUE == hFind) return;
	while(TRUE) {
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if(FindFileData.cFileName[0]!='.') {
				string str = string(lpPath)+string("\\")+string(FindFileData.cFileName);
				FindDir(const_cast<char*>(str.c_str()));
			}
		} else {
			string pth = string(lpPath)+"\\"+string(FindFileData.cFileName);
			if(isExt(pth)) {
				JPEGINFO_t info;
				if(JPEG_Open(const_cast<char*>(pth.c_str()),&info)!=0) {
					remove(pth.c_str());
					cout << pth << " has been deleted." << endl;	
				}
				
			}
		}
		if(!FindNextFile(hFind,&FindFileData)) break;
	}
	FindClose(hFind);
}

int main() {
	char szPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szPath);
	FindDir(szPath);
	return 0;
}
