#pragma once
#ifndef MODEL
#define MODEL
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

class Model {
public:
	std::vector<Mesh> meshes;

	//need initialize
	glm::mat4 model;
	std::string modelDictionary;
	
public:
	Model(std::string modelPath);
	void drawModel(StandardShader* shader, Skybox* sky, int shadowID = NULL);
	void drawModelInstaced(StandardShader* shader, Skybox* sky, int amount,glm::mat4 *matrix);
	
	void loadModel(std::string modelPath);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	//load texture
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, unsigned int typeName);

	//model operate
	void scale(glm::vec3 scale);
	void translate(glm::vec3 translate);
	void rotate(glm::vec3 axis, float radian);
};

Model::Model(std::string modelPath) {
	// load data from file
	loadModel(modelPath);
	model = glm::mat4(1.0f);
}

inline void Model::drawModel(StandardShader* shader,Skybox* sky,int shadowID)
{
	shader->use();
	shader->setMatrix4("model",model);
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i].drawMesh(shader, sky, shadowID);
	}
}

inline void Model::drawModelInstaced(StandardShader* shader, Skybox* sky, int amount, glm::mat4* matrix)
{
	unsigned int instanceVBO;
	// attach a new buffer then will be attah to each mesh's VAO
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), matrix, GL_STATIC_DRAW);
	for (int i = 0; i < meshes.size(); i++) {
		unsigned int VAO = meshes[i].VAO;
		// means later operation's target is instanceVBO,each VAO get same VBO data
		glBindVertexArray(VAO);
		// vertex atrribute for instance draw
		GLsizei vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

		// it means unpate these 4 vertex atrribution every 1 instance
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//shader->setMatrix4("model",model);
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i].drawMeshInstanced(shader, sky,amount);
	}
}

void Model::loadModel(std::string modelPath) {
	Assimp::Importer modelImpoter;
	// use post process : translate into triangles and flip y coord (opengl inverse the y in texcoord)
	// aiProcess_GenNormals
	// aiProcess_SplitLargeMeshes : splite a large mesh into small meshes
	// aiProcess_OptimizeMeshes : combine small meshes to a large mesh
	// aiProcess_CalcTangentSpace : calculate the tangent vector of each vertice
	const aiScene* scene = modelImpoter.ReadFile(modelPath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs |aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR: fail to load model:" << modelImpoter.GetErrorString() << std::endl;
		return;
	}
	modelDictionary = modelPath.substr(0, modelPath.find_last_of('\\'));
	processNode(scene->mRootNode, scene);
}

inline void Model::processNode(aiNode* node, const aiScene* scene)
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

inline Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
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
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

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

		std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, SPECULAR_TEX);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, NORMAL_TEX);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, AMBIENT_TEX);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}
	return Mesh(vertexs, indices, textures);
}


std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, unsigned int typeName) {
	std::vector<Texture> textures;
	for (int i = 0; i < material->GetTextureCount(type); i++) {
		aiString path;
		material->GetTexture(type, i, &path);
		Texture texture;
		//use stb read pic
		texture.textureID = texture.loadTextureFromFile(path.C_Str(),modelDictionary);
		texture.textureType = typeName;
		textures.push_back(texture);
	}
	return textures;
}

void Model::scale(glm::vec3 scale) {
	model = glm::scale(model, scale);
}

void Model::translate(glm::vec3 translate) {
	model = glm::translate(model, translate);
}

void Model::rotate(glm::vec3 axis,float radian) {
	model = glm::rotate(model, radian,axis);
}

#endif // !MODEL
