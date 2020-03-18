#pragma once
#ifndef AABBBOX
#define AABBBOX
#include <algorithm>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "../mesh.h"

namespace VANCollision {
	class AABBBox {
	public:
		//drawing setting
		unsigned int BVAO;
		unsigned int BVBO;
		std::vector<glm::vec3> lines;
	public:
		glm::vec3 center;
		glm::vec3 minBound;	//aabb min coord
		glm::vec3 maxBound;	//aabb max coord

		AABBBox() {}
		AABBBox(Mesh* mesh);
		AABBBox(glm::vec3& a, glm::vec3& b, glm::vec3& c);
		//AABBBox(glm::vec3& start, glm::vec3& end);

		static glm::vec3 Min(glm::vec3, glm::vec3);
		static glm::vec3 Max(glm::vec3, glm::vec3);

		//collision detect with another AABBbox
		bool CollideWithBox(AABBBox& box);
		//collision detect with a point
		bool CollideWithPoint(glm::vec3& point);
		//collision detect with a ray
		bool CollideWithRay(glm::vec3& origin, glm::vec3& dir, float& x1, float& x2);
		//collision detect with a line sigment
		bool CollideWithSegment(glm::vec3& origin, glm::vec3& end);

		//enclose
		void Enclosure(AABBBox* bbox);
		//get the longest axis
		int GetLongestAxis();

		//draw debug lines
		void DrawBox();
	};
	//get min position
	inline glm::vec3 AABBBox::Min(glm::vec3 a, glm::vec3 b) {
		return glm::vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
	}
	//get max position
	inline glm::vec3 AABBBox::Max(glm::vec3 a, glm::vec3 b) {
		return glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
	}

	//get mesh bound box
	AABBBox::AABBBox(Mesh* mesh) {
		//go througn all verteice
		minBound = glm::vec3(FLT_MAX);
		maxBound = glm::vec3(-FLT_MAX);
		for (int i = 0; i < mesh->vertexs.size(); i++) {
			glm::vec3 position = mesh->vertexs[i].Position;
			minBound = Min(position, minBound);
			maxBound = Max(position, maxBound);
		}
		center = (minBound + maxBound) / 2.0f;
	}

	//get triangle bound box
	AABBBox::AABBBox(glm::vec3& a, glm::vec3& b, glm::vec3& c) {
		minBound = glm::vec3(FLT_MAX);
		maxBound = glm::vec3(-FLT_MAX);
		minBound = Min(a, minBound);
		minBound = Min(b, minBound);
		minBound = Min(c, minBound);
		maxBound = Max(a, maxBound);
		maxBound = Max(b, maxBound);
		maxBound = Max(c, maxBound);
		center = (minBound + maxBound) / 2.0f;
	}

	// enlarge the box to contain bbox
	inline void AABBBox::Enclosure(AABBBox* bbox) {
		minBound = Min(minBound, bbox->minBound);
		maxBound = Max(maxBound, bbox->maxBound);
		center = (minBound + maxBound) / 2.0f;
	}

	inline int AABBBox::GetLongestAxis() {
		float x = maxBound.x - minBound.x;
		float y = maxBound.y - minBound.y;
		float z = maxBound.z - minBound.z;
		if (x > y) {
			if (x > z) return 0;
			return 2;
		}
		else {
			if (y > z)return 1;
			return 2;
		}
	}

	inline bool AABBBox::CollideWithBox(AABBBox& box) {
		if (box.minBound.x > maxBound.x) return (false);
		if (box.minBound.y > maxBound.y) return (false);
		if (box.minBound.z > maxBound.z) return (false);
		if (box.maxBound.x < minBound.x) return (false);
		if (box.maxBound.y < minBound.y) return (false);
		if (box.maxBound.z < minBound.z) return (false);
		return (true);
	}

	inline bool AABBBox::CollideWithPoint(glm::vec3& point)
	{
		if (point.x > maxBound.x) return (false);
		if (point.y > maxBound.y) return (false);
		if (point.z > maxBound.z) return (false);
		if (point.x < minBound.x) return (false);
		if (point.y < minBound.y) return (false);
		if (point.z < minBound.z) return (false);
		return (true);
	}



	//intersect with ray if intersect return true and two point
	// algorithm comes from https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
	inline bool AABBBox::CollideWithRay(glm::vec3& origin, glm::vec3& dir, float& p1, float& p2)
	{
		dir = glm::normalize(dir);
		//bound value of each coord
		float x0 = minBound.x;
		float y0 = minBound.y;
		float z0 = minBound.z;
		float x1 = maxBound.x;
		float y1 = maxBound.y;
		float z1 = maxBound.z;

		//get intersect point on coordinate plane
		// dir is normal vector so dir.x is the cosine of angle between dir and x-axis
		float t0x = (x0 - origin.x) / (dir.x);
		float t1x = (x1 - origin.x) / (dir.x);
		if (t0x > t1x) swap(t0x, t1x);
		float t0y = (y0 - origin.y) / (dir.y);
		float t1y = (y1 - origin.y) / (dir.y);
		if (t0y > t1y) swap(t0y, t1y);
		float t0z = (z0 - origin.z) / (dir.z);
		float t1z = (z1 - origin.z) / (dir.z);
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
	inline bool AABBBox::CollideWithSegment(glm::vec3& origin, glm::vec3& end)
	{

		//if line box contain current box
		glm::vec3 dir = end - origin;
		float length = glm::length(dir);
		float p1, p2;
		if(!this->CollideWithRay(origin, dir,p1,p2)) return false;
		if (p1 > length) return false;
		if (p2 < 0) return false;
		//if these two intersect
		//if (!this->CollideWithPoint(origin) && !this->CollideWithPoint(end)) return false;
		return true;
	}



	inline void AABBBox::DrawBox() {
		if (lines.size() == 0) {
			lines.push_back(glm::vec3(minBound.x, minBound.y, minBound.z));
			lines.push_back(glm::vec3(maxBound.x, minBound.y, minBound.z));
			lines.push_back(glm::vec3(minBound.x, maxBound.y, minBound.z));
			lines.push_back(glm::vec3(maxBound.x, maxBound.y, minBound.z));
			lines.push_back(glm::vec3(minBound.x, minBound.y, maxBound.z));
			lines.push_back(glm::vec3(maxBound.x, minBound.y, maxBound.z));
			lines.push_back(glm::vec3(minBound.x, maxBound.y, maxBound.z));
			lines.push_back(glm::vec3(maxBound.x, maxBound.y, maxBound.z));
												
			lines.push_back(glm::vec3(minBound.x, minBound.y, minBound.z));
			lines.push_back(glm::vec3(minBound.x, maxBound.y, minBound.z));
			lines.push_back(glm::vec3(maxBound.x, minBound.y, minBound.z));
			lines.push_back(glm::vec3(maxBound.x, maxBound.y, minBound.z));
			lines.push_back(glm::vec3(minBound.x, minBound.y, maxBound.z));
			lines.push_back(glm::vec3(minBound.x, maxBound.y, maxBound.z));
			lines.push_back(glm::vec3(maxBound.x, minBound.y, maxBound.z));
			lines.push_back(glm::vec3(maxBound.x, maxBound.y, maxBound.z));
											
			lines.push_back(glm::vec3(minBound.x, minBound.y, minBound.z));
			lines.push_back(glm::vec3(minBound.x, minBound.y, maxBound.z));
			lines.push_back(glm::vec3(maxBound.x, minBound.y, minBound.z));
			lines.push_back(glm::vec3(maxBound.x, minBound.y, maxBound.z));
			lines.push_back(glm::vec3(minBound.x, maxBound.y, minBound.z));
			lines.push_back(glm::vec3(minBound.x, maxBound.y, maxBound.z));
			lines.push_back(glm::vec3(maxBound.x, maxBound.y, minBound.z));
			lines.push_back(glm::vec3(maxBound.x, maxBound.y, maxBound.z));
			glGenVertexArrays(1, &BVAO);
			glGenBuffers(1, &BVBO);
			glBindVertexArray(BVAO);
			glBindBuffer(GL_ARRAY_BUFFER, BVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * lines.size(), &lines[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glBindVertexArray(0);
		}
		glBindVertexArray(BVAO);
		glDrawArrays(GL_LINES,0,48);
		glBindVertexArray(0);

	}
}



#endif