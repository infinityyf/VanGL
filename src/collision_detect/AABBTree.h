#pragma once
#ifndef AABBTREE
#define AABBTREE
#include "AABBBox.h"
#include <vector>

namespace VANCollision {
	struct AABBTreeNode {
		int leftTree;		//left child index
		int rightTree;		//right child index
		int depth;
		AABBBox node;
	};
	typedef std::vector<AABBTreeNode> treeNodeList;

	class AABBTree {
	public:
		treeNodeList nodes;
		void GenerateNodes(Mesh* mesh);
	};

	inline void AABBTree::GenerateNodes(Mesh* mesh) {
		for (int i = 0; i < mesh->indices.size(); i += 3) {
			glm::vec3 pos1 = mesh->vertexs[mesh->indices[i]].Position;
			glm::vec3 pos2 = mesh->vertexs[mesh->indices[i + 1]].Position;
			glm::vec3 pos3 = mesh->vertexs[mesh->indices[i + 2]].Position;
			AABBTreeNode node{ 0,0,0,AABBBox(pos1,pos2,pos3) };
			nodes.push_back(node);
		}
	}
}



#endif