	shader.setVector3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -1.0f));
	shader.setVector3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVector3("dirLight.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
	shader.setVector3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVector3("pointLights[0].position", glm::vec3(1.0f, 1.0f, -1.0f));
	shader.setVector3("pointLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVector3("pointLights[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setVector3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.22f);
	shader.setFloat("pointLights[0].quadratic", 0.2f);
	shader.setVector3("spotLights[0].position", glm::vec3(0.0f, 1.0f, 0.0f));
	shader.setVector3("spotLights[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
	shader.setVector3("spotLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVector3("spotLights[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setVector3("spotLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("spotLights[0].innerCutoff", 0.9f);
	shader.setFloat("spotLights[0].outerCutOff", 0.5f);