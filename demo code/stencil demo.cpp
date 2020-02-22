		glEnable(GL_DEPTH_TEST);
		glStencilMask(0xFF);
		//ALWAYS means always pass stencil test
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		nanosuit.scale(glm::vec3(0.2f, 0.2f, 0.2f));
		nanosuit.drawModel(&shader);
		// if not disenable depth test new object will cover last one
		glDisable(GL_DEPTH_TEST);
		glStencilMask(0x00);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);


		nanosuit.scale(glm::vec3(0.3f, 0.3f, 0.3f));
		nanosuit.drawModel(&shader);

		//if not set enable we cannot clear
		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);