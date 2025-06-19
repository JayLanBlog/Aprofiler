#pragma once

#include <glad/glad.h>
#include <device/gpu/gl/common/gl_frame_buffer.h>
#include <device/gpu/gl/common/gl_render_bffer.h>
namespace APROFILER {

	namespace GL {

		class RenderTarget {
		public:
			RenderTarget() {

			}

			RenderTarget(int w, int h) :width(w), heigh(h) {

			}

		public:
			int width = 0;
			int heigh = 0;
			FrameBuffer* frameBuffer = nullptr;
			RenderBuffer* renderBuffer = nullptr;
		};

		class RenderTargetSingle : public RenderTarget {
		public:
			RenderTargetSingle() = default;
			RenderTargetSingle(int w, int h) :RenderTarget(w, h) {
				frameBuffer = CreateSingleTextureFrameBuffer(width, heigh);
			}
			void UseForDraw() {
				frameBuffer->Use();
			}

			void RestFrame() {
				frameBuffer->Reset();
			}
			void ActiveOutAndBind() {
				frameBuffer->Active();
			}

			void ActiveOutAndBind(int index) {
				frameBuffer->Active(index);
			}
		};

		class RenderTargetThree : public RenderTarget {
		public:
			RenderTargetThree() = default;
			RenderTargetThree(int w, int h) :RenderTarget(w, h) {
				

			}
			void Make() {
				frameBuffer = CreateMultiTextureFrameBuffer(width, heigh);
				RenderToTarget();

				renderBuffer = new RenderBuffer();
				renderBuffer->Create();
				renderBuffer->Make(width,heigh);
				frameBuffer->Reset();
			}

			void UseForDraw() {
				frameBuffer->Use();
			}

			void RestFrame() {
				frameBuffer->Reset();
			}


			void ActiveOutAndBindAll() {
				frameBuffer->Active();
			}

			void ActiveOutAndBindTwo(int idx) {
				frameBuffer->ActiveTwo(idx);
			}


			void RenderToTarget() {
				frameBuffer->DrawBuffer();
			}



		};

	}
}