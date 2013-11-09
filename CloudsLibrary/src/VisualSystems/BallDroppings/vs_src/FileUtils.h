#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

/**
	some very high level functions for working with files
	@author jtnimoy
*/

#include <fstream>

using namespace std;


class FileUtils{
	public:
		/**
			dynamically allocate a char array and reads a file into it
			callers of this function: remember to delete the char array
			when you are done with it!
		*/
		static unsigned char* stringFromFile(char *fname);
		
		/**
			calculate the size of a file in bytes
		*/
		static long getFileSize( char *fname);
		
		/**
			copy LENGTH number of bytes into BUFF from FILENAME
		*/
		static void getFile( char *filename, unsigned char *buff, int length);
};

#endif