#include "scriptconsole.h"
#include "includes.h"
#include "scripts.h"
#include "input.h"

static char buff[MAX_PATH] = "";
//		ImGui::InputText("name", buff, MAX_PATH);
std::vector<std::string> Script::Console::items;
std::vector<std::string> Script::Console::commands;
					bool Script::Console::toggle_console;
		            char Script::Console::input_command[MAX_PATH] = "";
static void  Strtrim(char* str) { char* str_end = str + strlen(str); while (str_end > str&& str_end[-1] == ' ') str_end--; *str_end = 0; }

void Script::Console::Draw()
{
	if (!ImGui::Begin("Console", &Script::Console::toggle_console))
	{
		ImGui::End();
		return;
	}

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Close Console"))
			toggle_console = false;
		ImGui::EndPopup();
	}

	//Text window
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::Selectable("Clear"))
			ClearLog();
		ImGui::EndPopup();
	}
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

	for (auto& item : items)
	{
		bool pop_color = false;
		auto str = item.c_str();
		if (strstr(str, "[error]")) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; }
		else if (strstr(str, "[load]")) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, .08f, 1.0f)); pop_color = true; }
		else if (strncmp(str, "# ", 2) == 0) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f)); pop_color = true; }
		else if (strstr(str, "[warn]")) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f)); pop_color = true; }
		ImGui::TextUnformatted(str);
		if (pop_color)
			ImGui::PopStyleColor();
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();

	//Command line
	ImGui::Separator();
	bool reclaim_focus = false;
	
	static int last_command = -1;
	if (Input::wasKeyPressed(SDL_SCANCODE_UP,   true) && commands.size() > 0)
		last_command = min(commands.size()-1,max(-1, last_command+1));
	
	if (Input::wasKeyPressed(SDL_SCANCODE_DOWN, true) && commands.size() > 0)
		last_command = min(commands.size()-1,max(-1, last_command-1));

	if (last_command != -1)
		strcpy(input_command, commands[last_command].c_str());

	if (ImGui::InputText("Input", input_command, IM_ARRAYSIZE(input_command), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AlwaysInsertMode))
	{
		char* s = input_command;
		Strtrim(s);
		if (s[0])
			ExecCommand(s);

		strcpy(s, "");
		last_command = -1;
		reclaim_focus = true;
	}

	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

	ImGui::End();
}

void Script::Console::ClearLog() {
	items.clear();
}