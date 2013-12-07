/**
@author jtnimoy
*/

#include "FileUtils.h"


long FileUtils::getFileSize( char *fname){
	FILE *fp;
	long fsize;
	if (fname == NULL){ //filename was null
		return -1;
	}
	fp = fopen((const char*)fname, "rb");
	if (fp == NULL){ //file stream failed
		return -2;
	}
	if(fseek(fp, 0, SEEK_END)!=0){ //fseek failed
	}
	fsize = ftell(fp);
	fclose(fp);
	return fsize;
}

//-----------------------------------------------------------------

unsigned char* FileUtils::stringFromFile(char *fname){
	long fsize;
	unsigned char *a;

	if(fname==NULL){//filename was null
	  return NULL;
	}
	fsize = getFileSize(fname);

	if(fsize==0){//file size was zero
	  return NULL;
	}
	
	a = new unsigned char[fsize+1];
	
	if(a==NULL){//memory allocation failed
	  return NULL;
	}
	
	getFile((char*)fname,a,fsize);
	a[fsize]=0;
	return a;
}
//-----------------------------------------------------------------

void FileUtils::getFile( char *filename, unsigned char *buff, int length){
	unsigned char c=0;
	long buffcnt=0;
	long i=0;
	ifstream f;
	f.open((const char*)filename, ios::binary);
	f.read((char*)buff, length);
	f.close();
}
