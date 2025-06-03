#pragma once
#include <device/gpu/gl/load/assimp_loader.h>

namespace APROFILER {

	namespace GL {	

		class ModelManager {
		private:
			AssimpProxy* proxy;

			ModelManager() {
				string path = "resource/obj/backpack/backpack.obj";
				proxy = new AssimpProxy();
				ModelRender* render = proxy->Load(path);
				render->id = count++;
				models.push_back(*render);
			}
			int count = 0;
			
		public:
			vector<ModelRender> models;
			static ModelManager* getManager() {
				if (!manager) {
					manager = new ModelManager();
				}
				return manager;
			}
			//Model* laodModelFromAssmip(string const& path);
			static ModelManager* manager;
		};
	
	}
}