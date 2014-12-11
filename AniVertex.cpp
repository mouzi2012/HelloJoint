#include "AniVertex.h"

bool is_near(float v1, float v2){
	return fabs( v1-v2 ) < 0.01f;
}

AniVertex::AniVertex()
{
	memset(m_dataPoint,0,sizeof(float)*VTN);
	memset(m_boneArray,0,sizeof(AniBone*)*MAXBN);
	memset(m_boneWeights,0,sizeof(float)*MAXBN);
	memset(m_boneID,-1,sizeof(int)*MAXBN);
	m_boneSize = 0;
}
AniVertex::~AniVertex()
{
}
void AniVertex::SetPoint(float x,float y,float z)
{
	m_dataPoint[0] = x;
	m_dataPoint[1] = y;
	m_dataPoint[2] = z;
}
void AniVertex::AddBone(AniBone* pAniBone,float weight)
{
	if(m_boneSize ==  MAXBN )
	{
		printf("too much bones!!!\n");
		return ;
	}
	m_boneArray[m_boneSize]=pAniBone;
	m_boneWeights[m_boneSize]=weight;
	m_boneID[m_boneSize]=pAniBone->GetBoneId();
	++m_boneSize;	
}

void AniVertex::WriteToFile(FILE* pFile)
{
	int pointSize = VTN*sizeof(float);
	int boneSizeSize =sizeof(int);
	int boneWeightSize =sizeof(float)*m_boneSize;
	int boneIDSize =sizeof(int)*m_boneSize;
	int totalSize =pointSize+boneSizeSize+boneWeightSize+boneIDSize;
	char* pBuffer=new char[totalSize];
	memcpy(pBuffer,m_dataPoint,pointSize);
	memcpy(pBuffer+pointSize,&m_boneSize,boneSizeSize);
	memcpy(pBuffer+pointSize+boneSizeSize,m_boneWeights,boneWeightSize);
	memcpy(pBuffer+pointSize+boneSizeSize+boneWeightSize,m_boneID,boneIDSize);
	fwrite(&totalSize,sizeof(int),1,pFile);	
	fwrite(pBuffer,totalSize,1,pFile);
	delete [] pBuffer;
}

int AniVertex::ReadFromFile(FILE* pFile)
{
	int sizeToRead=0;
	fread(&sizeToRead,sizeof(int),1,pFile);
	char* pBuffer=new char[sizeToRead];
	fread(pBuffer,sizeToRead,1,pFile);

	int pointSize = VTN*sizeof(float);
	int boneSizeSize =sizeof(int);
	memcpy(m_dataPoint,pBuffer,pointSize);
	memcpy(&m_boneSize,pBuffer+pointSize,boneSizeSize);
	int boneWeightSize =sizeof(float)*m_boneSize;
	int boneIDSize =sizeof(int)*m_boneSize;
	memcpy(m_boneWeights,pBuffer+pointSize+boneSizeSize,boneWeightSize);
	memcpy(m_boneID,pBuffer+pointSize+boneSizeSize+boneWeightSize,boneIDSize);

	delete [] pBuffer;
	return 0;
}



// AniVertexInfo
void AniVertexInfo::SetVertexBoneInfo(int index,AniBone* pAniBone,float weight)
{
	(vertex+index)->AddBone(pAniBone,weight);
}
void AniVertexInfo::AddMaterial(AniMaterial* pAniMaterial)
{
	material = pAniMaterial;
}
void AniVertexInfo::AssambleVertexData()
{
	for(int i=0;i < vertexIndexSize;++i)
	{
		int index = -1;
		bool find = false;
		for(int k=0;k < (int)vertexForWrite.size();++k)
		{
			if(
				is_near((vertex+*(vertexIndex+i))->GetX(),vertexForWrite[k]->GetX())&&
				is_near((vertex+*(vertexIndex+i))->GetY(),vertexForWrite[k]->GetY())&&
				is_near((vertex+*(vertexIndex+i))->GetZ(),vertexForWrite[k]->GetZ())&&
				is_near(*(vertexUV+i*2),UVForWrite[k*2])&&
				is_near(*(vertexUV+i*2+1),UVForWrite[k*2+1])
			){
				index = k;
				find = true;
				break;
			}	
		}
		if(find)
		{
			triangleIndex.push_back(index);
		}
		else
		{
			vertexForWrite.push_back(vertex+*(vertexIndex+i));
			UVForWrite.push_back(*(vertexUV+i*2));
			UVForWrite.push_back(*(vertexUV+i*2+1));
			triangleIndex.push_back(vertexForWrite.size()-1);
		}
	}

	vertexForWriteSize = vertexForWrite.size();
	UVForWriteSize = UVForWrite.size();
    triangleForWriteSize = triangleIndex.size();	
}
//current we do not change startPosition
//startPosition current is not used
void AniVertexInfo::WriteToFile(FILE* pFile,AniDataFileHeader& header)
{
	//start write the vertex data
	header.vertexStartPosition = sizeof(header);
	fwrite(&vertexForWriteSize,sizeof(int),1,pFile);
	for(int i=0;i<(int)vertexForWrite.size();++i)
	{
		vertexForWrite[i]->WriteToFile(pFile);
	}
	int vertexEndPosition = ftell(pFile);
	header.vertexSize = vertexEndPosition - header.vertexStartPosition;
	//vertex data end
	//start write the uv data
	header.UVStartPosition =vertexEndPosition;
	fwrite(&UVForWriteSize,sizeof(int),1,pFile);
	fwrite(&UVForWrite[0],UVForWrite.size()*sizeof(float),1,pFile);	
	int UVEndPosition = ftell(pFile);
	header.UVSize = UVEndPosition - header.UVStartPosition;
	//uv data end
	//index data start
	header.indexStartPosition = UVEndPosition;
	fwrite(&triangleForWriteSize,sizeof(int),1,pFile);
	fwrite(&triangleIndex[0],triangleIndex.size()*sizeof(int),1,pFile);
	int indexEndPosition = ftell(pFile);
	header.indexSize =indexEndPosition - header.indexStartPosition;
	//index data end
	//material data start
	header.materialStartPosition = indexEndPosition;	
	material->WriteToFile(pFile);
	int materialEndPosition =ftell(pFile);
	header.materialSize = materialEndPosition - header.materialStartPosition;
	//material data end
}
int AniVertexInfo::ReadFromFile(FILE* pFile,AniDataFileHeader& header)
{
	//we consider the header is already read in and the pFile is current point to the vertex~~
	//start read the vertex data
	fread(&vertexForWriteSize,sizeof(int),1,pFile);
	for(int i=0;i<vertexForWriteSize;++i)
	{
		AniVertex* pVertex=new AniVertex;
		pVertex->ReadFromFile(pFile);
		vertexForWrite.push_back(pVertex);
	}
	fread(&UVForWriteSize,sizeof(int),1,pFile);
	UVForWrite.resize(UVForWriteSize);
	fread(&UVForWrite[0],sizeof(float)*UVForWriteSize,1,pFile);

	fread(&triangleForWriteSize,sizeof(int),1,pFile);
	triangleIndex.resize(triangleForWriteSize);
	fread(&triangleIndex[0],sizeof(int)*triangleForWriteSize,1,pFile);
	//not finish yet!!!
	material->ReadFromFile(pFile);
	return 0;
}
