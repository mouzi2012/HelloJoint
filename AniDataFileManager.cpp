#include "AniDataFileManager.h"
AniDataFileManager::AniDataFileManager()
{

	memset(m_nameWriter,0,nSize);
	memset(m_nameReader,0,nSize);
	m_fileWriter = NULL;
	m_fileReader = NULL;
}

AniDataFileManager::~AniDataFileManager()
{
}

void AniDataFileManager::OpenFileWriter()
{
}
void AniDataFileManager::CloseFileWriter()
{
}

void AniDataFileManager::OpenFileReader()
{
}
void AniDataFileManager::CloseFileReader()
{
}

void AniDataFileManager::WriteDataToFile()
{
}
void AniDataFileManager::ReadDataFromFile()
{
}

