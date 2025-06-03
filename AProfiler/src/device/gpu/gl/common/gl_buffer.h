#pragma once
#include <device/gpu/gl/common/gl_gpu_define.h>
namespace APROFILER {

	namespace GL {
		class Buffer {
		public:
			Buffer() {
			}
			virtual ~Buffer() {
				if (mData) {
					delete mData;
				}
			}
			virtual void Use() {
				//	glBindVertexArray(VAO);
			}
			virtual void Create() {

			}

			virtual void Make() {

			}

			virtual	void upload_data(void* data, int size, int count) {
			
			}

			int GetStrade() {
				return dataSize;
			};

			int GetSize() {
				return dataSize * dataCount;
			}

			virtual void Draw() {
			
			}

		protected:
			void* mData = nullptr;
			int dataSize = 0;
			int dataCount = 0;
		};

		class VBO : Buffer {
		public:
			VBO() {}

			virtual void Use() {
				//	glBindVertexArray(VAO);
				glBindVertexArray(vao_id);
			}
			virtual void Create() {
				glGenVertexArrays(1, &vao_id);
				glGenBuffers(1, &vbo_id);
			}

			virtual void Make() {
				Use();
				glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
				glBufferData(GL_ARRAY_BUFFER, GetSize(), mData, GL_STATIC_DRAW);
			}

			virtual	void upload_data(void* data, int size, int count) {
				dataSize = size;
				dataCount = count;
				mData = data;
			}
			virtual void Draw() {
				glDrawArrays(GL_TRIANGLES, 0, dataCount);
			}
		private:
			unsigned int vbo_id = 0;
			unsigned int vao_id = 0;
		};


		class IBO :Buffer {
		public:
			IBO() {}

			virtual void Use() {
				//	glBindVertexArray(VAO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
			}

			virtual void Create() {
				glGenBuffers(1, &ebo_id);
			}

			virtual void Make() {
				Use();
				glBufferData(GL_ELEMENT_ARRAY_BUFFER , GetSize(), mData, GL_STATIC_DRAW);
			}

			virtual	void upload_data(void* data, int size, int count) {
				dataSize = size;
				dataCount = count;
				mData = data;
			}
			virtual void Draw() {
				glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(dataCount), GL_UNSIGNED_INT, 0);
			}
		private:
			unsigned int ebo_id = 0;
		};
	}
}