/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This file has all the includes so the app works in different systems.
	It is a little bit low level so do not worry about the code.
*/
#define WIN32_LEAN_AND_MEAN
#ifndef INCLUDES_H
#define INCLUDES_H

//under windows we need this file to make opengl work
#ifdef WIN32 
	#include <windows.h>
#endif

#ifndef APIENTRY
    #define APIENTRY
#endif

#ifdef WIN32
	#define USE_GLEW
	#define GLEW_STATIC
	#include <GL/glew.h>
	#pragma comment(lib, "glew32s.lib")
#endif


//SDL
//#pragma comment(lib, "SDL2.lib")
//#pragma comment(lib, "SDL2main.lib")


#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


#include <GL/glext.h>

//GLUT
#ifdef WIN32
    #include "GL/GLU.h"
	//#include <GL/glut.h>
#endif

#ifdef __APPLE__
    #include "OpenGL/glu.h"
	//#include <GLUT/glut.h>
#endif

#include <iostream>

//remove warnings

//used to access opengl extensions
#define REGISTER_GLEXT(RET, FUNCNAME, ...) typedef RET ( * FUNCNAME ## _func)(__VA_ARGS__); FUNCNAME ## _func FUNCNAME = NULL; 
#define IMPORT_GLEXT(FUNCNAME) FUNCNAME = (FUNCNAME ## _func) SDL_GL_GetProcAddress(#FUNCNAME); if (FUNCNAME == NULL) { std::cout << "ERROR: This Graphics card doesnt support " << #FUNCNAME << std::endl; }


//OPENGL EXTENSIONS

//imgui
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_sdl.h"
#include "imgui-docking/imgui_impl_opengl3.h"


//Scripting
#include "scripts.h"

#include "json.hpp"
using Json = nlohmann::json;

#endif