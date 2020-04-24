#pragma once
#ifndef STANDARD_MODEL
#define STANDARD_MODEL
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../texture.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "StandardMesh.h"

class StandardModel {
public:
	std::vector<StandardMesh> meshes;
	glm::mat4 model;
	std::string modelDictionary;

public:
	StandardModel(std::string modelPath);
	void drawModel(ForwardStandardMaterial* material,unsigned int skybox,unsigned int shadowID=0);

	void loadModel(std::string modelPath);
	void processNode(aiNode* node, const aiScene* scene);
	StandardMesh processMesh(aiMesh* mesh, const aiScene* scene);

	//load texture
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, unsigned int typeName);

	//model operate
	void scale(glm::vec3 scale);
	void translate(glm::vec3 translate);
	void rotate(glm::vec3 axis, float radian);
};

//load model in normal way
inline StandardModel::StandardModel(std::string modelPath) {
	// load data from file
	loadModel(modelPath);
	model = glm::mat4(1.0f);
}


inline void StandardModel::drawModel(ForwardStandardMaterial* material,unsigned int skybox, unsigned int shadowID)
{
	material->setMatrix4("model", model, material->vertexProgram);
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i].drawMesh(material,skybox,shadowID);
	}
}


void StandardModel::loadModel(std::string modelPath) {
	Assimp::Importer modelImpoter;
	// use post process : translate into triangles and flip y coord (opengl inverse the y in texcoord)
	// aiProcess_GenNormals
	// aiProcess_SplitLargeMeshes : splite a large mesh into small meshes
	// aiProcess_OptimizeMeshes : combine small meshes to a large mesh
	// aiProcess_CalcTangentSpace : calculate the tangent vector of each vertice
	const aiScene* scene = modelImpoter.ReadFile(modelPath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR: fail to load model:" << modelImpoter.GetErrorString() << std::endl;
		return;
	}
	modelDictionary = modelPath.substr(0, modelPath.find_last_of('\\'));
	processNode(scene->mRootNode, scene);
}

inline void StandardModel::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// 接下来对它的子节点重复这一过程
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

inline StandardMesh StandardModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<StandardVertex> vertexs;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	//translate vertice 
	for (int i = 0; i < mesh->mNumVertices; i++) {
		StandardVertex vertex;
		//generate vertex
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		if (mesh->mNormals != nullptr) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		if (mesh->mTangents != nullptr) {
			//tangent coord
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
		}
		//assimp only allow 8 tex coords
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 uv;
			uv.x = mesh->mTextureCoords[0][i].x;
			uv.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = uv;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertexs.push_back(vertex);
	}
	//process indice
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	//process texture
	if (mesh->mMaterialIndex >= 0)
	{
		//from mesh get material
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, DIFFUSE_TEX);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, SPECULAR_TEX);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		//cannot use normalType to load normal map, use height map
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, NORMAL_TEX);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, AMBIENT_TEX);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}
	return StandardMesh(vertexs, indices, textures);
}


std::vector<Texture> StandardModel::loadMaterialTextures(aiMaterial* material, aiTextureType type, unsigned int typeName) {
	std::vector<Texture> textures;
	for (int i = 0; i < material->GetTextureCount(type); i++) {
		aiString path;
		material->GetTexture(type, i, &path);
		Texture texture;
		//use stb read pic
		texture.textureID = texture.loadTextureFromFile(path.C_Str(), modelDictionary);
		texture.textureType = typeName;
		textures.push_back(texture);
	}
	return textures;
}

void StandardModel::scale(glm::vec3 scale) {
	model = glm::scale(model, scale);
}

void StandardModel::translate(glm::vec3 translate) {
	model = glm::translate(model, translate);
}

void StandardModel::rotate(glm::vec3 axis, float radian) {
	model = glm::rotate(model, radian, axis);
}


#endif // !STANDARD_MODEL
