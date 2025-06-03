#pragma once
#include <device/gpu/gl/texture/gl_texture.h>
#include <map>

using namespace std;

namespace APROFILER {

	namespace GL {
		class ResourceManager {
		private:
			ResourceManager() {

			}

		static ResourceManager* resmanager;

		public:
			int texture_id = 0;
			std::map<int, Texture> texture_dict;
			std::map<string, int> texture_id_map;
			static ResourceManager* getResManager() {
				if (!resmanager) {
					resmanager = new ResourceManager();
				}
				return resmanager;
			}

			Texture* getTextureByID(int id) {
				return &texture_dict[id];
			}

			int load_texture(string path, string type) {
				if (texture_id_map.find(path) != texture_id_map.end()) {
					cout << "ResourceManager load_texture path already loaded!" << endl;
					return texture_id_map[path];
				}
				cout << "load_texture : " << type << endl;
				Texture* texture = new Texture();
				texture->name = type;
				//path, false, type
				texture->Load(path);
				texture_dict[texture_id] = *texture;
				texture_id_map[path] = texture_id;
				texture_id++;
				return texture_id_map[path];
			}
		};
	
	}
}