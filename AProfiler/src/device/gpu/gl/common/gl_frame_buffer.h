#pragma once
#include <device/gpu/gl/common/gl_gpu_define.h>
#include <device/gpu/gl/common/gl_attachment.h>
#include <vector>
namespace APROFILER {

	namespace GL {
		class FrameBuffer {
		public:
			FrameBuffer() {
				
			}
			FrameBuffer(int count) {
				attachCount = count;
			}
			~FrameBuffer() {
				for (int i = 0; i < attachs.size(); i++) {
					if (attachs[i]) {
						delete attachs[i];
					}
				}
			}

			void Create() {
				glGenFramebuffers(1, &fbi);  
				for (int i = 0; i < attachCount;i++) {
					Attachment* at = new Attachment();
					at->attachId = i;
					attachs.push_back(at);
				}
			}
			void Make(Attachment* att) {
				att->Make();
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << "SSAO Framebuffer not complete!" << std::endl;
			}

			void Make(Attachment* att ,int w, int h, GLint min, GLint mag, GLint wS, GLint wT, GLint inter, GLint fm, GLenum t) {
				Use();
				att->Make(w, h, min, mag, wS, wT, inter, fm, t);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+att->attachId, GL_TEXTURE_2D, att->outputTexture->texture_id, 0);
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << "SSAO Framebuffer not complete!" << std::endl;
			}

			void DrawBuffer() {
				unsigned int* attachments = new unsigned int[attachCount];
				for (int i = 0; i < attachCount; i++) {
					attachments[i] = GL_COLOR_ATTACHMENT0 + i;
				}
				glDrawBuffers(attachCount, attachments);
			}

			void Use() {
				glBindFramebuffer(GL_FRAMEBUFFER, fbi);
			}

			void Reset() {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			void Active(int index =0) {
				for (int i = 0; i < attachCount; i++) {
					attachs[i]->Active(i + index);
				}
			}

			void ActiveFirst(int index = 0) {
				for (int i = 0; i < 1; i++) {
					attachs[i]->Active(i + index);
				}
			}

			void ActiveTwo(int index = 0) {
				if (attachCount>=2) {
					for (int i = 0; i < 2; i++) {
						attachs[i]->Active(i + index);
					}
				}
			}

			//Color Attachment 

			// Textures
			std::vector<Attachment*> attachs;

			int attachCount = 1;

			unsigned int fbi = 0;
		};



		FrameBuffer* CreateSingleTextureFrameBuffer(int width, int height);

		FrameBuffer* CreateMultiTextureFrameBuffer(int width, int height);
	}
}