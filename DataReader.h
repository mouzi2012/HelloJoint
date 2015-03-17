#ifndef DATAREADER_H
#define DATAREADER_H
// this is the file to parse data from .fbx file do not lose it again!!!!
#include <fbxsdk.h>
#define NAMEL 256
class DataReader
{
public:
	DataReader();
	~DataReader();
public:
	void SetTarget(const char* name);
private:
	char m_target[NAMEL];
	FbxManager* m_manager;
	FbxScene* m_scene;
};
#endif
