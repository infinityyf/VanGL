	//create a MSAA frame
	Frame MSAAFrame(width, height, 4);
	Frame NormalFrame(width, height);
	Screen screen;


// render to MSAA framebuffer
		MSAAFrame.use();
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw();


        // use blit to resize the multisample texture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, MSAAFrame.FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NormalFrame.FBO);
		// last parameter means filter if pic need to be stretched
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);


        //use as a texture
        // render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		postShader.use();
		postShader.setInt("screenTexture", 0);
		screen.Draw(&postShader,NormalFrame.texAttach);