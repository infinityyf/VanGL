#pragma once
#ifndef LINE
#define LINE
#include "glad/glad.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>
#include "../shader.h"
class Line
{
public:
	glm::vec3 start;
	glm::vec3 end;
	glm::mat4 model;

	unsigned int LVAO;
	unsigned int LVBO;
	std::vector<glm::vec3> lines;
public:
	Line(glm::vec3&, glm::vec3&);

	// draw a line
	void debugDraw(StandardShader* shader) {
		shader->use();
		shader->setMatrix4("model", model);
		glBindVertexArray(LVAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

};

Line::Line(glm::vec3& start, glm::vec3& end) {
	this->start = start;
	this->end = end;
	lines.push_back(this->start);
	lines.push_back(this->end);
	model = glm::mat4(1.0f);

	glGenVertexArrays(1, &LVAO);
	glGenBuffers(1, &LVBO);
	glBindVertexArray(LVAO);
	glBindBuffer(GL_ARRAY_BUFFER, LVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * lines.size(), &lines[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glBindVertexArray(0);
}

#endif // ! LINE
