#include "AniBone.h"
#include "AniVertex.h"
int AniBone::m_idGenerator=0;
AniBone* AniBone::m_rootBone=NULL;
FILE* AniBone::m_fileReader=NULL;
FILE* AniBone::m_fileWriter=NULL;

float* GetGlmMat4Buffer(const glm::mat4& mat,float* pBuffer)
{
	for(int i=0;i<4;++i)
	{
		for(int k=0;k<4;++k)
		{
			*pBuffer++ = mat[i][k];
		}
	}
	return pBuffer;
}
float* GetGlmMat4FromBuffer(glm::mat4& mat,float* pBuffer)
{
	for(int i=0;i<4;++i)
	{
		for(int k=0;k<4;++k)
		{
			mat[i][k] = *pBuffer++ ;
		}
	}
	return pBuffer;
}
AniBone::AniBone()
{
	m_boneId = -1;
	m_parentBoneId = -1;
	memset(m_boneName,0,sizeof(char)*NAMELENG);		
	m_aniFrameSize = 0;
	m_parent = NULL;
	m_childSize = 0;
	m_vertexOffsetMatrix = glm::mat4(1.0f);

	m_boneSize = -1.f;
	m_boneLength = -1.f;
	m_boneType = -1;
	m_boneDefaultSize = -1.f;
	m_boneDefaultLength = -1.f;
	m_isRoot = false;

#ifdef HAVEFBX
	m_node = NULL;
#endif

}
AniBone::~AniBone()
{
}
void AniBone::SetParent(AniBone* pAniBone)
{
	m_parent = pAniBone;
}

#ifdef HAVEFBX
void AniBone::BuildTree(FbxNode* pNode,bool isRoot)
{
	m_isRoot = isRoot;
	m_node = pNode;
	const char* nodeName = pNode->GetName();
	strcpy(m_boneName,nodeName);
	FbxSkeleton* pSkeleton = pNode->GetSkeleton();
	m_boneType = pSkeleton->GetSkeletonType();
	m_boneSize = pSkeleton->Size;
	m_boneLength = pSkeleton->LimbLength;
	m_boneDefaultSize = pSkeleton->sDefaultSize;
	m_boneDefaultSize = pSkeleton->sDefaultLimbLength;
	m_childSize = pNode->GetChildCount(); 
	for(int i = 0; i < m_childSize ; i++)
	{
		AniBone* child = new AniBone();
		child->SetParent(this);
		child->BuildTree(pNode->GetChild(i),false);
		AddChilds(child);
	}

}
AniBone* AniBone::FindBone(FbxNode* pNode)
{
	if(m_node == pNode)
	{
		return this;
	}
	else
	{
		for(int i=0; i < (int)m_childs.size();++i)
		{
			if(m_childs[i]->FindBone(pNode) != NULL)
			{
				return m_childs[i];
			}
		}
	}
	return NULL;
}
FbxNode* AniBone::GetBoneFbxNode()
{
	return m_node;
}
void AniBone::SetOffsetMatrix(const FbxAMatrix& matrix)
{
	m_OffsetMatrix = matrix;
}
const FbxAMatrix& AniBone::GetOffsetMatrix()
{
	return m_OffsetMatrix;
}
#endif


void AniBone::VisitAllBone(VisitTypeEnum visit)
{
	if(visit == VisitEnd)
	{
		return;
	}
	//printf("name is %s \n",m_boneName);
	switch(visit)
	{
	case VisitGenerateId:
		{
			GenerateBoneId();
		}
		break;
	case VisitWriteToFile:
		{
			WriteBoneToFile();
		}
		break;

	default:
		break;
	}
	for(int i = 0;i < (int)m_childs.size();++i)
	{
		m_childs[i]->VisitAllBone(visit);
	}
}

//we now consider how to write the data to file
void AniBone::WriteBoneToFile()
{
	//now we write the basic data
	int sizeBoneId = sizeof(int);
	int sizeParentBoneId = sizeof(int);

	int sizeBoneName = sizeof(char)*NAMELENG;
	int sizeBoneSize = sizeof(float);
	int sizeBoneLength = sizeof(float);
	int sizeBoneDefaultSize = sizeof(float);
	int sizeBoneDefaultLength = sizeof(float);
	int sizeBoneType = sizeof(int);

	int sizeChildSize = sizeof(int);
	int sizeAniFrameSize = sizeof(int);
	int totalBoneDataSize = sizeBoneId+sizeParentBoneId
						  +sizeBoneName+sizeBoneSize+sizeBoneLength+sizeBoneDefaultSize+sizeBoneDefaultLength+sizeBoneType
						  +sizeChildSize+sizeAniFrameSize;
	char* pBoneDataBuffer = new char[totalBoneDataSize];
	char* pBoneDataBufferTmp = pBoneDataBuffer;

	memcpy(pBoneDataBufferTmp,&m_boneId,sizeBoneId);
	pBoneDataBufferTmp +=sizeBoneId;
	memcpy(pBoneDataBufferTmp,&m_parentBoneId,sizeParentBoneId);
	pBoneDataBufferTmp +=sizeParentBoneId;
	
	memcpy(pBoneDataBufferTmp,m_boneName,sizeBoneName);
	pBoneDataBufferTmp +=sizeBoneName;
	memcpy(pBoneDataBufferTmp,&m_boneSize,sizeBoneSize);
	pBoneDataBufferTmp +=sizeBoneSize;
	memcpy(pBoneDataBufferTmp,&m_boneLength,sizeBoneLength);
	pBoneDataBufferTmp +=sizeBoneLength;
	memcpy(pBoneDataBufferTmp,&m_boneDefaultSize,sizeBoneDefaultSize);
	pBoneDataBufferTmp +=sizeBoneDefaultSize;
	memcpy(pBoneDataBufferTmp,&m_boneDefaultLength,sizeBoneDefaultLength);
	pBoneDataBufferTmp +=sizeBoneDefaultLength;
	memcpy(pBoneDataBufferTmp,&m_boneType,sizeBoneType);
	pBoneDataBufferTmp +=sizeBoneType;

	memcpy(pBoneDataBufferTmp,&m_childSize,sizeChildSize);
	pBoneDataBufferTmp +=sizeChildSize;
	memcpy(pBoneDataBufferTmp,&m_aniFrameSize,sizeAniFrameSize);
	pBoneDataBufferTmp +=sizeAniFrameSize;
	fwrite(&totalBoneDataSize,sizeof(int),1,m_fileWriter);
	fwrite(pBoneDataBuffer,totalBoneDataSize,1,m_fileWriter);
	delete [] pBoneDataBuffer ;
	//now we write the offset matrix
	float pMat4Buffer[16];
	int sizeMat4 = 16*sizeof(float);
	GetGlmMat4Buffer(m_vertexOffsetMatrix,pMat4Buffer);
	fwrite(pMat4Buffer,sizeMat4,1,m_fileWriter);
	//now we write the aniframes
	WriteAniFrameData();	

}
void AniBone::WriteAniFrameData()
{
	int elePerAniStruct = 16*3;
	float* pAniFrameDataBuffer = new float[elePerAniStruct*m_aniFrameSize];
	float* pAniFrameDataBufferTmp = pAniFrameDataBuffer;
	for(int i=0;i < m_aniFrameSize;++i)
	{
		pAniFrameDataBufferTmp = GetGlmMat4Buffer(m_aniFrames[i]->m_localMatrix,pAniFrameDataBufferTmp);	
		pAniFrameDataBufferTmp = GetGlmMat4Buffer(m_aniFrames[i]->m_globalMatrix,pAniFrameDataBufferTmp);	
		pAniFrameDataBufferTmp = GetGlmMat4Buffer(m_aniFrames[i]->m_outputMatrix,pAniFrameDataBufferTmp);	
		
	}
	fwrite(pAniFrameDataBuffer,sizeof(float)*elePerAniStruct*m_aniFrameSize,1,m_fileWriter);
	delete [] pAniFrameDataBuffer;
}
//not finish yet
void AniBone::ReadBoneFromFile()
{
	int sizeBoneId = sizeof(int);
	int sizeParentBoneId = sizeof(int);

	int sizeBoneName = sizeof(char)*NAMELENG;
	int sizeBoneSize = sizeof(float);
	int sizeBoneLength = sizeof(float);
	int sizeBoneDefaultSize = sizeof(float);
	int sizeBoneDefaultLength = sizeof(float);
	int sizeBoneType = sizeof(int);

	int sizeChildSize = sizeof(int);
	int sizeAniFrameSize = sizeof(int);

	int totalBoneDataSize =0; 
	fread(&totalBoneDataSize,sizeof(int),1,m_fileReader);
	char* pBoneDataBuffer = new char[totalBoneDataSize];
	char* pBoneDataBufferTmp = pBoneDataBuffer;
	fread(pBoneDataBuffer,totalBoneDataSize,1,m_fileReader);
	memcpy(&m_boneId,pBoneDataBufferTmp,sizeBoneId);
	pBoneDataBufferTmp +=sizeBoneId;
	memcpy(&m_parentBoneId,pBoneDataBufferTmp,sizeParentBoneId);
	pBoneDataBufferTmp +=sizeParentBoneId;
	
	memcpy(m_boneName,pBoneDataBufferTmp,sizeBoneName);
	pBoneDataBufferTmp +=sizeBoneName;
	memcpy(&m_boneSize,pBoneDataBufferTmp,sizeBoneSize);
	pBoneDataBufferTmp +=sizeBoneSize;
	memcpy(&m_boneLength,pBoneDataBufferTmp,sizeBoneLength);
	pBoneDataBufferTmp +=sizeBoneLength;
	memcpy(&m_boneDefaultSize,pBoneDataBufferTmp,sizeBoneDefaultSize);
	pBoneDataBufferTmp +=sizeBoneDefaultSize;
	memcpy(&m_boneDefaultLength,pBoneDataBufferTmp,sizeBoneDefaultLength);
	pBoneDataBufferTmp +=sizeBoneDefaultLength;
	memcpy(&m_boneType,pBoneDataBufferTmp,sizeBoneType);
	pBoneDataBufferTmp +=sizeBoneType;

	memcpy(&m_childSize,pBoneDataBufferTmp,sizeChildSize);
	pBoneDataBufferTmp +=sizeChildSize;
	memcpy(&m_aniFrameSize,pBoneDataBufferTmp,sizeAniFrameSize);
	pBoneDataBufferTmp +=sizeAniFrameSize;
	delete [] pBoneDataBuffer ;
	//the offset matrix
	float pMat4Buffer[16];
	int sizeMat4 = 16*sizeof(float);
	fread(pMat4Buffer,sizeMat4,1,m_fileReader);
	GetGlmMat4FromBuffer(m_vertexOffsetMatrix,pMat4Buffer);
	//read the aniframes
	ReadAniFrameData();
}
//not finish yet
void AniBone::ReadAniFrameData()
{
	int elePerAniStruct = 16*3;
	float* pAniFrameDataBuffer = new float[elePerAniStruct*m_aniFrameSize];
	float* pAniFrameDataBufferTmp = pAniFrameDataBuffer;
	fread(pAniFrameDataBuffer,sizeof(float)*elePerAniStruct*m_aniFrameSize,1,m_fileReader);
	for(int i=0; i < m_aniFrameSize;++i)
	{
		pAniFrameDataBufferTmp = GetGlmMat4FromBuffer(m_aniFrames[i]->m_localMatrix,pAniFrameDataBufferTmp);	
		pAniFrameDataBufferTmp = GetGlmMat4FromBuffer(m_aniFrames[i]->m_globalMatrix,pAniFrameDataBufferTmp);	
		pAniFrameDataBufferTmp = GetGlmMat4FromBuffer(m_aniFrames[i]->m_outputMatrix,pAniFrameDataBufferTmp);	
	}
	delete [] pAniFrameDataBuffer;

}
void AniBone::GenerateBoneId()
{
	m_boneId = m_idGenerator++;
    m_parentBoneId = -1;
	if(m_parent == NULL)
	{
		return ;
	}
	m_parentBoneId = m_parent->GetBoneId();	
}
int AniBone::GetBoneId()
{
	return m_boneId;
}
void AniBone::GenerateAllId()
{
	VisitAllBone(VisitGenerateId);
}
void AniBone::WriteAllBoneToFile()
{
	//this function must call afeter generateallid!!
	fwrite(&m_idGenerator,sizeof(m_idGenerator),1,m_fileWriter);
	VisitAllBone(VisitWriteToFile);
}
//not finish yet
void AniBone::ReadAllBoneFromFile()
{
	//we first read all the data to memery
	fread(&m_idGenerator,sizeof(m_idGenerator),1,m_fileReader);
	if(m_idGenerator <= 1)
	{
		return;
	}
	ReadBoneFromFile();//read the root bone
	
	//read other data
	AniBone* pBoneBuffer=new AniBone[m_idGenerator-1];
	for(int i = 0;i < m_idGenerator-1 ; ++i)
	{
		(pBoneBuffer+i)->ReadBoneFromFile();
	}
	//now we build the bone tree!!
	for(int i = 0;i < m_idGenerator-1 ; ++i)
	{
		AniBone* pCur = pBoneBuffer+i;
		AniBone* pParent = FindParent(pCur->GetParentBoneId());
		if(pParent != NULL)
		{
			pParent->AddChilds(pCur);	
			pCur->SetParent(pParent);	
		}
		else
		{
			printf("hehhehehehe can not find the bone parent !!!!!!!!\n");
			return;
		}
	}
	
}
void AniBone::InitStaticData()
{
	m_idGenerator = 0;
	m_rootBone =NULL;
	m_fileReader = NULL;
	m_fileWriter = NULL;
		
}


void AniBone::SetVertexOffsetMatrix(const glm::mat4& mat)
{
	m_vertexOffsetMatrix = mat;
}
const glm::mat4& AniBone::GetVertexOffsetMatrix()
{
	return m_vertexOffsetMatrix;
}
AniBone* AniBone::GetParent()
{
	return m_parent;
}
//current we just set the first frame to the bind matrix !!! the order control is always 
//by the programer self!!
void AniBone::SetBindAniMatrix(AniMatrix* pAniMatrix)
{
	m_aniFrames.push_back(pAniMatrix);
}
void AniBone::AddAniMatrix(AniMatrix* pAniMatrix)
{
	m_aniFrames.push_back(pAniMatrix);
}

void AniBone::OpenFileWriter()
{
	const char* pFileName ="cheqiAniDatas";
	m_fileWriter = fopen (pFileName, "wb");
}
void AniBone::CloseFileWriter()
{
	fclose(m_fileWriter);
}
FILE* AniBone::GetFileWriter()
{
	return m_fileWriter; 
}
void AniBone::WriteTheHeader(AniDataFileHeader& header)
{
	header.headerSize = sizeof(header);
	fwrite(&header,sizeof(header),1,m_fileWriter);
}
void AniBone::ReadTheHeader(AniDataFileHeader& header)
{
	fread(&header,sizeof(header),1,m_fileReader);
}
AniBone* AniBone::GetRootBone()
{
	if(NULL == m_rootBone)
	{
		m_rootBone =new AniBone;
		m_rootBone->SetRoot(true);
	}
	return m_rootBone;
}
void AniBone::SetRoot(bool root)
{
	m_isRoot = root;
}
int AniBone::GetParentBoneId()
{
	return m_parentBoneId;
}
AniBone* AniBone::FindParent(int Id)
{
	if(m_boneId == Id)
	{
		return this;
	}	
	for(int i = 0;i < (int)m_childs.size();++i)
	{
		AniBone* result = m_childs[i]->FindParent(Id);
		if(result != NULL)
		{
			return result;
		}
	}
	return NULL;
}
void AniBone::AddChilds(AniBone* pAniBone)
{
	m_childs.push_back(pAniBone);
}
