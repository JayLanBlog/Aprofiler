#pragma once
#include <device/gpu/gl/common/gl_gpu_define.h>
#include <iostream>
namespace APROFILER {

	namespace GL {
		class RenderBuffer {
		public:
			
			RenderBuffer() {
			
			}

			void Create() {
				glGenRenderbuffers(1, &rbo);
			}

			void Use() {
				glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			}

			void Make(unsigned int width, unsigned int height) {
				Use();
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
				// finally check if framebuffer is complete
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << "Framebuffer not complete!" << std::endl;	
			}

		public:
			unsigned int rbo = 0;

		};
	}
}