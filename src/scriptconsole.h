#include <vector>
#include <string>

namespace Script {
	namespace Console {
		extern bool toggle_console;
		extern char input_command[];
		extern std::vector<std::string> items;
		extern std::vector<std::string> commands;

		void Draw();
		void ClearLog();
	}
}
