#pragma once
#include <device/gpu/gl/shader/gl_shader.h>
#include <device/gpu/gl/common/gl_primitive.h>
#include <device/gpu/gl/manager/resource_manager.h>
#include <device/gpu/gl/common/gl_render_target.h>
#include <device/gpu/gl/camera/camera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
namespace APROFILER {

	namespace GL {
		class Render {
		public:
			Render() {

			}
			Render(Shader* shader) :mShader(shader) {}
			virtual ~Render() {
				Destory();
			}
			virtual void AddShader(Shader* shader) {
				mShader = shader;
			}
			virtual void CreateGeometry() {}
			virtual void Draw() {}
			virtual void Destory() {
				if (mShader) {
					delete mShader;
				}
			}
			void Clean() {
				points.clear();
			}
			void PusPoint(float  point) {
				points.push_back(point);
			}
		protected:
			unsigned int VBO = 0, VAO = 0;
			Shader* mShader = nullptr;
			std::vector<float> points;
		};

		/**/
		

		class ModelRender : public Render {
		public:
			ModelRender() {}

			ModelRender(Shader* shader) :Render(shader) {

			}

			void AddPrimtive(Primitive* primitive, std::vector<int> textures) {
				primitives.push_back(primitive);
				textureMap[primitiveCount] = textures;
				primitive->primitiveId = primitiveCount;
				primitiveCount++;
			}

			virtual void CreateGeometry() {

			}

			virtual void Draw() {

				for (int i = 0; i < primitives.size(); i++) {
					// bind appropriate textures
					unsigned int diffuseNr = 1;
					unsigned int specularNr = 1;
					unsigned int normalNr = 1;
					unsigned int heightNr = 1;
					ResourceManager* resManager = ResourceManager::getResManager();
					std::vector<int> textures = textureMap[primitives[i]->primitiveId];
					for (unsigned int i = 0; i < textures.size(); i++) {
						glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
						// retrieve texture number (the N in diffuse_textureN)
						string number;
						Texture* texture = resManager->getTextureByID(textures[i]);
						string name = texture->name;
						if (name == "texture_diffuse")
							number = std::to_string(diffuseNr++);
						else if (name == "texture_specular")
							number = std::to_string(specularNr++); // transfer unsigned int to string
						else if (name == "texture_normal")
							number = std::to_string(normalNr++); // transfer unsigned int to string
						else if (name == "texture_height")
							number = std::to_string(heightNr++); // transfer unsigned int to string
					  //  cout << shader.ID << "-" << " texture id " << texture_ids[i] << ", glTexture path : "<<texture->path  << ",shader prop name : " << (name + number).c_str() << std::endl;
						// now set the sampler to the correct texture unit
						glUniform1i(glGetUniformLocation(mShader->ID, (name + number).c_str()), i);
						// and finally bind the texture
						texture->Use();
					}
					mShader->use();
					primitives[i]->Draw();
					glActiveTexture(GL_TEXTURE0);
				}

			}
			int id = 0;
		private:
			int primitiveCount = 0;
			std::map<int, std::vector<int>> textureMap;
			std::vector<Primitive*> primitives;

		};



		class PrimitiveRender : public Render {
		public:
			PrimitiveRender() {
				primitive = new Primitive();
			}
			PrimitiveRender(Shader* shader) :Render(shader) {
				primitive = new Primitive();
			}

			virtual void CreateGeometry() {
				primitive->CreateVBO();
				float vertices[] = {
					-0.5f, -0.5f, 0.0f, // left  
					0.5f, -0.5f, 0.0f, // right 
					0.0f,  0.5f, 0.0f  // top   
				};
				PrimitiveElement* element = new PrimitiveElement();
				element->element_count = 3;
				element->mType = ElementType::TYPE_VECTOR3;
				element->sematic = ElementSemantic::SEM_POSITION;
				element->mStrade = sizeof(float) * 3;
				primitive->AddElement(*element);
				primitive->UploadToVBO(vertices, sizeof(float) * 3, 3);
				primitive->Make();
			}

			virtual void Draw() {
				mShader->use();
				primitive->Draw();
			}

		private:
			Primitive* primitive = nullptr;
		};

		class PointRender : public Render {
		public:
			PointRender() {}
			PointRender(Shader* shader) :Render(shader) {}
			virtual ~PointRender() {
				//	Destory();
			}
			virtual void CreateGeometry() {
				float vertices[] = {
				 -0.5f, -0.5f, 0.0f, // left  
				  0.5f, -0.5f, 0.0f, // right 
				  0.0f,  0.5f, 0.0f  // top   
				};
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
				// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
				glBindVertexArray(0);
			}
			virtual void Draw() {
				mShader->use();
				glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
				glPointSize(5);
				glDrawArrays(GL_POINTS, 0, 3);
			}
		};


		class TargetRender : public Render {
		public:

			TargetRender() {
			    glEnable(GL_DEPTH_TEST);
				noiseTexture = new Texture();
				//shader  = new Shader("resource/shader/1.model_loading.vs", "resource/shader/1.model_loading.fs");
				shaderGeometryPass = new Shader("resource/shader/9.ssao_geometry.vs", "resource/shader/9.ssao_geometry.fs");
				shaderLightingPass = new Shader("resource/shader/9.ssao.vs", "resource/shader/9.ssao_lighting.fs");
				shaderSSAO = new Shader("resource/shader/9.ssao.vs", "resource/shader/9.ssao.fs");
				shaderSSAOBlur = new Shader("resource/shader/9.ssao.vs", "resource/shader/9.ssao_blur.fs");
			}

			void LoadModel(ModelRender* md) {
				backpack =md;
				backpack->AddShader(shaderGeometryPass);
			}

			void MakeTarget() {
				gBufferTarget = new RenderTargetThree(SCR_WIDTH, SCR_HEIGHT);
				gBufferTarget->Make();
				ssaoTarget = new RenderTargetSingle(SCR_WIDTH, SCR_HEIGHT);
				
				ssaoBlurFBOTarget = new RenderTargetSingle(SCR_WIDTH, SCR_HEIGHT);
				ssaoBlurFBOTarget->RestFrame();
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
				noiseTexture->Cretate();
				// 4, 4,GL_NEAREST,GL_NEAREST,GL_REPEAT,GL_REPEAT,GL_RGBA32F,GL_RGB,GL_FLOAT,&ssaoNoise[0]
				noiseTexture->Make(4, 4, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_RGBA32F, GL_RGB, GL_FLOAT, &ssaoNoise[0]);

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
				 glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
				gBufferTarget->UseForDraw();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f);
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
				model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
				model = glm::scale(model, glm::vec3(1.0f));
				shaderGeometryPass->setMat4("model", model);
				backpack->Draw();
				gBufferTarget->RestFrame();
				ssaoTarget->UseForDraw();
				glClear(GL_COLOR_BUFFER_BIT);
				shaderSSAO->use();
				// Send kernel + rotation 
				for (unsigned int i = 0; i < 64; ++i)
					shaderSSAO->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
				shaderSSAO->setMat4("projection", projection);
				gBufferTarget->ActiveOutAndBindTwo(0);
			//	ssaoTarget->ActiveOutAndBind(2);
				noiseTexture->Active(2);
				renderQuad();
				ssaoTarget->RestFrame();
				ssaoBlurFBOTarget->UseForDraw();
				glClear(GL_COLOR_BUFFER_BIT);
				shaderSSAOBlur->use();
				ssaoTarget->ActiveOutAndBind();
				renderQuad();
				ssaoBlurFBOTarget->RestFrame();
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
				gBufferTarget->ActiveOutAndBindAll();
				ssaoBlurFBOTarget->ActiveOutAndBind(3);
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
			// lighting info
 // -------------
			glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
			glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);
			std::vector<glm::vec3> ssaoKernel;
			Texture* noiseTexture = nullptr;
			ModelRender* backpack = nullptr;
			const unsigned int SCR_WIDTH = 2000;
			const unsigned int SCR_HEIGHT = 1280;
			RenderTargetSingle* ssaoTarget = nullptr;
			RenderTargetSingle* ssaoBlurFBOTarget = nullptr;
			RenderTargetThree* gBufferTarget = nullptr;
			Shader* shaderGeometryPass =  nullptr;
			Shader* shaderLightingPass = nullptr;
			Shader* shaderSSAO = nullptr;
			Shader* shaderSSAOBlur = nullptr;
		};


		/*

		*/
		class TrangleRender : public Render {
		public:
			TrangleRender() {}
			TrangleRender(Shader* shader) :Render(shader) {}

			virtual void CreateGeometry() {
				float vertices[] = {
			 -0.5f, -0.5f, 0.0f, // left  
			  0.5f, -0.5f, 0.0f, // right 
			  0.0f,  0.5f, 0.0f  // top   
				};
				for (int i = 0; i < 9; i++) {
					points.push_back(vertices[i]);
				}
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(points.data()[0]) * points.size(), points.data(), GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
				// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
				glBindVertexArray(0);

			}
			virtual void Draw() {
				mShader->use();
				glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}

		};
	}
}