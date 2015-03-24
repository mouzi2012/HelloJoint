#include "DataReader.h"
#include "AniBone.h"
#include "AniMaterial.h"
#include "AniVertex.h"
FbxAMatrix GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	return FbxAMatrix(lT, lR, lS);
}
FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
	FbxAMatrix lPoseMatrix;
	FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);
	memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));
	return lPoseMatrix;
}
void FbxAMatrix2GLMMat4(const FbxAMatrix& matrix,glm::mat4& mat4)
{
	//this may be getcol but now it is no sure!!!!
	for(int i=0;i<4;++i)
	{
		FbxVector4 col = matrix.GetRow(i);
		for(int k=0;k<4;++k)
		{
			mat4[i][k] = col.mData[k];
		}
	}
}
void FbxDouble32GLMVec3(glm::vec3& val0,const FbxDouble3& val1)
{
	for(int i=0;i<3;++i)
	{
		val0[i] = val1.mData[i];
	}
}
DataReader::DataReader()
{
	memset(m_target,0,sizeof(m_target));
	m_manager = NULL;
	m_scene = NULL;

	m_bone = NULL;
	m_vertexInfo = NULL;
}
DataReader::~DataReader()
{
}
void DataReader::SetTarget(const char* name)
{
	strcpy(m_target,name);
}
void DataReader::InitFbxData()
{
	const char* lFilename = m_target;
	// Initialize the SDK manager. This object handles all our memory management.
	m_manager = FbxManager::Create();
	FbxManager* lSdkManager = m_manager;
	// Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager,"");
	// Use the first argument as the filename for the importer.
	if(!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) { 
		printf("Call to FbxImporter::Initialize() failed.\n"); 
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString()); 
		exit(-1); 
	}   

	// Create a new scene so that it can be populated by the imported file.
	m_scene = FbxScene::Create(lSdkManager,"myScene");

	FbxScene* lScene = m_scene;
	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

    // The file is imported; so get rid of the importer.
    lImporter->Destroy();

	


	//the following set the scale system
    // Convert Axis System to what is used in this example, if needed
    FbxAxisSystem SceneAxisSystem = lScene->GetGlobalSettings().GetAxisSystem();
    FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
    if( SceneAxisSystem != OurAxisSystem )
    {
        OurAxisSystem.ConvertScene(lScene);
    }

    // Convert Unit System to what is used in this example, if needed
    FbxSystemUnit SceneSystemUnit = lScene->GetGlobalSettings().GetSystemUnit();
    if( SceneSystemUnit.GetScaleFactor() != 1.0 )
    {
        //The unit in this example is centimeter.
        FbxSystemUnit::cm.ConvertScene( lScene);
    }

    // Convert mesh, NURBS and patch into triangle mesh
    FbxGeometryConverter lGeomConverter(lSdkManager);
    lGeomConverter.Triangulate(lScene, /*replace*/true);

    // Split meshes per material, so that we only have one material per mesh (for VBO support)
    lGeomConverter.SplitMeshesPerMaterial(lScene, /*replace*/true);
	
}

void DataReader::StartParse()
{
	ParseBone();
	//we here first find ctrlpt bones
	ConnectBone2Vertex();
	ParseVertex();
	ParseMaterial();
}
void DataReader::ParseBone()
{
	ReadBone();
	ReadBoneAni();
}
void DataReader::ReadBone()
{
	FbxNode* pRootBone =GetRootBone();
	m_bone = new AniBone();
	m_bone->BuildTree(pRootBone,true);	
}
//reade bone animation is about the bone bind position and the other move
void DataReader::ReadBoneAni()
{
	FbxNode* lRootNode = m_scene->GetRootNode();
	FbxMesh* pMesh = lRootNode->GetMesh();
	const int lPoseCount = m_scene->GetPoseCount();
	FbxPose* pPose = m_scene->GetPose(0);
	if(pPose->IsBindPose())
	{
		printf("find bind yes\n");
	}

	FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
	int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	FbxArray<FbxString*> mAnimStackNameArray;
	m_scene->FillAnimStackNameArray(mAnimStackNameArray);
	FbxTakeInfo* lCurrentTakeInfo = m_scene->GetTakeInfo(*(mAnimStackNameArray[0]));  
	FbxTime mFrameTime, mStart, mStop, mCurrentTime;

	if(!lCurrentTakeInfo)
	{
		printf("no time info found");
		return;
	}
	mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
	mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
	mFrameTime.SetTime(0, 0, 0, 1, 0, m_scene->GetGlobalSettings().GetTimeMode());
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			FbxNode* pBone = lCluster->GetLink();
			if (!pBone)
				continue;
			AniBone* pAniBone = m_bone->FindBone(pBone);
			if(pAniBone == NULL)
			{
				printf("can not find the anibone!!!");
				return;
			}
			FbxAMatrix lVertexTransformMatrix;

			FbxAMatrix lReferenceGlobalInitPosition;
			FbxAMatrix lReferenceGlobalCurrentPosition;
			FbxAMatrix lClusterGlobalInitPosition;
			FbxAMatrix lClusterGlobalCurrentPosition;
			FbxAMatrix lReferenceGeometry;
			FbxAMatrix lAssociateGeometry;
			FbxAMatrix lClusterGeometry;
			
			FbxAMatrix offsetMatrix;
			FbxAMatrix localMatrix;
			FbxAMatrix globalMatrix;
			FbxAMatrix outputMatrix;
			glm::mat4 offsetMat;
			glm::mat4 localMat;
			glm::mat4 globalMat;
			glm::mat4 outputMat;


			lCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
//				lReferenceGlobalCurrentPosition = pGlobalPosition;
			
			lReferenceGeometry = GetGeometry(pMesh->GetNode());
			lReferenceGlobalInitPosition *= lReferenceGeometry;

			lCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
			offsetMatrix = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
			//we first push the bind matrix!!!
			int lNodeIndex = pPose->Find(pBone);
			if (lNodeIndex > -1)
			{
				globalMatrix = GetPoseMatrix(pPose, lNodeIndex);	
				FbxNode* pParent = pBone->GetParent();
				if(!pParent)
				{
					localMatrix.SetIdentity();
				}
				else
				{
					int lParentNodeIndex = pPose->Find(pParent);
					if(lParentNodeIndex > -1)
					{
						localMatrix = GetPoseMatrix(pPose,lParentNodeIndex).Inverse()*globalMatrix;
					}
					else
					{
						localMatrix.SetIdentity();
					}
				}
				outputMatrix = globalMatrix * offsetMatrix;
			}
			else
			{
				printf("error happen can not find bind pose \n");
				return;
			}
			FbxAMatrix2GLMMat4(offsetMatrix,offsetMat);
			pAniBone->SetOffsetMatrix(offsetMatrix);
			pAniBone->SetVertexOffsetMatrix(offsetMat);

			FbxAMatrix2GLMMat4(localMatrix,localMat);
			FbxAMatrix2GLMMat4(globalMatrix,globalMat);
			FbxAMatrix2GLMMat4(outputMatrix,outputMat);

			AniMatrix* pAniMatrix = new AniMatrix;
			pAniMatrix->m_localMatrix = localMat;
			pAniMatrix->m_globalMatrix = globalMat;
			pAniMatrix->m_outputMatrix = outputMat;
			pAniBone->SetBindAniMatrix(pAniMatrix);			
			//push bind matrix end
			//we push the animation matrix
			mCurrentTime = mStart;
			for(;mCurrentTime < mStop;)
			{

				globalMatrix = pBone->EvaluateGlobalTransform(mCurrentTime);
				FbxNode* pParent = pBone->GetParent();
				if(!pParent)
				{
					localMatrix.SetIdentity();
				}
				else
				{
					localMatrix = pParent->EvaluateGlobalTransform(mCurrentTime).Inverse()*globalMatrix;
				}
				outputMatrix = globalMatrix * offsetMatrix;

				FbxAMatrix2GLMMat4(localMatrix,localMat);
				FbxAMatrix2GLMMat4(globalMatrix,globalMat);
				FbxAMatrix2GLMMat4(outputMatrix,outputMat);
				AniMatrix* pAniMatrix = new AniMatrix;
				pAniMatrix->m_localMatrix = localMat;
				pAniMatrix->m_globalMatrix = globalMat;
				pAniMatrix->m_outputMatrix = outputMat;
				pAniBone->AddAniMatrix(pAniMatrix);			

				mCurrentTime += mFrameTime;
			}
				//push animation matrix end
		}
	}
}
FbxNode* DataReader::GetRootBone()
{
	FbxNode* lRootNode = m_scene->GetRootNode();
	if(!lRootNode)
	{
	  printf("can not find root\n");
	  return NULL;
	}
	for(int i = 0; i < lRootNode->GetChildCount();++i)
	{
		FbxNode* child = lRootNode->GetChild(i);
		bool isBone = false;
		for(int j = 0;j < child->GetNodeAttributeCount(); ++j)
		{
			if(child->GetNodeAttributeByIndex(i)->GetAttributeType() == FbxNodeAttribute::eSkeleton)
			{
				isBone =true;
				break;
			}
		}
		if(isBone)
		{
			return child;
		}
	}
	return NULL;
}
void DataReader::ConnectBone2Vertex()
{
	FbxNode* lRootNode = m_scene->GetRootNode();
	FbxMesh* pMesh = lRootNode->GetMesh();
	int lVertexCount = pMesh->GetControlPointsCount();
	m_index2bone.resize(lVertexCount);
	int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			FbxNode* pBone = lCluster->GetLink();
			if (!pBone)
				continue;
			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k) 
			{
				int lIndex = lCluster->GetControlPointIndices()[k];
				if (lIndex >= lVertexCount)
					continue;
				double lWeight = lCluster->GetControlPointWeights()[k];
				if (lWeight == 0.0)
				{
					continue;
				}
				m_index2bone[lIndex].index = lIndex;
				//this is not right!!!!
				AniBone* pAniBone = m_bone->FindBone(pBone);
				if(pAniBone == NULL)
				{
					printf("can not find the anibone!!!");
					return;
				}
				m_index2bone[lIndex].bones.push_back(pAniBone);
				m_index2bone[lIndex].boneFactors.push_back(lWeight);
			}
		}
	}
}
void DataReader::ParseVertex()
{
	FbxNode* lRootNode = m_scene->GetRootNode();
	if(!lRootNode)
	{
	  printf("can not find root\n");
	  return;
	}
	FbxMesh* pMesh = lRootNode->GetMesh();
	if(!pMesh)
	{
		printf("no mesh find!!");
		return;
	}	
	
	int triangleCount = pMesh->GetPolygonCount();
	int vertexCounter = 0;
	m_vertexInfo = new AniVertexInfo();
	AniVertex* pVertex=new AniVertex[triangleCount*3];
	m_vertexInfo->vertex = pVertex;
	int uvSize = triangleCount*6;
	float* pUV =new float[uvSize];
	m_vertexInfo->vertexUV = pUV;
	m_vertexInfo->vertexUVSize = uvSize;
	FbxVector4* pCtrlPoint = pMesh->GetControlPoints();
	for(int i = 0 ; i < triangleCount ; ++i)
	{
		for(int j = 0 ; j < 3 ; ++j)
		{
			//read the vertex data
			int ctrlPointIndex = pMesh->GetPolygonVertex(i , j);
			float x=pCtrlPoint[ctrlPointIndex].mData[0];
			float y=pCtrlPoint[ctrlPointIndex].mData[1];
			float z=pCtrlPoint[ctrlPointIndex].mData[2];
			(pVertex+vertexCounter)->SetPoint(x,y,z);
			for(int k = 0; k < (int)m_index2bone[ctrlPointIndex].bones.size();++k)
			{
				(pVertex+vertexCounter)->AddBone(m_index2bone[ctrlPointIndex].bones[k],
											   m_index2bone[ctrlPointIndex].boneFactors[k]);
			}

			//end vertex data
			//read the uv data
			if(pMesh->GetElementUVCount() < 1 )
			{
				printf("can not find the uv\n");
				return ;
			}
			FbxGeometryElementUV* pVertexUV = pMesh->GetElementUV(0);
			const bool lUseIndex = pVertexUV->GetReferenceMode() != FbxGeometryElement::eDirect;

			switch(pVertexUV->GetMappingMode()) 
			{ 
			case FbxGeometryElement::eByControlPoint:
				{
					if(!lUseIndex)
					{
						*(pUV+vertexCounter*2) = pVertexUV->GetDirectArray().GetAt(ctrlPointIndex).mData[0]; 
						*(pUV+vertexCounter*2+1) = pVertexUV->GetDirectArray().GetAt(ctrlPointIndex).mData[1]; 
					}
					else
					{
						int id = pVertexUV->GetIndexArray().GetAt(ctrlPointIndex);
						*(pUV+vertexCounter*2) = pVertexUV->GetDirectArray().GetAt(id).mData[0]; 
						*(pUV+vertexCounter*2+1) = pVertexUV->GetDirectArray().GetAt(id).mData[1];

					}

				}
				break;
			case FbxGeometryElement::eByPolygonVertex:
				{
					int index = pMesh->GetTextureUVIndex(i, j);
					*(pUV+vertexCounter*2) = pVertexUV->GetDirectArray().GetAt(index).mData[0];
					*(pUV+vertexCounter*2+1) = pVertexUV->GetDirectArray().GetAt(index).mData[1]; 
				}
				break;
				//we just handle two condition!!!
			default:
				break;
			}
			//end uv data
			vertexCounter++; 
		}
	}
	//we not finish the  vertex bone data yet!!!
}
void DataReader::ParseMaterial()
{
	
	FbxNode* lRootNode = m_scene->GetRootNode();
	FbxMesh* pMesh = lRootNode->GetMesh();
	FbxNode* pNode = pMesh->GetNode();  
	int materialCount = pNode->GetMaterialCount(); 
	printf("matetial count is %d\n",materialCount);
	FbxSurfaceMaterial *lMaterial = pNode->GetMaterial(0);
	if(lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
	{   
		printf("yes this is a SurfacePhong \n");
		FbxSurfacePhong* pPhong =(FbxSurfacePhong*)lMaterial;
		// need add the matetial to!!       
		AniMaterial* pMaterial =new AniMaterial;
		FbxDouble32GLMVec3(pMaterial->Ambient,pPhong->Ambient);
		FbxDouble32GLMVec3(pMaterial->AmbientFactor,pPhong->AmbientFactor);
		FbxDouble32GLMVec3(pMaterial->Diffuse,pPhong->Diffuse);
		FbxDouble32GLMVec3(pMaterial->DiffuseFactor,pPhong->DiffuseFactor);
		FbxDouble32GLMVec3(pMaterial->Emissive,pPhong->Emissive);
		FbxDouble32GLMVec3(pMaterial->EmissiveFactor,pPhong->EmissiveFactor);
		FbxDouble32GLMVec3(pMaterial->Reflection,pPhong->Reflection);
		FbxDouble32GLMVec3(pMaterial->ReflectionFactor,pPhong->ReflectionFactor);
		FbxDouble32GLMVec3(pMaterial->Shininess,pPhong->Shininess);
		FbxDouble32GLMVec3(pMaterial->Specular,pPhong->Specular);
		FbxDouble32GLMVec3(pMaterial->SpecularFactor,pPhong->SpecularFactor);
		m_vertexInfo->AddMaterial(pMaterial);
	}   

}
