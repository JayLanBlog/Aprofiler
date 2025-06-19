#pragma once
#include "gl_element.h"
#include "gl_buffer.h"
#include <vector>

namespace APROFILER {

	namespace GL {
		class Primitive
		{
		public:
			Primitive() {
				vbo = new VBO();
			}
			~Primitive() {
				if (ibo) {
					delete ibo;
				}
				if (vbo) {
					delete vbo;
				}
			}
			void EnableIBO() {
				ibo = new IBO();
			}

			void CreateVBO() {
				vbo->Create();
			}

			void CreateIBO() {
				if (!ibo) {
					EnableIBO();
				}
				ibo->Create();
			}
			void UploadToVBO(void* data, int unitSize, int count) {
				vbo->upload_data(data, unitSize, count);
			}

			void UploadToIBO(void* data, int unitSize, int count) {
				if (!ibo) {
					CreateIBO();
				}
				ibo->upload_data(data, unitSize, count);
			}

			void Make() {
				vbo->Make();
				if (ibo) {
					ibo->Make();
				}
				int id = 0;
				for (int i = 0; i < elements.size(); i++) {
					GLenum elementType = ELEMENTTYPE[static_cast<int>(elements[i].mType)];
					int eCount = elements[i].element_count;
					elements[i].mType != ElementType::TYPE_INT ?
						glVertexAttribPointer(id, eCount, elementType, elements[i].mType == ElementType::TYPE_UBYTE4_NORM ? GL_TRUE : GL_FALSE, elements[i].mStrade, (void*)elements[i].offset) :
						glVertexAttribIPointer(id, eCount, elementType, elements[i].mStrade, (void*)elements[i].offset);
					glEnableVertexAttribArray(id);
					id++;
				}
			   // auto a =	GL_COLOR_ATTACHMENT0 + 1;
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
				// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
				glBindVertexArray(0);
			}


			void AddElement(PrimitiveElement element) {
				elements.push_back(element);
			}

			void Draw() {
				vbo->Use();
				ibo ? ibo->Draw() : vbo->Draw();
			}
		public:
			int primitiveId = 0;
		private:
			VBO* vbo = nullptr;
			IBO* ibo = nullptr;
			bool enableIBO = false;
			std::vector<PrimitiveElement> elements;


		};

	}
}