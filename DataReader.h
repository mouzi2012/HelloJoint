#ifndef DATAREADER_H
#define DATAREADER_H
// this is the file to parse data from .fbx file do not lose it again!!!!
#include <vector>
using std::vector;
#include <fbxsdk.h>

#define NAMEL 256
class AniBone;
class AniMaterial;
struct AniVertexInfo;

struct Index2Bone
{
	int index;
	vector<AniBone*> bones;
	vector<float> boneFactors;
};
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
	void ReadBone();
	void ReadBoneAni();
	void ConnectBone2Vertex();
	FbxNode* GetRootBone();
private:
	char m_target[NAMEL];
	FbxManager* m_manager;
	FbxScene* m_scene;
// the fbx detail data
	AniBone* m_bone;
	AniVertexInfo* m_vertexInfo;
	vector<Index2Bone> m_index2bone;
};
#endif
