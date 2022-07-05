#include "app.hh"

ColorPairKeyPair::ColorPairKeyPair(
	const std::string &p_section, const std::string &p_key, short p_colorPair
):
	section(p_section),
	key(p_key),
	colorPair(p_colorPair)
{}

Config g_config;

// i do not want a thousand if statements, so i made some maps to shorten code
std::array<ColorPairKeyPair, NC::ColorPair::ReservedEnd - 1> g_keyToColorPairMap = {
	ColorPairKeyPair("top_bar", "", NC::ColorPair::TopBar),

	ColorPairKeyPair("top_bar", "input", NC::ColorPair::TopBar_Input),
	ColorPairKeyPair("top_bar", "error", NC::ColorPair::TopBar_Error),

	ColorPairKeyPair("top_bar", "tab",         NC::ColorPair::TopBar_Tab),
	ColorPairKeyPair("top_bar", "tab_current", NC::ColorPair::TopBar_TabCurrent),

	ColorPairKeyPair("top_bar", "list",         NC::ColorPair::TopBar_List),
	ColorPairKeyPair("top_bar", "list_current", NC::ColorPair::TopBar_ListCurrent),

	ColorPairKeyPair("editor", "",        NC::ColorPair::Editor),
	ColorPairKeyPair("editor", "current", NC::ColorPair::Editor_Current),

	ColorPairKeyPair("editor", "ruler",         NC::ColorPair::Editor_Ruler),
	ColorPairKeyPair("editor", "ruler_current", NC::ColorPair::Editor_RulerCurrent),

	ColorPairKeyPair("editor", "marked_column", NC::ColorPair::Editor_MarkedColumn),
	ColorPairKeyPair("editor", "selection",     NC::ColorPair::Editor_Selection),

	ColorPairKeyPair("scroll_bar", "",           NC::ColorPair::Editor_ScrollBar),
	ColorPairKeyPair("scroll_bar", "button",     NC::ColorPair::Editor_ScrollButton),
	ColorPairKeyPair("scroll_bar", "button_alt", NC::ColorPair::Editor_ScrollButtonAlt),
	ColorPairKeyPair("scroll_bar", "back",       NC::ColorPair::Editor_ScrollBack),

	ColorPairKeyPair("info_bar", "",          NC::ColorPair::Editor_InfoBar),
	ColorPairKeyPair("info_bar", "separator", NC::ColorPair::Editor_InfoBarSeparator)
};

std::unordered_map<std::string, short> g_stringColorMap = {
	{"default", NC::Color::Default},

	{"black",   NC::Color::Black},
	{"red",     NC::Color::Red},
	{"green",   NC::Color::Green},
	{"yellow",  NC::Color::Yellow},
	{"blue",    NC::Color::Blue},
	{"magenta", NC::Color::Magenta},
	{"cyan",    NC::Color::Cyan},
	{"white",   NC::Color::White},

	{"grey",           NC::Color::Grey},
	{"bright_red",     NC::Color::BrightRed},
	{"bright_green",   NC::Color::BrightGreen},
	{"bright_yellow",  NC::Color::BrightYellow},
	{"bright_blue",    NC::Color::BrightBlue},
	{"bright_magenta", NC::Color::BrightMagenta},
	{"bright_cyan",    NC::Color::BrightCyan},
	{"bright_white",   NC::Color::BrightWhite}
};

std::vector<std::string> g_args;

bool  g_quit = false;
Vec2D g_winSize;

std::vector<Editor> Editors::g_list;
std::size_t Editors::g_currentIdx = 0;

void Init(const std::vector<std::string> &p_args) {
	g_args = p_args;

	std::setlocale(LC_CTYPE, "");

	initscr();            // init ncurses
	raw();                // raw mode
	noecho();             // dont echo input
	keypad(stdscr, true); // keypad input
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);

	start_color();
	use_default_colors();

	mouseinterval(0);

	ESCDELAY = 0; // no delay when escape is pressed

	getmaxyx(stdscr, g_winSize.y, g_winSize.x);
	erase();

	TopBar::Init();
	refresh();

	Defaults();        // default config
	ReadOrFixConfig(); // read and fix config if needed

	// read the command line history
	if (Utils::PathExists(ConfigPath()/"history"))
		Utils::ReadFile(ConfigPath()/"history", TopBar::g_history);
	else // if it doesnt exist, create it
		Utils::WriteFile(ConfigPath()/"history", "");

	// if no command line args were provided, open a default editor
	if (g_args.size() == 0)
		Editors::g_list.push_back(Editors::Construct("Untitled"));
	else { // else, open the files if possible
		for (const auto &arg : g_args) {
			Editors::g_list.push_back(Editors::Construct(arg));

			if (Utils::PathExists(arg))
				Utils::ReadFile(arg, Editors::g_list.back().buffer.rawBuffer);
		}

		Editors::g_currentIdx = Editors::g_list.size() - 1;
	}
}

void Finish() {
	TopBar::Finish();

	endwin();

	// write the command line history
	Utils::WriteFile(ConfigPath()/"history", TopBar::g_history);

	// write the config file (maybe the config was modified from the program)
	WriteConfigFile();
}

void Run() {
	while (not g_quit) {
		Update();
		Render();
		Input();
	}
}

void Input() {
	NC::input_t input = NC::Input();

	switch (input) {
	case NC::Key::Resize:
		refresh();
		getmaxyx(stdscr, g_winSize.y, g_winSize.x);

		TopBar::ResizeWindow();
		Editors::g_list.at(Editors::g_currentIdx).ResizeWindow(Vec2D(g_winSize.x, g_winSize.y - 1));

		break;

	default: break;
	}

	if (TopBar::g_state == TopBar::State::Question or TopBar::g_state == TopBar::State::Input)
		TopBar::Input(input);
	else
		Editors::Input(input);
}

void Render() {
	curs_set(0); // hide the cursor while drawing

	Editors::g_list.at(Editors::g_currentIdx).Render();

	TopBar::Render();

	// show the cursor
	curs_set(1);
	if (TopBar::g_state == TopBar::State::Input)
		TopBar::PosCursor();
	else
		Editors::g_list.at(Editors::g_currentIdx).PosCursor();
}

void Update() {
	TopBar::Update();
	Editors::g_list.at(Editors::g_currentIdx).Update();
}

short StringToColor(const std::string &p_str) {
	if (g_stringColorMap.count(p_str))
		return g_stringColorMap.at(p_str);
	else
		throw std::runtime_error(p_str + " is not a color");
}

void Defaults() {
	init_pair(NC::ColorPair::TopBar,             NC::Color::Black,  NC::Color::White);
	init_pair(NC::ColorPair::TopBar_Input,       NC::Color::White,  NC::Color::Blue);
	init_pair(NC::ColorPair::TopBar_Error,       NC::Color::Yellow, NC::Color::Red);
	init_pair(NC::ColorPair::TopBar_Tab,         NC::Color::Black,  NC::Color::White);
	init_pair(NC::ColorPair::TopBar_TabCurrent,  NC::Color::Blue,   NC::Color::White);
	init_pair(NC::ColorPair::TopBar_List,        NC::Color::Black,  NC::Color::White);
	init_pair(NC::ColorPair::TopBar_ListCurrent, NC::Color::Blue,   NC::Color::White);

	init_pair(NC::ColorPair::Editor,                  NC::Color::White,  NC::Color::Black);
	init_pair(NC::ColorPair::Editor_Current,          NC::Color::White,  NC::Color::Black);
	init_pair(NC::ColorPair::Editor_Ruler,            NC::Color::Yellow, NC::Color::Black);
	init_pair(NC::ColorPair::Editor_RulerCurrent,     NC::Color::White,  NC::Color::Black);
	init_pair(NC::ColorPair::Editor_ScrollBar,        NC::Color::Cyan,   NC::Color::Blue);
	init_pair(NC::ColorPair::Editor_ScrollButton,     NC::Color::Cyan,   NC::Color::Blue);
	init_pair(NC::ColorPair::Editor_ScrollButtonAlt,  NC::Color::White,  NC::Color::Blue);
	init_pair(NC::ColorPair::Editor_ScrollBack,       NC::Color::Black,  NC::Color::Blue);
	init_pair(NC::ColorPair::Editor_InfoBar,          NC::Color::Black,  NC::Color::White);
	init_pair(NC::ColorPair::Editor_InfoBarSeparator, NC::Color::Black,  NC::Color::White);
	init_pair(NC::ColorPair::Editor_MarkedColumn,     NC::Color::Red,    NC::Color::Black);
	init_pair(NC::ColorPair::Editor_Selection,        NC::Color::Black,  NC::Color::White);

	g_config.ruler        = true;
	g_config.trimOnSave   = false;
	g_config.scrollBar    = true;
	g_config.indentSize   = 4;
	g_config.markedColumn = 0;
	g_config.theme        = "default";
}

void ReadOrFixConfig() {
	// create missing directories
	if (not Utils::PathExists(Utils::Home()/".config"))
		std::fs::create_directory(Utils::Home()/".config");

	if (not Utils::PathExists(ConfigPath()))
		std::fs::create_directory(ConfigPath());

	if (not Utils::PathExists(ConfigPath()/"syntax/"))
		std::fs::create_directory(ConfigPath()/"syntax/");

	if (not Utils::PathExists(ConfigPath()/"themes/"))
		std::fs::create_directory(ConfigPath()/"themes/");

	// check themes
	if (not Utils::PathExists(ConfigPath()/"themes/default.ini"))
		Utils::WriteFile(ConfigPath()/"themes/default.ini", DEFAULT_THEME_DEFAULT);

	if (not Utils::PathExists(ConfigPath()/"themes/classic.ini"))
		Utils::WriteFile(ConfigPath()/"themes/classic.ini", DEFAULT_THEME_CLASSIC);

	if (not Utils::PathExists(ConfigPath()/"themes/tilde.ini"))
		Utils::WriteFile(ConfigPath()/"themes/tilde.ini", DEFAULT_THEME_TILDE);

	if (not Utils::PathExists(ConfigPath()/"themes/16.ini"))
		Utils::WriteFile(ConfigPath()/"themes/16.ini", DEFAULT_THEME_16);

	// check the config file
	if (Utils::PathExists(ConfigPath()/"config.ini"))
		ReadConfigFile();
	else
		WriteConfigFile();
}

void ReadConfigFile() {
	std::string buffer;
	Utils::ReadFile(ConfigPath()/"config.ini", buffer);

	INI::Struct config;

	try {
		config.Parse(buffer);
	} catch (const std::exception &p_except) {
		TopBar::Error(p_except.what());

		return;
	}

	if (not config.Contains(INI::GlobalSection, "theme"))
		config[INI::GlobalSection]["theme"] = g_config.theme;

	if (not config.Contains("bools", "ruler"))
		config["bools"]["ruler"] = Utils::ToString(g_config.ruler);

	if (not config.Contains("bools", "trim_ws_on_save"))
		config["bools"]["trim_ws_on_save"] = Utils::ToString(g_config.trimOnSave);

	if (not config.Contains("bools", "scroll_bar"))
		config["bools"]["scroll_bar"] = Utils::ToString(g_config.scrollBar);

	if (not config.Contains("editor", "indent_size"))
		config["editor"]["indent_size"] = std::to_string(g_config.indentSize);

	if (not config.Contains("editor", "marked_column"))
		config["editor"]["marked_column"] = std::to_string(g_config.markedColumn);

	try {
		g_config.theme        = config.AsString(INI::GlobalSection, "theme");
		g_config.ruler        = config.AsBool("bools", "ruler");
		g_config.trimOnSave   = config.AsBool("bools", "trim_ws_on_save");
		g_config.scrollBar    = config.AsBool("bools", "scroll_bar");
		g_config.indentSize   = config.AsInteger("editor", "indent_size");
		g_config.markedColumn = config.AsInteger("editor", "marked_column");
	} catch (const std::exception &p_except) {
		TopBar::Error(p_except.what());

		return;
	}

	UpdateTheme();
}

void WriteConfigFile() {
	// construct the config ini structure
	INI::Struct config({
		{
			INI::GlobalSection, {
				{"theme", g_config.theme}
			}
		},
		{
			"bools", {
				{"ruler",           Utils::ToString(g_config.ruler)},
				{"trim_ws_on_save", Utils::ToString(g_config.trimOnSave)},
				{"scroll_bar",      Utils::ToString(g_config.scrollBar)}
			}
		},
		{
			"editor", {
				{"indent_size",   std::to_string(g_config.indentSize)},
				{"marked_column", std::to_string(g_config.markedColumn)}
			}
		}
	});

	Utils::WriteFile(ConfigPath()/"config.ini", config.Stringify());
}

void UpdateTheme() {
	if (not Utils::PathExists(ConfigPath()/"themes"/std::string(g_config.theme + ".ini"))) {
		TopBar::Error("Theme file ~/.config/pona/themes/" + g_config.theme + ".ini not found");

		g_config.theme = "default";

		return;
	}

	std::string buffer;
	Utils::ReadFile(ConfigPath()/"themes"/std::string(g_config.theme + ".ini"), buffer);

	INI::Struct theme;

	try {
		theme.Parse(buffer);
	} catch (const std::exception &p_except) {
		TopBar::Error(std::string(p_except.what()) + " in theme " + g_config.theme);

		return;
	}

	for (const auto &[section, key, colorPair] : g_keyToColorPairMap) {
		std::string keyFg = key == ""? "fg" : key + "_fg";
		std::string keyBg = key == ""? "bg" : key + "_bg";

		if (not theme.Contains(section, keyFg)) {
			TopBar::Error(section + "::" + keyFg + " not found in theme " + g_config.theme);

			return;
		} else if (not theme.Contains(section, keyBg)) {
			TopBar::Error(section + "::" + keyBg + " not found in theme " + g_config.theme);

			return;
		}

		std::string fgColorString = theme.At(section, keyFg);
		std::string bgColorString = theme.At(section, keyBg);

		try {
			init_pair(colorPair, StringToColor(fgColorString), StringToColor(bgColorString));
		} catch (const std::exception &p_except) {
			TopBar::Error(std::string(p_except.what()) + ", from theme " + g_config.theme);

			return;
		}
	}
}

std::fs::path ConfigPath() {
	return Utils::Home()/".config/pona/";
}

void Editors::Input(NC::input_t p_input) {
	switch (p_input) {
	case NC::Key::Ctrl('e'): // open the command line
		TopBar::g_state = TopBar::State::Input;
		TopBar::g_input = "";
		TopBar::g_historyPos = -1;

		break;

	case NC::Key::Alt('.'): // next tab
		if (Editors::g_list.size() > 1) {
			if (Editors::g_currentIdx < Editors::g_list.size() - 1)
				++ Editors::g_currentIdx;
			else
				Editors::g_currentIdx = 0;
		}

		break;

	case NC::Key::Alt(','): // previous tab
		if (Editors::g_list.size() > 1) {
			if (Editors::g_currentIdx > 0)
				-- Editors::g_currentIdx;
			else
				Editors::g_currentIdx = Editors::g_list.size() - 1;
		}

		break;

	default: // else check if a command shortcut was pressed
		bool found = false;
		for (const auto &[name, info] : Commands::g_list) {
			if (p_input == NC::Key::Ctrl(info.second)) {
				found = true;

				if (info.first) { // if the command needs params
					// open the command line and put the command there
					TopBar::g_state   = TopBar::State::Input;
					TopBar::g_input   = name + ' ';
					TopBar::g_cursorX = TopBar::g_input.length();

					TopBar::g_tokens = Commands::Parse(TopBar::g_input);
				} else { // else just run the command
					TopBar::g_tokens = {name};
					Commands::Run(TopBar::g_tokens);
				}

				break;
			}
		}

		if (not found) // if no command was pressed, give the input to the editor
			Editors::g_list.at(Editors::g_currentIdx).Input(p_input);

		break;
	}
}

Editor Editors::Construct(
	const std::string &p_title, const std::vector<std::string> &p_contents, bool p_readOnly
) {
	Editor editor(Vec2D(g_winSize.x, g_winSize.y - 1), Vec2D(0, 1), p_title, p_contents);
	UpdateEditorConfig(editor);
	editor.SetReadOnly(p_readOnly);

	return editor;
}

void Editors::UpdateEditorConfig(Editor &p_editor) {
	p_editor.EnableRuler(g_config.ruler);
	p_editor.EnableScrollBar(g_config.scrollBar);
	p_editor.SetIndentSize(g_config.indentSize);
	p_editor.SetMarkedColumn(g_config.markedColumn);
}

void Editors::UpdateAllEditorConfigs() {
	for (auto &editor : Editors::g_list)
		UpdateEditorConfig(editor);
}

