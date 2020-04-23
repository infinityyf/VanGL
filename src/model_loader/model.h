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

#include "collision_detect/AABBBox.h"
#include "collision_detect/AABBTree.h"
using namespace VANCollision;

class Model {
public:
	std::vector<Mesh> meshes;

	//need initialize
	glm::mat4 model;
	std::string modelDictionary;

	//aabb tree
	AABBTree tree;
	int root;
	
public:
	Model(std::string modelPath);
	void drawModel(StandardShader* shader, unsigned int sky, int shadowID = NULL);
	void drawModel(SeperatePipeline* shader);
	void drawModelInstaced(StandardShader* shader, unsigned int sky, int amount,glm::mat4 *matrix);
	
	void loadModel(std::string modelPath);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	//load texture
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, unsigned int typeName);

	//model operate
	void scale(glm::vec3 scale);
	void translate(glm::vec3 translate);
	void rotate(glm::vec3 axis, float radian);

	//collision related(must calculate after model operate)
	void generateAABBTree();
	void debugDraw(StandardShader* shader,int depth);
	void debugDrawBox(StandardShader* shader, int node);

	//intersect with a triangle
	bool IntersectWithTriangle(glm::vec3& origin, glm::vec3& target, int node, glm::vec3& intersectPoint);
};

//load model in normal way
inline Model::Model(std::string modelPath) {
	// load data from file
	loadModel(modelPath);
	model = glm::mat4(1.0f);
}

inline void Model::drawModel(StandardShader* shader, unsigned int sky,int shadowID)
{
	shader->use();
	shader->setMatrix4("model",model);
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i].drawMesh(shader, sky, shadowID);
	}
}

inline void Model::drawModel(SeperatePipeline* shader)
{
	setMatrix4("model", model, shader->vertexProgram);
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i].drawMesh(shader);
	}
}

inline void Model::drawModelInstaced(StandardShader* shader, unsigned int sky, int amount, glm::mat4* matrix)
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
	const aiScene* scene = modelImpoter.ReadFile(modelPath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs |aiProcess_CalcTangentSpace| aiProcess_GenSmoothNormals);
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

inline void Model::generateAABBTree()
{
	//building AABB tree
	this->tree = AABBTree();
	glm::vec3 origin = glm::vec3(0.0f);
	glm::vec3 dir = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 p1, p2;
	for (int i = 0; i < meshes.size(); i++) {
		tree.GenerateNodes(&meshes[i],i);
	}
	this->root = tree.BuildTree(0, tree.nodes.size() - 1, 0);
	//for (int i = 0; i < tree.nodes.size(); i++) {
	//	if (tree.nodes[i].node.CollideWithRay(origin, dir, p1, p2)) {
	//		std::cout << "hit:" << p1.x << "," << p1.y << "," << p1.z << std::endl;
	//	}
	//	else {
	//		std::cout << "miss" << std::endl;
	//	}
	//}
}

inline void Model::debugDraw(StandardShader* shader, int depth)
{
	shader->use();
	shader->setMatrix4("model", model);
	for (int i = root; i >= 0; i--) {
		if (tree.nodes[i].depth == depth) tree.nodes[i].node.DrawBox();
	}
}

inline void Model::debugDrawBox(StandardShader* shader, int node)
{
	shader->use();
	shader->setMatrix4("model", model);
	if (node == -1) return;
	tree.nodes[node].node.DrawBox();
}

//interesct with triangle
//here origin and target from model coord
//Moller-Trumbore algorithm
inline bool Model::IntersectWithTriangle(glm::vec3& origin, glm::vec3& target, int node, glm::vec3& intersectPoint)
{
	if (node == -1) return false;
	int meshID = tree.nodes[node].meshID;
	int faceID = tree.nodes[node].faceID;
	int indice0 = meshes[meshID].indices[faceID];
	int indice1 = meshes[meshID].indices[faceID + 1];
	int indice2 = meshes[meshID].indices[faceID + 2];
	float t = glm::length(target - origin);
	float epsilon = 1E-5f;
	glm::vec3 dir = glm::normalize(target - origin);
	glm::vec3 V0 = meshes[meshID].vertexs[indice0].Position;
	glm::vec3 V1 = meshes[meshID].vertexs[indice1].Position;
	glm::vec3 V2 = meshes[meshID].vertexs[indice2].Position;
	glm::vec3 E1 = glm::normalize(V1 - V0);
	glm::vec3 E2 = glm::normalize(V2 - V0);
	glm::vec3 Y = origin - V0;
	glm::mat3 K;
	K[0] = E1;
	K[1] = E2;
	K[2] = -dir;
	K = glm::inverse(K);
	intersectPoint = K * Y;
	if ((intersectPoint.x >= 0 && intersectPoint.x <= 1) && (intersectPoint.y >= 0 && intersectPoint.y <= 1) && abs(intersectPoint.z -t)<= epsilon) return true;
	return false;
}

#endif // !MODEL
