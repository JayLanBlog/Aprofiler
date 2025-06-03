#pragma once
#include <device/gpu/gl/common/gl_gpu_define.h>
namespace APROFILER {

	namespace GL {

		struct PrimitiveElement
		{
			int element_count = 0;
			/*
				The offset position of the structure attribute
			*/
			int offset = 0;
			
			ElementSemantic sematic =ElementSemantic::SEM_POSITION;

			ElementType mType = ElementType::TYPE_INT;
				
			int mStrade = 0;
		};
	}
}