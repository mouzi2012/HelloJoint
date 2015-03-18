#include "DataReader.h"
#include "AniBone.h"
#include "AniMaterial.h"
#include "AniVertex.h"

DataReader::DataReader()
{
	memset(m_target,0,sizeof(m_target));
	m_manager = NULL;
	m_scene = NULL;

	m_bones = NULL;
	m_material = NULL;	
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
	ParseVertex();
	ParseMaterial();
}
void DataReader::ParseBone()
{
}
void DataReader::ParseVertex()
{
}
void DataReader::ParseMaterial()
{
}
