#include "Model.h"
#include <iostream>
#include <sstream>
#include <fstream>

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Model::Model()
	:
{
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Model::~Model()
{
	
}

void Model::loadModel(string path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::Draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader);
}

