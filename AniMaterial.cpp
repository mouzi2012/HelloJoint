#include "AniMaterial.h"
#include <string.h>
void AniMaterial::WriteToFile(FILE* pFile)
{
	float* pDataBuffer =new float[3*11];
	float* pBuffer =pDataBuffer;
	float* pVecBuffer =new float[3];
	{	
		getGlmVec3Tofloat(Emissive,pVecBuffer);
		memcpy(pBuffer,pVecBuffer,sizeof(float)*3);
		pBuffer +=3;

		getGlmVec3Tofloat(EmissiveFactor,pVecBuffer);
		memcpy(pBuffer,pVecBuffer,sizeof(float)*3);
		pBuffer +=3;

		getGlmVec3Tofloat(Ambient,pVecBuffer);
		memcpy(pBuffer,pVecBuffer,sizeof(float)*3);
		pBuffer +=3;

		getGlmVec3Tofloat(AmbientFactor,pVecBuffer);
		memcpy(pBuffer,pVecBuffer,sizeof(float)*3);
		pBuffer +=3;

		getGlmVec3Tofloat(Diffuse,pVecBuffer);
		memcpy(pBuffer,pVecBuffer,sizeof(float)*3);
		pBuffer +=3;

		getGlmVec3Tofloat(DiffuseFactor,pVecBuffer);
		memcpy(pBuffer,pVecBuffer,sizeof(float)*3);
		pBuffer +=3;

		getGlmVec3Tofloat(Specular,pVecBuffer);
		memcpy(pBuffer,pVecBuffer,sizeof(float)*3);
		pBuffer +=3;

		getGlmVec3Tofloat(SpecularFactor,pVecBuffer);
		memcpy(pBuffer,pVecBuffer,sizeof(float)*3);
		pBuffer +=3;

		getGlmVec3Tofloat(Shininess,pVecBuffer);
		memcpy(pBuffer,pVecBuffer,sizeof(float)*3);
		pBuffer +=3;

		getGlmVec3Tofloat(Reflection,pVecBuffer);
		memcpy(pBuffer,pVecBuffer,sizeof(float)*3);
		pBuffer +=3;

		getGlmVec3Tofloat(ReflectionFactor,pVecBuffer);
		memcpy(pBuffer,pVecBuffer,sizeof(float)*3);
		pBuffer +=3;
	}
	delete [] pVecBuffer;
	fwrite(pDataBuffer,sizeof(float)*3*11,1,pFile);
	delete [] pDataBuffer;
}
void AniMaterial::ReadFromFile(FILE* pFile)
{

	float* pDataBuffer =new float[3*11];
	float* pBuffer =pDataBuffer;
	fread(pDataBuffer,sizeof(float)*3*11,1,pFile);
	pBuffer = GetOneData(Emissive,pBuffer);	
	pBuffer = GetOneData(EmissiveFactor,pBuffer);	
	pBuffer = GetOneData(Ambient,pBuffer);	
	pBuffer = GetOneData(AmbientFactor,pBuffer);	
	pBuffer = GetOneData(Diffuse,pBuffer);	
	pBuffer = GetOneData(DiffuseFactor,pBuffer);	
	pBuffer = GetOneData(Specular,pBuffer);	
	pBuffer = GetOneData(SpecularFactor,pBuffer);	
	pBuffer = GetOneData(Shininess,pBuffer);	
	pBuffer = GetOneData(Reflection,pBuffer);	
	pBuffer = GetOneData(ReflectionFactor,pBuffer);	
	delete [] pDataBuffer;
}
void AniMaterial::getGlmVec3Tofloat(const glm::vec3& vec,float* buffer)
{
	*buffer++ = vec[0];
	*buffer++ = vec[1];
	*buffer++ = vec[2];
}

float* AniMaterial::GetOneData(glm::vec3& vec,float* buffer)
{
	vec[0] =*buffer++;
	vec[1] =*buffer++;
	vec[2] =*buffer++;
	return buffer;
}
