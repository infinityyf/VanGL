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
	
		int BuildTree(int start,int mid,int depth);
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

	// the thought is more like a quick sort(every time move smaller to left)
	// conference from CHAI3D
	// return root node index in the treeNodeList
	inline int AABBTree::BuildTree(int start, int end, int depth) {
		//1.create a large box contains all nodes
		AABBTreeNode currentRoot;
		currentRoot.depth = depth;
		for (int i = start; i <= end; i++) {
			currentRoot.node.Enclosure(&nodes[i].node);
		}
		//2. get split axis
		int axis = currentRoot.node.GetLongestAxis();
		int i = start;
		int mid = end;
		float center = currentRoot.node.center[axis];

		//3. quick sort
		while (i < mid) {
			if (nodes[i].node.center[axis] < center) {
				// make sure all less node is on the left of mid node
				i++;
			}
			else {
				//swap(i, mid);
				int leftSubTree = nodes[i].leftTree;
				int rightSubTree = nodes[i].rightTree;
				AABBBox leftBox = nodes[i].node;

				nodes[i].leftTree = nodes[mid].leftTree;
				nodes[i].rightTree = nodes[mid].rightTree;
				nodes[i].node = nodes[mid].node;

				nodes[mid].leftTree = leftSubTree;
				nodes[mid].rightTree = nodes[i].rightTree;
				nodes[mid].node = leftBox;
				mid--;
			}
		}//now this layer is sort go to deeper layer
		int nextDeep = depth + 1;

		if ((mid == start) || (mid == end))
		{
			mid = (start + end) / 2;
		}

		//4.recursive end conditions
		//if contains two nodes just return
		if ((end - start) == 1)
		{
			// set left leaf(only assign depth when assign the left or right tree)
			currentRoot.leftTree = start;
			nodes[start].depth = nextDeep;

			// set right leaf
			currentRoot.rightTree = end;
			nodes[end].depth = nextDeep;
		}
		else//if contains more than 2 nodes, go to deeper
		{
			//[start,mid]'s length is not 0
			if (mid > start) {
				currentRoot.leftTree = BuildTree(start, mid, nextDeep);
			}
			else {
				currentRoot.leftTree = start;
				nodes[start].depth = nextDeep;
			}
			//[mid+1,end]'s length is not 0
			if ((mid + 1) < end)
			{
				currentRoot.rightTree = BuildTree((mid + 1), end, nextDeep);
			}
			else
			{
				currentRoot.rightTree = end;
				nodes[end].depth = nextDeep;
			}
		}

		// push back current root(the deeper root contains more smaller box,the deepest only contains 2 nodes)
		nodes.push_back(currentRoot);
		//return the index of Root node
		return nodes.size() - 1;
		/*
		from back search the tree
		|**leaf**|**leaf**|`````````|**root00**|**root01**|**root10**|**root11**|**root20**|``````````````````
		*/
	}
}



#endif