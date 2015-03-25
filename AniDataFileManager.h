#ifndef ANIDATAFILEMANAGER_H
#define ANIDATAFILEMANAGER_H

#include <stdio.h>
#include <string.h>

class AniBone;
class AniVertexInfo;
class AniDataFileManager
{
public:
	AniDataFileManager();
	~AniDataFileManager();

public:
	void SetFileName(int type,char* name);
public:
	void WriteDataToFile(AniBone* pBone,AniVertexInfo* pVertexInfo);
	void ReadDataFromFile(AniBone*& pBone,AniVertexInfo*& pVertexInfo);
	
	void OpenFileType(int type);
	void CloseFileType(int type);
private:
	enum EManagerType {EWriter=0,EReader};
	static const int nSize=256;
	FILE* m_file[EReader+1];
	char m_name[EReader+1][nSize];
};
#endif
