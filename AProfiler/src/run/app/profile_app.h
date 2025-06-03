#pragma once
#include <device/gpu/gl/common/gl_gpu_define.h>
#include <device/profiler_device.h>
#include <actor/actor.h>

namespace APROFILER {
	class App {
	public:
		App() {}
		virtual void Create() = 0;
		virtual void Init() = 0;
		virtual void Run() = 0;
		virtual void Destroy() = 0;

	};

	class ProfilerApp : public App {
	public:

		ProfilerApp() {
			profilerDevice = new ProfilerGLDevice();
			if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
			{
				printf("Error: SDL_Init(): %s\n", SDL_GetError());
				return;
			}
			Create();
			Init();
		}

		virtual void Create() {
			profilerDevice->SetWindowSize(2000,1280);
			profilerDevice->Create();

			
		}

		virtual void Init() {
			profilerDevice->MakeContextForDevice();
			actor = new ACTOR::ModelActor();
			actor->MakeActor();
		}

		virtual void Run() {
			while (!done) {
				SDL_Event event;
				while (SDL_PollEvent(&event)) {
					done = profilerDevice->ProcessEvent(event);
				}
				if (profilerDevice->ConfigMinSizeIFNeed()) {
					SDL_Delay(10);
					continue;
				}
				profilerDevice->Update();
				actor->Draw();
				profilerDevice->SubmitForPresent();
			}
			Destroy();
		}

		virtual void Destroy() {
			if (profilerDevice) {
				delete profilerDevice;
			}
		}

	private:
		ProfilerDevice* profilerDevice = nullptr;
		ACTOR::Actor* actor = nullptr;
		bool done = false;
	};
}