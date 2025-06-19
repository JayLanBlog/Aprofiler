#pragma once
#include <device/gpu/gl/common/gl_gpu_define.h>
#include <string>
#include <stb_image.h>
#include <device/gpu/gl/common/gl_gpu_define.h>
#include <iostream>
namespace APROFILER {

	namespace GL {
		struct TextureOpts
		{
			GLint min = GL_NEAREST;
			GLint mag = GL_NEAREST;
			GLint wrap_s = GL_REPEAT;
			GLint wrap_t = GL_REPEAT;
			GLint type = GL_FLOAT;

			int Width, Height;
		};


		class Texture {
		public:
			Texture() = default;
			void Use() {
				glBindTexture(GL_TEXTURE_2D, texture_id);
			}

			void Cretate() {
				glGenTextures(1, &texture_id);
			}

			void Load(std::string path) {
				mPath = path;
				Cretate();
				unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
				if (data) {
					if (nrComponents == 1)
						color = ColorSpace::RED;
					else if (nrComponents == 3)
						color = ColorSpace::RGB;
					else if (nrComponents == 4)
						color = ColorSpace::RGBA;
					minFilter = TextureFilter::MIPMAP_LINEAR;
					//generateTexture(data);
					Make(data);
					stbi_image_free(data);
				}
				else {
					std::cout << "Texture failed to load at path: " << path << std::endl;
					stbi_image_free(data);
				}
			}
			
			void Make(void* data) {
				Use();
				glTexImage2D(GL_TEXTURE_2D, 0, COLOR_SPACE[static_cast<int>(color)], width, height, 0, COLOR_SPACE[static_cast<int>(color)], DATATYPES[static_cast<int>(type)], data);
				//	PFNGLGENERATEMIPMAPPROC();
				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TEXTURE_WRAPS[static_cast<int>(sWrap)]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TEXTURE_WRAPS[static_cast<int>(tWrap)]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TEXTUREFILTERS[static_cast<int>(minFilter)]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TEXTUREFILTERS[static_cast<int>(magFilter)]);
			}
			// 4, 4,GL_NEAREST,GL_NEAREST,GL_REPEAT,GL_REPEAT,GL_RGBA32F,GL_RGB,GL_FLOAT,&ssaoNoise[0]

			void Make(int w,int h,GLint min, GLint mag, GLint wrap_s, GLint wrap_t, GLint insternalFormat, GLint format, GLenum t,void* data) {
				Use();
				glTexImage2D(GL_TEXTURE_2D, 0, insternalFormat, w, h, 0, format,t, data);
				//	PFNGLGENERATEMIPMAPPROC();
				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TEXTUREFILTERS[static_cast<int>(magFilter)]);
			}

			void Active(int index){
				glActiveTexture(GL_TEXTURE0+index);
			}

			void ConfigTexture(TextureFilter min, TextureFilter mag, Wrap s, Wrap t, ColorSpace colorSpace, DataType dType) {
				minFilter = min;
				magFilter = mag;
				sWrap = s;
				tWrap = t;
				color = colorSpace;
				type = dType;
			}
			std::string name;
			unsigned int texture_id = 0;
		private:
			
			std::string mPath;
			int width = 0, height = 0, nrComponents = 0;
			
			ColorSpace color = ColorSpace::RGB;
			DataType type = DataType::UNSIGNED_BYTE;
			TextureFilter minFilter = TextureFilter::MIPMAP_LINEAR;
			TextureFilter magFilter = TextureFilter::LINEAR;
			Wrap sWrap = Wrap::REPEAT;
			Wrap tWrap = Wrap::REPEAT;
		};
	}
}