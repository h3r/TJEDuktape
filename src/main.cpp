/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com

	MAIN:
	 + This file creates the window and the game instance. 
	 + It also contains the mainloop
	 + This is the lowest level, here we access the system to create the opengl Context
	 + It takes all the events from SDL and redirect them to the game
*/

#include "includes.h"

#include "framework.h"
#include "mesh.h"
#include "camera.h"
#include "utils.h"
#include "input.h"
#include "game.h"

#include <iostream> //to output

long last_time = 0; //this is used to calcule the elapsed time between frames

Game* game = NULL;
SDL_GLContext glcontext;

// *********************************
//create a window using SDL
SDL_Window* createWindow(const char* caption, int width, int height, bool fullscreen = false)
{
    int multisample = 8;
    bool retina = true; //change this to use a retina display

	//set attributes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16); //or 24
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//antialiasing (disable this lines if it goes too slow)
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multisample ); //increase to have smoother polygons

	// Initialize the joystick subsystem
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	//create the window
	SDL_Window *window = SDL_CreateWindow(caption, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|
                                          (retina ? SDL_WINDOW_ALLOW_HIGHDPI:0) |
                                          (fullscreen?SDL_WINDOW_FULLSCREEN_DESKTOP:0) );
	if(!window)
	{
		fprintf(stderr, "Window creation error: %s\n", SDL_GetError());
		exit(-1);
	}
  
	// Create an OpenGL context associated with the window.
	glcontext = SDL_GL_CreateContext(window);

	//in case of exit, call SDL_Quit()
	atexit(SDL_Quit);

	//get events from the queue of unprocessed events
	SDL_PumpEvents(); //without this line asserts could fail on windows

	//launch glew to extract the opengl extensions functions from the DLL
	#ifdef USE_GLEW
		glewInit();
	#endif

	int window_width, window_height;
	SDL_GetWindowSize(window, &window_width, &window_height);
	std::cout << " * Window size: " << window_width << " x " << window_height << std::endl;
	std::cout << " * Path: " << getPath() << std::endl;
	std::cout << std::endl;

	return window;
}


//The application main loop
void mainLoop()
{
	SDL_Event sdlEvent;

	long start_time = SDL_GetTicks();
	long now = start_time;
	long frames_this_second = 0;

	while (!game->must_exit)
	{
		Input::update();

		//update events
		while(SDL_PollEvent(&sdlEvent))
		{
			ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
			switch (sdlEvent.type)
			{
			case SDL_QUIT: return; break; //EVENT for when the user clicks the [x] in the corner
			case SDL_MOUSEBUTTONDOWN: //EXAMPLE OF sync mouse input
				Input::mouse_state |= SDL_BUTTON(sdlEvent.button.button);
				game->onMouseButtonDown(sdlEvent.button);
				break;
			case SDL_MOUSEBUTTONUP:
				Input::mouse_state &= ~SDL_BUTTON(sdlEvent.button.button);
				game->onMouseButtonUp(sdlEvent.button);
				break;
			case SDL_MOUSEWHEEL:
				Input::mouse_wheel += sdlEvent.wheel.y;
				Input::mouse_wheel_delta = sdlEvent.wheel.y;
				game->onMouseWheel(sdlEvent.wheel);
				break;
			case SDL_KEYDOWN:
				game->onKeyDown(sdlEvent.key);
				break;
			case SDL_KEYUP:
				game->onKeyUp(sdlEvent.key);
				break;
			case SDL_JOYBUTTONDOWN:
				game->onGamepadButtonDown(sdlEvent.jbutton);
				break;
			case SDL_JOYBUTTONUP:
				game->onGamepadButtonUp(sdlEvent.jbutton);
				break;
			case SDL_TEXTINPUT:
				// you can read the ASCII character from sdlEvent.text.text 
				break;
			case SDL_WINDOWEVENT:
				switch (sdlEvent.window.event) {
				case SDL_WINDOWEVENT_RESIZED: //resize opengl context
					game->onResize(sdlEvent.window.data1, sdlEvent.window.data2);
					break;
				}
			}
		}
		
		//swap between front buffer and back buffer
		SDL_GL_SwapWindow(game->window);

		//compute delta time
		long last_time = now;
		now = SDL_GetTicks();
		double elapsed_time = (now - last_time) * 0.001; //0.001 converts from milliseconds to seconds
		double last_time_seconds = game->time;
        game->time = float(now * 0.001);
		game->elapsed_time = elapsed_time;
		game->frame++;
		frames_this_second++;
		if (int(last_time_seconds *2) != int(game->time*2)) //next half second
		{
			game->fps = frames_this_second*2;
			frames_this_second = 0;
		}
		//ImGui::NewFrame();
		//ImGui::Render();

		//update game logic
		game->update(elapsed_time);

		//render frame
		game->render();

		game->renderDebug(game);


		//check errors in opengl only when working in debug
		#ifdef _DEBUG
			checkGLErrors();
		#endif
	}

	Script::Destroy();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(game->window);
	SDL_Quit();

	return;
}

int main(int argc, char **argv)
{
	std::cout << "Initiating game..." << std::endl;

	//prepare SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	bool fullscreen = false; //change this to go fullscreen
	Vector2 size(800,600);

	if(fullscreen)
		size = getDesktopSize(0);

	//create the game window (WINDOW_WIDTH and WINDOW_HEIGHT are two macros defined in includes.h)
	SDL_Window* window = createWindow("TJE", (int)size.x, (int)size.y, fullscreen );
	if (!window)
		return 0;
	int window_width, window_height;
	SDL_GetWindowSize(window, &window_width, &window_height);

	// Setup Dear ImGui context
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer bindings
		const char* glsl_version = "#version 130";
		ImGui_ImplSDL2_InitForOpenGL(window, glcontext);
		ImGui_ImplOpenGL3_Init(glsl_version);

		// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Read 'docs/FONTS.txt' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != NULL);

		// @RemoteImgui begin
		//ImGui::RemoteInit("0.0.0.0", 7002); // local host, local port
	}

	Input::init(window);
	Script::Create();

	//launch the game (game is a global variable)
	game = new Game(window_width, window_height, window);

	//main loop, application gets inside here till user closes it
	mainLoop();


	//save state and free memory

	return 0;
}
