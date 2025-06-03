#pragma once
#include "imgui.h"
#include "com/file_system.h"

namespace APROFILER {
	namespace VISUAL {
		enum class EVETTYPE
		{


		};

		struct VisualData
		{
			float threshold = 0;
			float window_height = 0;
			float window_width = 0;
			int current_image = 0;
			int image_count = 0;
			char** iamge_files = 0;
			void GetImagesSource() {
				
			}
		};

		class VisualView {
		public:
			
		
		private:
			VisualData* visualGUI = nullptr;
		};
	
	}
}