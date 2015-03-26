#include "DataReader.h"
#include "AniDataFileManager.h"
int main()
{
	const char* pFbxFile = "cheqiyu.FBX";
	const char* pOutputFile = "ani.data";

	AniDataFileManager manager;
	manager.SetFileName(0,pOutputFile);	
	manager.OpenFileType(0);	

	DataReader reader;
	reader.SetTarget(pFbxFile);
	reader.InitFbxData();
	reader.StartParse();
	reader.WriteToFile(manager);
	return 0;
}
