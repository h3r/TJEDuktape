#pragma once

#ifndef SCRIPTS_H
#define SCRIPTS_H

#include "duktape.h"
#include "dukglue.h"
#include "duk_console.h"
#include "scriptconsole.h"

namespace Script
{
	extern duk_context* ctx;

	void Create();
	void Destroy();
	void Load(const std::string path);
	void ExecCommand(char* command);
	void ReloadAll();
	void DrawConsole();
	void toggleConsole();
}

#endif