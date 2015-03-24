#ifndef ANIDATAFILEMANAGER_H
#define ANIDATAFILEMANAGER_H

#include <stdio.h>
#include <string.h>

class AniDataFileManager
{
public:
	AniDataFileManager();
	~AniDataFileManager();

public:
	void OpenFileWriter();
	void CloseFileWriter();

	void OpenFileReader();
	void CloseFileReader();

	void WriteDataToFile();
	void ReadDataFromFile();

private:
	static const int nSize=256;
	char m_nameWriter[nSize];
	char m_nameReader[nSize];
	FILE* m_fileWriter;
	FILE* m_fileReader;
};
#endif
