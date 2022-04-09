#include<iostream>
#include<cstdint>

// SDL2
#include<SDL2/SDL.h>

// GLAD
#include<glad/glad.h>

// ImGui
#include<imgui.h>
#include<imgui_impl_sdl.h>
#include<imgui_impl_opengl3.h>
#include<implot.h>
#include<ImGuiFileBrowser.h>

// Diaflow
#include<flow.h>

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}	

	SDL_GL_LoadLibrary(nullptr);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_Window* window = SDL_CreateWindow("Diaflow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if(!window)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(!context)
	{
		std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	gladLoadGLLoader(SDL_GL_GetProcAddress);

	SDL_GL_SetSwapInterval(1);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init();

	ImGuiIO& io = ImGui::GetIO();

	Diaflow::Program program;
	program["main"] = std::make_pair(
		Diaflow::Args(),
		Diaflow::Comp
		{
			new Diaflow::If("a == b",
				Diaflow::Comp
				{
					new Diaflow::Switch("a", Diaflow::Cases
					{
						Diaflow::Case("\"x\"", Diaflow::Comp{}),
						Diaflow::Case("\"y\"", Diaflow::Comp{}),
					}),
				},
				Diaflow::Comp
				{
					new Diaflow::Switch("b", Diaflow::Cases
					{
						Diaflow::Case("\"x\"", Diaflow::Comp{}),
						Diaflow::Case("\"y\"", Diaflow::Comp{}),
					}),
			}),
		}
	);

	std::cout << program.xml_string() << std::endl;

	bool running = true;
	while(running)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					running = false;
					break;

				default:
					ImGui_ImplSDL2_ProcessEvent(&event);
					break;
			};
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(io.DisplaySize);

		if(ImGui::Begin("DiaflowEditor", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar))
		{
			if(ImGui::BeginMenuBar())
			{	
				if(ImGui::BeginMenu("File"))
				{
					if(ImGui::MenuItem("New", "Ctrl+N"))
					{
						std::cout << "New" << std::endl;
					}
					
					if(ImGui::MenuItem("Open", "Ctrl+O"))
					{
						std::cout << "Open" << std::endl;
					}
					
					if(ImGui::MenuItem("Save", "Ctrl+S"))
					{
						std::cout << "Save" << std::endl;
					}
					
					if(ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
					{
						std::cout << "Save As" << std::endl;
					}

					if(ImGui::MenuItem("Exit", "Ctrl+Q"))
					{
						running = false;
					}

					ImGui::EndMenu();
				}

				if(ImGui::BeginMenu("Edit"))
				{
					if(ImGui::MenuItem("Undo", "Ctrl+Z"))
					{
						std::cout << "Undo" << std::endl;
					}
					
					if(ImGui::MenuItem("Redo", "Ctrl+Y"))
					{
						std::cout << "Redo" << std::endl;
					}
					
					if(ImGui::MenuItem("Options", "Ctrl+,"))
					{
						std::cout << "Options" << std::endl;
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	SDL_DestroyWindow(window);
	SDL_Quit();
}

