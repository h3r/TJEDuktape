#include "includes.h"
#include "entity.h"

#include <fstream>

#include "utils.h"
#include "includes.h"
#include "register_function.h"

#include <filesystem>
namespace fs = std::filesystem;

using uint = unsigned int;
duk_context* Script::ctx;

void Script::Create() {
	ctx = duk_create_heap_default();
	if (!ctx)	exit(1);

	duk_push_global_object(ctx);
	duk_put_global_string(ctx, "window");
	duk_console_init(ctx,0);

	if (!fs::exists("tmp"))	fs::create_directory("tmp");

	#include "register_things";
}

void Script::Destroy() {
	duk_destroy_heap(ctx);
}


void Script::Load(const std::string path) {

	while (true){

		if (!fs::exists(path)) {
			fatal("Script error: file not found: %s\n", path.c_str());
			continue;
		}

		std::string name = fs::path(path).filename().string();
		std::ifstream ifs(path.c_str());
		std::string content(
			(std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>())
		);
		ifs.close();
		unsigned int size = content.length();

		// compile source code
		if (size > 0) duk_push_lstring(ctx, content.c_str(), (duk_size_t)size);
		else		  duk_push_undefined(ctx);

		if (duk_peval(ctx) != 0) {
			// Use duk_safe_to_string() to convert error into string.  This APIcall is guaranteed not to throw an error during the coercion.

			fatal("Script error: %s\n %s\n", name.c_str(), duk_safe_to_string(ctx, -1));
			continue;
		}
		Console::items.push_back("[load] " + name);
		std::string out = duk_safe_to_string(ctx, -1);
		Console::items.push_back("> " + out);
		duk_pop(ctx);
		break;
	}

}

	



void Script::ReloadAll() {

}

void Script::ExecCommand(char* command)
{
	Script::Console::items.push_back(command);
	Script::Console::commands.push_back(command);

	if (duk_peval_string(ctx, command)) {
		duk_get_prop_string(ctx, -1, "stack");
		//fatal("Script error: %s\n", duk_safe_to_string(Script::ctx, -1));
		std::string error = duk_safe_to_string(ctx, -1);
		Console::items.push_back( "[error] "+ error);
	}
	else {
		std::string out = duk_safe_to_string(ctx, -1);
		Console::items.push_back("> " + out);
	}
	duk_pop(ctx);
}

void Script::toggleConsole() {
	Console::toggle_console = !Console::toggle_console;
}
void Script::DrawConsole() {
	Console::Draw();
}
