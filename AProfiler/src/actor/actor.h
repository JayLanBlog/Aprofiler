#pragma once
#include <device/gpu/gl/render/gl_render.h>
#include <device/gpu/gl/manager/model_manager.h>
#include <device/gpu/gl/camera/camera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace APROFILER::GL;


namespace APROFILER {

	namespace ACTOR {
		class Actor {
		public:
			Actor() {
				//Shader * shader = new Shader("resource/shader/hello.vs", "resource/shader/hello.fs");
				//New Only For Allocating Memory
				render = new PrimitiveRender();
			}
			virtual ~Actor() {
			
			}
			virtual void MakeActor() {
				Shader* shader = new Shader("resource/shader/hello.vs", "resource/shader/hello.fs");
				render->AddShader(shader);
				render->CreateGeometry();
			}
			virtual void PreSet() {}
			
			virtual void PostSet(){}

			virtual void Draw() {
				render->Draw();
			}

			virtual void Destory() {
				if (render) {
					delete render;
				}
			}

		protected:
			Render* render = nullptr;
		};

		class ModelActor : public Actor {
		public:
			glm::mat4 projection;
			Camera* camera;
			const unsigned int SCR_WIDTH = 800;
			const unsigned int SCR_HEIGHT = 600;
			Shader* shader = nullptr;
			ModelActor() {
		
				camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
			}
			virtual void MakeActor() {
				shader  = new Shader("resource/shader/1.model_loading.vs", "resource/shader/1.model_loading.fs");
				
				ModelManager* manager = ModelManager::getManager();
				render = &manager->models[0];
				render->AddShader(shader);
			}

			virtual void PostSet() {}

			virtual void Draw() {
				glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
				glm::mat4 view = camera->GetViewMatrix();
				shader->setMat4("projection", projection);
				shader->setMat4("view", view);
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
				shader->setMat4("model", model);
				render->Draw();
			}
		};

		class PointActor : public Actor {
		public:
			PointActor(){
				render = new PointRender();
			}

			virtual void MakeActor() {
				Shader* shader = new Shader("resource/shader/hello.vs", "resource/shader/hello.fs");
				render->AddShader(shader);
				float vertices[] = {
		 -0.5f, -0.5f, 0.0f, // left  
		  0.5f, -0.5f, 0.0f, // right 
		  0.0f,  0.5f, 0.0f  // top   
				};
				render->Clean();
				for (int i = 0; i < 9; i++) {
					render->PusPoint(vertices[i]);
				}
				render->CreateGeometry();
			}
			virtual void PreSet() {}

			virtual void PostSet() {}

			virtual void Draw() {
				render->Draw();
			}

			
		private:
			/*
				x,y,z  
			*/
			std::vector<float> points;


		};
	}

}