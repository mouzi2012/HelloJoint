#ifndef ANIBONE_H
#define ANIBONE_H

#define HAVEFBX 1
#ifdef HAVEFBX
#include <fbxsdk.h>
#else
#include <stdio.h>
#include <string.h>
#endif

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class AniDataFileHeader;
using std::vector;
struct AniMatrix
{
	glm::mat4 m_localMatrix;
	glm::mat4 m_globalMatrix;
	glm::mat4 m_outputMatrix;//this value equal m_globalMatrix*m_vertexOffsetMatrix!!
};
#define NAMELENG 16
#define MAXCHILDEN 16
class AniBone
{
public:
	AniBone();
	~AniBone();
public:
	void SetParent(AniBone* pAniBone);
public:
	enum VisitTypeEnum {VisitGenerateId=0,VisitWriteToFile,VisitEnd};
	void GenerateAllId();
	void WriteAllBoneToFile(FILE* pfile);
	void ReadAllBoneFromFile(FILE* pfile);
	int GetBoneId();
public:
	void SetVertexOffsetMatrix(const glm::mat4& mat);
	const glm::mat4& GetVertexOffsetMatrix();

	void SetBindAniMatrix(AniMatrix* pAniMatrix);
	void AddAniMatrix(AniMatrix* pAniMatrix);
public:
	AniBone* GetParent();
protected:
	void VisitAllBone(VisitTypeEnum visit=VisitEnd,void* extra=NULL);
	void GenerateBoneId();
	void WriteBoneToFile(FILE* pfile);
	void WriteAniFrameData(FILE* pfile);
	void ReadBoneFromFile(FILE* pfile);
	void ReadAniFrameData(FILE* pfile);

	void SetRoot(bool root);
	// if we build tree from file we need find function
	AniBone* FindParent(int Id);
	int GetParentBoneId();
	void AddChilds(AniBone* pAniBone);
#ifdef HAVEFBX 
public:
	void BuildTree(FbxNode* pNode,bool isRoot);
	AniBone* FindBone(FbxNode* pNode);
	FbxNode* GetBoneFbxNode();
	void SetOffsetMatrix(const FbxAMatrix& matrix);
	const FbxAMatrix& GetOffsetMatrix();
private:
	FbxNode* m_node;
	FbxAMatrix m_OffsetMatrix;
#endif
//
private:
	int m_boneId;// this is for writing to files!!!
	int m_parentBoneId;// this for writing to files!!!
	char m_boneName[NAMELENG];//read the following from the node!!
	float m_boneSize;
	float m_boneLength;
	float m_boneDefaultSize;
	float m_boneDefaultLength;
	int m_boneType;
//the init value this may not right!!
	vector<AniMatrix*> m_aniFrames;
	int m_aniFrameSize;//default current I set the first frame as the bindpostion for debug!!
	glm::mat4 m_vertexOffsetMatrix;
//the bone struct
	AniBone* m_parent;
	vector<AniBone*> m_childs;
	int m_childSize;
private:
	bool m_isRoot;
private:
	// current we just write the need data as static member 
	// all this should managed by system resource manager!!
	static int m_idGenerator;
	static AniBone* m_rootBone;
public:
	static void InitStaticData();
	static AniBone* GetRootBone();

};
#endif
