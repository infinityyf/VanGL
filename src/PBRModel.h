#pragma once
#ifndef PBR_MODEL
#define PBR_MODEL
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "mesh.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

class PBRModel {
public:
	std::vector<Mesh> meshes;

	//need initialize
	glm::mat4 model;
	std::string modelDictionary;
	std::string modelName;

public:
	PBRModel(std::string modelPath);
	void drawPBRModel(StandardShader* shader, unsigned int irradiance, unsigned int prefilter, unsigned int brdfLUT, int shadowID = NULL);

	void loadModel(std::string modelPath);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	//model operate
	void scale(glm::vec3 scale);
	void translate(glm::vec3 translate);
	void rotate(glm::vec3 axis, float radian);
};

//load model in PBR way
inline PBRModel::PBRModel(std::string modelPath) {
	loadModel(modelPath);
	model = glm::mat4(1.0f);
}

inline void PBRModel::drawPBRModel(StandardShader* shader, unsigned int irradiance, unsigned int prefilter,unsigned int brdfLUT, int shadowID)
{
	shader->use();
	shader->setMatrix4("model", model);
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i].drawMeshPBR(shader, irradiance,prefilter,brdfLUT, shadowID);
	}
}


void PBRModel::loadModel(std::string modelPath) {
	Assimp::Importer modelImpoter;
	const aiScene* scene = modelImpoter.ReadFile(modelPath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR: fail to load model:" << modelImpoter.GetErrorString() << std::endl;
		return;
	}
	modelDictionary = modelPath.substr(0, modelPath.find_last_of('\\'));
	modelName = modelPath.substr();
	processNode(scene->mRootNode, scene);
}

inline void PBRModel::processNode(aiNode* node, const aiScene* scene)
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

inline Mesh PBRModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertexs;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	//translate vertice 
	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
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
	//load pbr textures
	std::string texturePath = modelDictionary + "\\Textures";
	Texture albedo = Texture();
	albedo.textureID = albedo.loadTextureFromFile("Cerberus_A.tga", texturePath);
	albedo.textureType = AMBIENT_TEX;
	Texture roughness = Texture();
	roughness.textureID = roughness.loadTextureFromFile("Cerberus_R.tga", texturePath);
	roughness.textureType = ROUGHNESS_TEX;
	Texture normal = Texture();
	normal.textureID = normal.loadTextureFromFile("Cerberus_N.tga", texturePath);
	normal.textureType = NORMAL_TEX;
	Texture metallic = Texture();
	metallic.textureID = metallic.loadTextureFromFile("Cerberus_M.tga", texturePath);
	metallic.textureType = METALLIC_TEX;
	Texture ao = Texture();
	ao.textureID = ao.loadTextureFromFile("Cerberus_AO.tga", texturePath);
	ao.textureType = AO_TEX;
	textures.push_back(albedo);
	textures.push_back(roughness);
	textures.push_back(normal);
	textures.push_back(metallic);
	textures.push_back(ao);
	return Mesh(vertexs, indices, textures);
}



void PBRModel::scale(glm::vec3 scale) {
	model = glm::scale(model, scale);
}

void PBRModel::translate(glm::vec3 translate) {
	model = glm::translate(model, translate);
}

void PBRModel::rotate(glm::vec3 axis, float radian) {
	model = glm::rotate(model, radian, axis);
}

#endif // !MODEL
