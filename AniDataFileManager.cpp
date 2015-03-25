#include "AniDataFileManager.h"
#include "AniBone.h"
#include "AniVertex.h"

AniDataFileManager::AniDataFileManager()
{
	memset(m_file,0,sizeof(FILE*)*(EReader+1));
	memset(m_name,0,sizeof(char)*(EReader+1)*nSize);
}

AniDataFileManager::~AniDataFileManager()
{
}

void AniDataFileManager::OpenFileType(int type)
{
	const char* op[] ={"wb","rb"};
	const char* typeName[] ={"writer","reader"};
	m_file[type] = fopen(m_name[type],op[type]);
	if(!m_file[type])
	{
		printf("can not open file %s !!!!\n",typeName[type]);
	}
}

void AniDataFileManager::CloseFileType(int type)
{
	fclose(m_file[type]);
}


void AniDataFileManager::WriteDataToFile(AniBone* pBone,AniVertexInfo* pVertexInfo)
{
	AniDataFileHeader header;
	FILE* pWriter = m_file[EWriter];
	//occupy the data first
	WriteTheHeader(header,pWriter);
	pVertexInfo->WriteToFile(pWriter,header);
	pBone->WriteAllBoneToFile(pWriter,header);
	//write the real data now
	fseek(pWriter,0,SEEK_SET);
	WriteTheHeader(header,pWriter);
}
void AniDataFileManager::ReadDataFromFile(AniBone*& pBone,AniVertexInfo*& pVertexInfo)
{
	AniDataFileHeader header;
	FILE* pReader = m_file[EReader];
	ReadTheHeader(header,pReader);	
	pVertexInfo = new AniVertexInfo;
	pVertexInfo->ReadFromFile(pReader,header);
	pBone = AniBone::GetRootBone();
	pBone->ReadAllBoneFromFile(pReader,header);
}

void AniDataFileManager::SetFileName(int type,char* name)
{
	strcpy(m_name[type],name);
}
void AniDataFileManager::WriteTheHeader(AniDataFileHeader& header,FILE* pfile)
{
	header.headerSize = sizeof(header);
	fwrite(&header,sizeof(header),1,pfile);
}
void AniDataFileManager::ReadTheHeader(AniDataFileHeader& header,FILE* pfile)
{
	fread(&header,sizeof(header),1,pfile);
}
