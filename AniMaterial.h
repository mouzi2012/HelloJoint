#ifndef ANIMATERIAL_H
#define ANIMATERIAL_H
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class AniMaterial
{
public:
	// the lambert struct!!
	glm::vec3 Emissive;
	glm::vec3 EmissiveFactor;
	glm::vec3 Ambient;
	glm::vec3 AmbientFactor;
	glm::vec3 Diffuse;
	glm::vec3 DiffuseFactor;
	
	// the phong struct!!
	glm::vec3 Specular;
	glm::vec3 SpecularFactor;
	glm::vec3 Shininess;
	glm::vec3 Reflection;
	glm::vec3 ReflectionFactor;
	
public:
	AniMaterial(){};
	~AniMaterial(){};	
	void WriteToFile(FILE* pFile);
	void ReadFromFile(FILE* pFile);
	void getGlmVec3Tofloat(const glm::vec3& vec,float* buffer);
	float* GetOneData(glm::vec3& vec,float* buffer);
};
#endif
