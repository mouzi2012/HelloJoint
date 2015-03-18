#ifndef DATAREADER_H
#define DATAREADER_H
// this is the file to parse data from .fbx file do not lose it again!!!!
#include <fbxsdk.h>
#define NAMEL 256
class AniBone;
class AniMaterial;
struct AniVertexInfo;
class DataReader
{
public:
	DataReader();
	~DataReader();
public:
	void SetTarget(const char* name);
	void InitFbxData();
	void StartParse();
protected:
	void ParseBone();
	void ParseVertex();
	void ParseMaterial();
private:
	char m_target[NAMEL];
	FbxManager* m_manager;
	FbxScene* m_scene;
// the fbx detail data
	AniBone* m_bones;
	AniVertexInfo* m_vertexInfo;
};
#endif
