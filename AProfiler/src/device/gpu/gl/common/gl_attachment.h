#pragma once
#include <device/gpu/gl/common/gl_gpu_define.h>
#include <device/gpu/gl/texture/gl_texture.h>
namespace APROFILER {

	namespace GL {


		class Attachment {
		public:
			Attachment() {
				outputTexture = new Texture();
				outputTexture->Cretate();
			}

			~Attachment() {
				if (outputTexture) {
					delete outputTexture;
				}
			}

			void Make() {
				outputTexture->Make(width,height,minFilter,magFilter,wrap_s,wrap_t,internalFormat,format,type,nullptr);
			}


			void Make(int w, int h, GLint min, GLint mag, GLint wS, GLint wT, GLint inter, GLint fm, GLenum t) {
				outputTexture->Make(w, h, min, mag, wS, wT, inter, fm, t, nullptr);
			}


			void Active(int index = 0) {
				outputTexture->Active(index);
				outputTexture->Use();
			}

			int attachId = 0;
			GLint type = GL_FLOAT;
			GLint wrap_s =GL_REPEAT , wrap_t = GL_REPEAT;
			GLint format = GL_RED,internalFormat = GL_RGBA32F;
			int width = 0, height = 0;
			GLint minFilter = GL_NEAREST;
			GLint magFilter = GL_NEAREST;
			Texture* outputTexture = nullptr;
		};

	}
}