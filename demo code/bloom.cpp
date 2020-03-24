// pingpang buffers to get guassion blur
	Frame* gaussionFrame1 = new Frame(width, height, false);
	Frame* gaussionFrame2 = new Frame(width, height, false);
	Frame* pingpangFrames[2] = { gaussionFrame1 ,gaussionFrame2 };

//calculate the gaussion blur（bloom effect）
if (BLOOM_ENABLE) {
    gaussionShader.use();
    gaussionShader.setInt("screenTexture", 0);
    for (int i = 0; i < 10; i++) {
        gaussionShader.setInt("horizontal", i % 2);
        pingpangFrames[i % 2]->use();
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        screen->Draw(&gaussionShader, i == 0 ? frame->texAttachs[BLOOM_TEXTURE] : pingpangFrames[1 - (i % 2)]->texAttachs[BLOOM_TEXTURE]);
    }
    // send all results to frame
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gaussionFrame2->FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame->FBO);
    glReadBuffer(attachments[BLOOM_TEXTURE]);	//which attachment to read
    glDrawBuffer(attachments[BLOOM_TEXTURE]);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glDrawBuffers(8, attachments);// set frame draw buffers to normal
}