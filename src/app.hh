#ifndef APP_HH__HEADER_GUARD__
#define APP_HH__HEADER_GUARD__

#include <string>        // std::string, std::stoi
#include <vector>        // std::vector
#include <array>         // std::array
#include <map>           // std::map
#include <unordered_map> // std::unordered_map
#include <stdexcept>     // std::runtime_error
#include <cctype>        // std::toupper
#include <cstdint>       // std::uint64_t, std::uint8_t
#include <cstdio>        // std::FILE, popen, pclose

#include "ncurses.hh"
#include "defaults.hh"
#include "types.hh"
#include "utils.hh"
#include "ini.hh"
#include "editor.hh"
#include "clipboard.hh"

void Init(const std::vector<std::string> &p_args);
void Finish();

void Run();

void Input();
void Render();
void Update();

short StringToColor(const std::string &p_str);

void Defaults();
void ReadOrFixConfig();
void ReadConfigFile();
void WriteConfigFile();
void UpdateTheme();

std::fs::path ConfigPath();

struct Config {
	bool ruler, trimOnSave, scrollBar;
	std::uint8_t indentSize;
	std::size_t  markedColumn;
	std::string  theme;
	wchar_t      separator;
}; // struct Config

struct ColorPairKeyPair {
	ColorPairKeyPair(const std::string &p_section, const std::string &p_key, short p_colorPair);

	std::string section;
	std::string key;
	short colorPair;
}; // struct ColorPairKeyPair

extern Config g_config;

extern std::array<ColorPairKeyPair, NC::ColorPair::Count - 1> g_keyToColorPairMap;

extern std::unordered_map<std::string, short> g_stringColorMap;
extern std::vector<std::string> g_args;

extern bool  g_quit;
extern Vec2D g_winSize;

namespace Editors {
	void Input(NC::input_t p_input);

	Editor Construct(
		const std::string &p_title, const std::vector<std::string> &p_contents = {""},
		bool p_readOnly = false
	);

	void UpdateEditorConfig(Editor &p_editor);
	void UpdateAllEditorConfigs();

	extern std::vector<Editor> g_list;
	extern std::size_t g_currentIdx;
} // namespace Commands

namespace Commands {
	std::vector<std::string> Parse(std::string p_cmd);
	void Run(std::vector<std::string> p_tokens);

	extern const std::map<std::string, std::pair<bool, char>> g_list;
} // namespace Commands

namespace TopBar {
	enum class State {
		None = 0,
		Input,
		Error,
		Question
	}; // enum class State

	void Init();
	void Finish();

	void Input(NC::input_t p_input);
	void CommandInput(NC::input_t p_input);
	void QuestionInput(NC::input_t p_input);

	void Render();
	void RenderInputBar();
	void RenderMessage();
	void RenderTabs();
	void RenderMatchesList();

	void Update();
	void GetMatchingCommands();
	void GetMatchingThemes();
	std::size_t CalcCurrentToken();

	void ResizeWindow();

	void SetInput(const std::string &p_input);

	void Ask(const std::string &p_question);
	void Error(const std::string &p_msg);
	void Reset();
	void PosCursor();

	extern WINDOW *g_win, *g_matchListWin;

	extern State g_state;

	extern std::string g_input;
	extern bool        g_answer;

	extern std::size_t g_cursorX, g_scrollX, g_length;

	extern std::vector<std::string> g_history;
	extern std::size_t g_historyPos;

	extern std::vector<std::string> g_matches, g_tokens;
	extern std::size_t g_currentTokenIdx;
} // namespace TopBar

#endif
