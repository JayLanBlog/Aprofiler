#pragma once
#include "gl_render.h"
#include <device/gpu/gl/refer/model.h>

namespace APROFILER {
	namespace GL {
	
		class TargetReferencRender : public Render {
		public:

			TargetReferencRender() {
				glEnable(GL_DEPTH_TEST);
				//shader  = new Shader("resource/shader/1.model_loading.vs", "resource/shader/1.model_loading.fs");
				shaderGeometryPass = new Shader("resource/shader/9.ssao_geometry.vs", "resource/shader/9.ssao_geometry.fs");
				shaderLightingPass = new Shader("resource/shader/9.ssao.vs", "resource/shader/9.ssao_lighting.fs");
				shaderSSAO = new Shader("resource/shader/9.ssao.vs", "resource/shader/9.ssao.fs");
				shaderSSAOBlur = new Shader("resource/shader/9.ssao.vs", "resource/shader/9.ssao_blur.fs");
			
			   backpack = new Refer::Model("resource/obj/backpack/backpack.obj");
			}

			void MakeTarget() {
				glGenFramebuffers(1, &gBuffer);
				glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
				glGenTextures(1, &gPosition);
				glBindTexture(GL_TEXTURE_2D, gPosition);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
				// normal color buffer
				glGenTextures(1, &gNormal);
				glBindTexture(GL_TEXTURE_2D, gNormal);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
				// color + specular color buffer
				glGenTextures(1, &gAlbedo);
				glBindTexture(GL_TEXTURE_2D, gAlbedo);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
				// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
				unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
				glDrawBuffers(3, attachments);

				glGenRenderbuffers(1, &rboDepth);
				glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
				// finally check if framebuffer is complete
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << "Framebuffer not complete!" << std::endl;
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
				glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

				// SSAO color buffer
				glGenTextures(1, &ssaoColorBuffer);
				glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << "SSAO Framebuffer not complete!" << std::endl;
				// and blur stage
				glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
				glGenTextures(1, &ssaoColorBufferBlur);
				glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

			
			}



			void LoadModel(ModelRender* md) {
				//backpack = md;
				//backpack->AddShader(shaderGeometryPass);
			}



			float ourLerp(float a, float b, float f)
			{
				return a + f * (b - a);
			}


			void CreateNoiseTexture() {
				// generate sample kernel
				 // ----------------------
				std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
				std::default_random_engine generator;

				for (unsigned int i = 0; i < 64; ++i)
				{
					glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
					sample = glm::normalize(sample);
					sample *= randomFloats(generator);
					float scale = float(i) / 64.0f;

					// scale samples s.t. they're more aligned to center of kernel
					scale = ourLerp(0.1f, 1.0f, scale * scale);
					sample *= scale;
					ssaoKernel.push_back(sample);
				}

				// generate noise texture
				// ----------------------
				std::vector<glm::vec3> ssaoNoise;
				for (unsigned int i = 0; i < 16; i++)
				{
					glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
					ssaoNoise.push_back(noise);
				}
				glGenTextures(1, &noiseTexture);
				glBindTexture(GL_TEXTURE_2D, noiseTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			
			}


			void PrepareShaderParam() {
				// shader configuration
				// --------------------
				shaderLightingPass->use();
				shaderLightingPass->setInt("gPosition", 0);
				shaderLightingPass->setInt("gNormal", 1);
				shaderLightingPass->setInt("gAlbedo", 2);
				shaderLightingPass->setInt("ssao", 3);
				shaderSSAO->use();
				shaderSSAO->setInt("gPosition", 0);
				shaderSSAO->setInt("gNormal", 1);
				shaderSSAO->setInt("texNoise", 2);
				shaderSSAOBlur->use();
				shaderSSAOBlur->setInt("ssaoInput", 0);


			}
			void Draw(Camera camera) {
				// 1. geometry pass: render scene's geometry/color data into gbuffer
		   // -----------------------------------------------------------------
				glViewport(0,0, SCR_WIDTH, SCR_HEIGHT);
				glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
				glm::mat4 view = camera.GetViewMatrix();
				glm::mat4 model = glm::mat4(1.0f);
				shaderGeometryPass->use();
				shaderGeometryPass->setMat4("projection", projection);
				shaderGeometryPass->setMat4("view", view);
				// room cube
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
				model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
				shaderGeometryPass->setMat4("model", model);
				shaderGeometryPass->setInt("invertedNormals", 1); // invert normals as we're inside the cube
				renderCube();
				shaderGeometryPass->setInt("invertedNormals", 0);
				// backpack model on the floor
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
				model = glm::scale(model, glm::vec3(1.0f));
				shaderGeometryPass->setMat4("model", model);
				backpack->Draw(*(shaderGeometryPass));
				glBindFramebuffer(GL_FRAMEBUFFER, 0);


				// 2. generate SSAO texture
				// ------------------------
				glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
				glClear(GL_COLOR_BUFFER_BIT);
				shaderSSAO->use();
				// Send kernel + rotation 
				for (unsigned int i = 0; i < 64; ++i)
					shaderSSAO->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
				shaderSSAO->setMat4("projection", projection);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, gPosition);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, gNormal);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, noiseTexture);
				renderQuad();
				glBindFramebuffer(GL_FRAMEBUFFER, 0);


				// 3. blur SSAO texture to remove noise
				// ------------------------------------
				glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
				glClear(GL_COLOR_BUFFER_BIT);
				shaderSSAOBlur->use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
				renderQuad();
				glBindFramebuffer(GL_FRAMEBUFFER, 0);


				// 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
				// -----------------------------------------------------------------------------------------------------
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				shaderLightingPass->use();
				// send light relevant uniforms
				glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPos, 1.0));
				shaderLightingPass->setVec3("light.Position", lightPosView);
				shaderLightingPass->setVec3("light.Color", lightColor);
				// Update attenuation parameters
				const float linear = 0.09f;
				const float quadratic = 0.032f;
				shaderLightingPass->setFloat("light.Linear", linear);
				shaderLightingPass->setFloat("light.Quadratic", quadratic);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, gPosition);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, gNormal);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, gAlbedo);
				glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
				glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
				renderQuad();

			}




			// renderQuad() renders a 1x1 XY quad in NDC
			// -----------------------------------------
			unsigned int quadVAO = 0;
			unsigned int quadVBO = 0;
			void renderQuad()
			{
				if (quadVAO == 0)
				{
					float quadVertices[] = {
						// positions        // texture Coords
						-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
						-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
						 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
						 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
					};
					// setup plane VAO
					glGenVertexArrays(1, &quadVAO);
					glGenBuffers(1, &quadVBO);
					glBindVertexArray(quadVAO);
					glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
					glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
					glEnableVertexAttribArray(1);
					glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
				}
				glBindVertexArray(quadVAO);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);
			}



			// renderCube() renders a 1x1 3D cube in NDC.
			// -------------------------------------------------
			unsigned int cubeVAO = 0;
			unsigned int cubeVBO = 0;
			void renderCube()
			{
				// initialize (if necessary)
				if (cubeVAO == 0)
				{
					float vertices[] = {
						// back face
						-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
						 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
						 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
						 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
						-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
						-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
						// front face
						-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
						 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
						 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
						 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
						-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
						-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
						// left face
						-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
						-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
						-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
						-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
						-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
						-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
						// right face
						 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
						 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
						 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
						 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
						 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
						 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
						// bottom face
						-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
						 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
						 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
						 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
						-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
						-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
						// top face
						-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
						 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
						 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
						 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
						-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
						-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
					};
					glGenVertexArrays(1, &cubeVAO);
					glGenBuffers(1, &cubeVBO);
					// fill buffer
					glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
					glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
					// link vertex attributes
					glBindVertexArray(cubeVAO);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
					glEnableVertexAttribArray(1);
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
					glEnableVertexAttribArray(2);
					glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindVertexArray(0);
				}
				// render Cube
				glBindVertexArray(cubeVAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
			}
		public:

			unsigned int gBuffer;
			unsigned int gPosition, gNormal, gAlbedo;
			unsigned int rboDepth;
			unsigned int ssaoFBO, ssaoBlurFBO;
			unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
			unsigned int noiseTexture;

	
		public:
			// lighting info
 // -------------
			glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
			glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);
			std::vector<glm::vec3> ssaoKernel;
			
			Refer::Model* backpack = nullptr;
			const unsigned int SCR_WIDTH = 2000;
			const unsigned int SCR_HEIGHT =1280;
			RenderTargetSingle* ssaoTarget = nullptr;
			RenderTargetSingle* ssaoBlurFBOTarget = nullptr;
			RenderTargetThree* gBufferTarget = nullptr;
			Shader* shaderGeometryPass = nullptr;
			Shader* shaderLightingPass = nullptr;
			Shader* shaderSSAO = nullptr;
			Shader* shaderSSAOBlur = nullptr;

		};
	}

}