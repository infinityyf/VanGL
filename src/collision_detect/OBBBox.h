#pragma once
#ifndef OBBBOX
#define OBBBOX
#include <algorithm>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "../mesh.h"
using namespace std;
namespace VANCollision {
	class OBBBox {
	public:
		//drawing setting
		unsigned int BVAO;
		unsigned int BVBO;
		std::vector<glm::vec3> lines;
	public:
		glm::vec3 center;
		glm::mat4 modelMatrix;
		glm::vec3 minBound;	//obb min coord
		glm::vec3 maxBound;	//aabb max coord
		glm::vec3 orientX;	// obb x axis orient
		glm::vec3 orientY;	// obb y axis orient
		glm::vec3 orientZ;	// obb z axis orient
		float halfZ;		// obb axis length
		float halfY;
		float halfX;
		OBBBox() {}


		//collision detect with another OBB use SAT algorithm
		bool CollideWithBox(OBBBox& box);

		//collision detect with a point, just like AABB (calculate the distance)
		bool CollideWithPoint(glm::vec3& point);

		//collision detect with a ray
		bool CollideWithRay(glm::vec3& origin, glm::vec3& dir, float& x1, float& x2);

		//collision detect with a line sigment
		bool CollideWithSegment(glm::vec3& origin, glm::vec3& end);

		//get the longest axis
		int GetLongestAxis();

		//draw debug lines
		void DrawBox();
	};

	inline int OBBBox::GetLongestAxis() {
		return 0;
	}

	inline bool OBBBox::CollideWithBox(OBBBox& box) {

		return (true);
	}

	inline bool OBBBox::CollideWithPoint(glm::vec3& point)
	{
		glm::vec3 testVector = point - center;
		if (abs(glm::dot(testVector, orientX)) > halfX) return false;
		if (abs(glm::dot(testVector, orientY)) > halfY) return false;
		if (abs(glm::dot(testVector, orientZ)) > halfZ) return false;
		return (true);
	}



	//intersect with ray if intersect return true and two point
	// algorithm comes from 
	inline bool OBBBox::CollideWithRay(glm::vec3& origin, glm::vec3& dir, float& p1, float& p2)
	{
		dir = glm::normalize(dir);
		//point from origin to OBB center
		glm::vec3 PC = center - origin;

		float orientXLength = abs(glm::dot(PC, orientX));
		float orientYLength = abs(glm::dot(PC, orientY));
		float orientZLength = abs(glm::dot(PC, orientZ));
		float orientXDirLength = abs(glm::dot(dir, orientX));
		float orientYDirLength = abs(glm::dot(dir, orientY));
		float orientZDirLength = abs(glm::dot(dir, orientZ));

		//get intersect point on coordinate plane
		// dir is normal vector so dir.x is the cosine of angle between dir and x-axis
		float t0x = (orientXLength - halfX) / orientXDirLength;
		float t1x = (orientXLength + halfX) / orientXDirLength;
		if (t0x > t1x) swap(t0x, t1x);
		float t0y = (orientYLength - halfY) / orientYDirLength;
		float t1y = (orientYLength + halfY) / orientYDirLength;
		if (t0y > t1y) swap(t0y, t1y);
		float t0z = (orientZLength - halfZ) / orientZDirLength;
		float t1z = (orientZLength + halfZ) / orientZDirLength;
		if (t0z > t1z) swap(t0z, t1z);

		//get intersect points pair(min is the bigger one)
		float t0 = (t0x < t0y) ? t0y : t0x;
		float t1 = (t1x < t1y) ? t1x : t1y;
		t0 = (t0 > t0z) ? t0 : t0z;
		t1 = (t1 > t1z) ? t1z : t1;

		//get intersect point
		p1 = t0;
		p2 = t1;

		if (t1 > t0) return true;
		else return false;
	}

	//if line segment collide current box
	inline bool OBBBox::CollideWithSegment(glm::vec3& origin, glm::vec3& end)
	{
		//if line box contain current box
		glm::vec3 dir = end - origin;
		float length = sqrt(glm::dot(dir, dir));
		float p1, p2;
		if (!this->CollideWithRay(origin, dir, p1, p2)) return false;
		if (p1 > length) return false;
		if (p2 < 0) return false;
		return true;
	}



	
}



#endif