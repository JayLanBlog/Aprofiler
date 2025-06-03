#pragma once
#include <device/gpu/gl/shader/gl_shader.h>
#include <device/gpu/gl/common/gl_primitive.h>
#include <device/gpu/gl/manager/resource_manager.h>
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