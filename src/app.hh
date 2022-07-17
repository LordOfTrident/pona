#ifndef APP_HH__HEADER_GUARD__
#define APP_HH__HEADER_GUARD__

#include <string>        // std::string, std::stoi
#include <vector>        // std::vector
#include <array>         // std::array
#include <map>           // std::map
#include <unordered_map> // std::unordered_map
#include <stdexcept>     // std::runtime_error
#include <cstdint>       // std::uint64_t, std::uint8_t

#include "ncurses.hh"
#include "defaults.hh"
#include "types.hh"
#include "utils.hh"
#include "ini.hh"
#include "editor.hh"
#include "clipboard.hh"

void Init(const std::vector<std::string> &p_args, const std::string &p_title);
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

	extern std::vector<std::string> g_lastMatchCmdTokens;
} // namespace Commands

#endif
