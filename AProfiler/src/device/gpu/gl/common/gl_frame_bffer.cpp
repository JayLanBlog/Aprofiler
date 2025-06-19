#include "gl_frame_buffer.h"

namespace APROFILER {

	namespace GL {
		FrameBuffer* CreateSingleTextureFrameBuffer(int width, int height) {
			FrameBuffer* singleBuffer = new FrameBuffer();
			singleBuffer->Create();
			singleBuffer->Make(singleBuffer->attachs[0], width, height, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_RED, GL_RED, GL_FLOAT);
			return singleBuffer;
		}


		FrameBuffer* CreateMultiTextureFrameBuffer(int width, int height) {
			FrameBuffer* singleBuffer = new FrameBuffer(3);
			singleBuffer->Create();
			singleBuffer->Make(singleBuffer->attachs[0], width, height, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGBA16F, GL_RGBA, GL_FLOAT);
			singleBuffer->Make(singleBuffer->attachs[1], width, height, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_RGBA16F, GL_RGBA, GL_FLOAT);
			singleBuffer->Make(singleBuffer->attachs[2], width, height, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
			return singleBuffer;
		}
	}
}