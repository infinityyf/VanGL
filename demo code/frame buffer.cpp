	unsigned int SVBO, SVAO;
	glGenVertexArrays(1, &SVAO);
	glGenBuffers(1, &SVBO);
	glBindVertexArray(SVAO);
	glBindBuffer(GL_ARRAY_BUFFER, SVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Screen), Screen, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	StandardShader screenPost((path+"src\\shaders\\postProcessShader.vs").c_str(), (path+"src\\shaders\\postProcessShader.fs").c_str());



	//frame buffer
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	// also can be binded to GL_READ_FRAMEBUFFER or GL_DRAW_FRAMEBUFFER
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// create attachment
	//1. texture attachment
	unsigned int texAttach;
	glGenTextures(1, &texAttach);
	glBindTexture(GL_TEXTURE_2D, texAttach);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach to frame(32) //frame target   //color attach num    //attachment type         //mipmap
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texAttach, 0);
	//2. render buffer
	unsigned int renderBufferObject;
	glGenRenderbuffers(1, &renderBufferObject);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);

	// framebuffer set up success
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	// now rendering result cannot be blited to screen (need to activate default buffer)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



/******************rendering code****************************/
// render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_TEXTURE_2D);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		shader.setMatrix4("camera.view", camera.view);
		shader.setMatrix4("camera.projection", camera.projection);
		shader.setFloat("camera.near", camera.near);
		shader.setFloat("camera.far", camera.far);
		shader.setVector3("viewPos", camera.cameraPos);
		nanosuit.scale(glm::vec3(0.3f, 0.3f, 0.3f));
		nanosuit.drawModel(&shader);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		screenPost.use();
		//remember ti disable depth test
		glActiveTexture(GL_TEXTURE0);
		screenPost.setInt("screenTexture", 0);
		glBindTexture(GL_TEXTURE_2D, frame.texAttach);
		//render a screensized-quad to screen
		glBindVertexArray(SVAO);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);