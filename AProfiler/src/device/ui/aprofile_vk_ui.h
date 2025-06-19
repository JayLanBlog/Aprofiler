#pragma once
#include "aprofiler_ui.h"



namespace APROFILER {
	class AProfilerVKUI : public AProflierUI {
	public:
		AProfilerVKUI() = default;
		~AProfilerVKUI() {
			Destroy();
		}

		virtual void Create() {
			

		};

		virtual void MakeContextForUI(SDL_Window* window, SDL_GLContext gl_context) {
		
		
		}

		virtual void NewUIFrame() {
			
		}

		virtual void EndUIFrme() {
			
		}


		virtual void SubmitImguiDataForPresent() {
			
		}

		virtual void ProccessEvent(SDL_Event event) {

		}

		virtual void Destroy() {
			/*
			*Detail
			*/
		};
	};
}