#pragma once
#include <device/ui/aprofiler_ui.h>

namespace APROFILER {
	class ProfilerDevice {
	public:
		
		virtual void Create() = 0;

		virtual void SetWindowSize(float window_width, float window_height) = 0;

		virtual void MakeContextForDevice() = 0;

		virtual void Update() = 0;

		virtual bool ConfigMinSizeIFNeed() = 0;

		virtual bool ProcessEvent(SDL_Event event) = 0;

		virtual void UpdareViewPort(ImVec4 view, ImVec4 clear_color) = 0;

		virtual void Destory() = 0;

		virtual void SubmitForPresent() = 0;
	};

	class ProfilerGLDevice : public ProfilerDevice {
	public:

		ProfilerGLDevice() {
			profile_ui = new AProfilerGLUI();
		}

		~ProfilerGLDevice() {
			Destory();
		}

		virtual void Create();

		virtual void MakeContextForDevice();

		virtual void Update();

		virtual bool ConfigMinSizeIFNeed();

		virtual bool ProcessEvent(SDL_Event event);

		virtual void Destory();

		virtual void UpdareViewPort(ImVec4 view, ImVec4 clear_color);

		virtual void SubmitForPresent();

		virtual void SetWindowSize(float window_width, float window_height) {
			windowWidth = window_width;
			windowHeight = window_height;
		}

	private:

		void CreateProfilerWindow();

		void CreateProfilerContext();

		void MakeWindow();

	private:
		//window
		SDL_Window* sdlWindow = nullptr;
		float windowWidth = 800, windowHeight = 600;
		AProflierUI* profile_ui;
		SDL_GLContext gl_context = nullptr;
		//comment data
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 0.00f);


	};
}