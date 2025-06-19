#include "glad/glad.h"
#include "profiler_device.h"


namespace APROFILER {

	void ProfilerGLDevice::Create() {

		// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100 (WebGL 1.0)
		const char* glsl_version = "#version 100";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
	// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
		const char* glsl_version = "#version 300 es";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
		const char* glsl_version = "#version 150";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

		// Create window with graphics context
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		CreateProfilerWindow();
		CreateProfilerContext();
		profile_ui->Create();
	}

	void ProfilerGLDevice::MakeContextForDevice() {
		MakeWindow();
		profile_ui->MakeContextForUI(sdlWindow,gl_context);
		//load opengl address 
		gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
	}

	void ProfilerGLDevice::Update() {
		profile_ui->NewUIFrame();
		ImGuiIO& io = ImGui::GetIO();
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;
			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
		
			ImGui::SetWindowFontScale(1.5);
			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}
		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
		profile_ui->EndUIFrme();
		//UpdareViewPort(ImVec4(0, 0, windowWidth,windowHeight), clear_color);
	}
	
	void ProfilerGLDevice::SubmitForPresent() {
		profile_ui->SubmitImguiDataForPresent();
		SDL_GL_SwapWindow(sdlWindow);
	}


	bool ProfilerGLDevice::ConfigMinSizeIFNeed() {
		bool needMin = false;
		// [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
		if (SDL_GetWindowFlags(sdlWindow) & SDL_WINDOW_MINIMIZED)
		{	
			needMin = true;
		}
		return needMin;
	}

	void ProfilerGLDevice::UpdareViewPort(ImVec4 view, ImVec4 clear_color) {
		glViewport(view.x, view.y, (int)view.z, (int)view.w);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
	}


	bool ProfilerGLDevice::ProcessEvent(SDL_Event event) {
		bool close = false;
		profile_ui->ProccessEvent(event);
		if (event.type == SDL_EVENT_QUIT)
			close = true;
		if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(sdlWindow))
			close = true;

		return close;
	}

	void ProfilerGLDevice::Destory() {
		if (profile_ui) {
			delete profile_ui;
		}
		SDL_GL_DestroyContext(gl_context);
		SDL_DestroyWindow(sdlWindow);
	}

	void ProfilerGLDevice::CreateProfilerWindow() {
		Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
		sdlWindow = SDL_CreateWindow("Dear ImGui SDL3+OpenGL3 example", windowWidth, windowHeight, window_flags);
		if (sdlWindow == nullptr)
		{
			printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());

		}
		SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}

	void ProfilerGLDevice::CreateProfilerContext() {
		
		gl_context = SDL_GL_CreateContext(sdlWindow);
		if (gl_context == nullptr)
		{
			printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
			return;
		}
	}


	void ProfilerGLDevice::MakeWindow() {
		SDL_GL_MakeCurrent(sdlWindow, gl_context);
		SDL_GL_SetSwapInterval(1); // Enable vsync
		SDL_ShowWindow(sdlWindow);
	}


}