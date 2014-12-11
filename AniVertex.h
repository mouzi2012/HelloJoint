#ifndef ANIVERTEX_H
#define ANIVERTEX_H
#include "AniBone.h"
#include "AniMaterial.h"
#define VTN 3
#define MAXBN 16
class AniVertex
{
public:
	AniVertex();
	~AniVertex();
public:
	void SetPoint(float x,float y,float z);
	void AddBone(AniBone* pAniBone,float weight);
	float GetX()
	{
		return m_dataPoint[0];
	}
	float GetY()
	{
		return m_dataPoint[1];
	}
	float GetZ()
	{
		return m_dataPoint[2];
	}
	void WriteToFile(FILE* pFile);
	int ReadFromFile(FILE* pFile);
private:
	float m_dataPoint[VTN];
	AniBone* m_boneArray[MAXBN];	
	float m_boneWeights[MAXBN];
	int m_boneID[MAXBN];//this for writing to file simple~~
	int m_boneSize;
};

struct AniDataFileHeader
{
	int headerSize;
	int vertexStartPosition;
	int vertexSize;
	int UVStartPosition;
	int UVSize;
	int indexStartPosition;
	int indexSize;
	int materialStartPosition;
	int materialSize;
	int aniBoneStartPosition;
	int aniSize;

};

struct AniVertexInfo
{
public:
	AniVertexInfo()
	{
		SetDefault();
	}
	~AniVertexInfo()
	{
		delete [] vertex;
		delete [] vertexIndex;
		delete [] vertexUV;
		SetDefault();
	}
	void SetDefault()
	{
		vertex = NULL;
		vertexSize = 0;
		vertexIndex = NULL;
		vertexIndexSize = 0;
		material = NULL;
		vertexUV = NULL;
		vertexUVSize = 0;

	}
	void SetVertexBoneInfo(int index,AniBone* pAniBone,float weight);	
	void AddMaterial(AniMaterial* pAniMaterial);
public:
	AniVertex* vertex;
	int vertexSize;
	int* vertexIndex;
	int vertexIndexSize;
	AniMaterial* material;
	float* vertexUV;
	int vertexUVSize;
public:
	void AssambleVertexData();
	void WriteToFile(FILE* pFile,AniDataFileHeader& header);
	int ReadFromFile(FILE* pFile,AniDataFileHeader& header);
public:
	//we assamble the data here to the file
	vector<AniVertex*> vertexForWrite;
	vector<float> UVForWrite;
	vector<int> triangleIndex;
	int vertexForWriteSize;
	int UVForWriteSize;
    int triangleForWriteSize;	

	// the material should write to the file
};
#endif
