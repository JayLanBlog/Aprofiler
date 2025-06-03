#pragma once
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <SDL3/SDL.h>¡¢
#include <common/core.h>
//Vulkan Impl
#include <SDL3/SDL_vulkan.h> 
//OpengGL Impl
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#include "imgui_impl_opengl3.h"
#endif


namespace APROFILER {

	class AProflierUI {
	public:
		AProflierUI() = default;
		virtual ~AProflierUI() {
		
		}
		virtual void Create() {

		};
		virtual void MakeContextForUI(SDL_Window* window, SDL_GLContext gl_context) = 0;

		virtual void NewUIFrame() = 0;

		virtual void EndUIFrme() = 0;

		virtual void SubmitImguiDataForPresent() = 0;

		virtual void ProccessEvent(SDL_Event event) = 0;

		virtual void Destroy() {
		};
	};

	class AProfilerGLUI : public AProflierUI {
	public:
		AProfilerGLUI() = default;
		~AProfilerGLUI() {
			Destroy();
		}
		virtual void Create() {
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui::StyleColorsDark();
		};

		virtual void MakeContextForUI(SDL_Window* window, SDL_GLContext gl_context) {
			ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
#if defined(IMGUI_IMPL_OPENGL_ES2)
			// GL ES 2.0 + GLSL 100 (WebGL 1.0)
			const char* glsl_version = "#version 100";
#elif defined(IMGUI_IMPL_OPENGL_ES3)
			// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
			const char* glsl_version = "#version 300 es";
#elif defined(__APPLE__)
			// GL 3.2 Core + GLSL 150
			const char* glsl_version = "#version 150";
#else
			// GL 3.0 + GLSL 130
			const char* glsl_version = "#version 130";
#endif
			ImGui_ImplOpenGL3_Init(glsl_version);
		};

		virtual void NewUIFrame() {
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();
		};

		virtual void EndUIFrme() {
			ImGui::Render();
		};

		virtual void SubmitImguiDataForPresent() {
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		};

		virtual void ProccessEvent(SDL_Event event) {
			ImGui_ImplSDL3_ProcessEvent(&event);
		}

		virtual void Destroy() {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL3_Shutdown();
			ImGui::DestroyContext();
		};

	};

}