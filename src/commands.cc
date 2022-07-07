#include "app.hh"

// ONLY space (' ') means no shortcut, dont use anything else for that
const std::map<std::string, std::pair<bool, char>> Commands::g_list = {
//   command name       needs arguments?      shortcut
	{"help",            std::make_pair(false, 'g')},
	{"defaults",        std::make_pair(false, ' ')},
	{"mark_column",     std::make_pair(true,  ' ')},
	{"indent_size",     std::make_pair(true,  ' ')},
	{"exit",            std::make_pair(false, ' ')},
	{"new",             std::make_pair(false, 't')},
	{"close",           std::make_pair(false, 'q')},
	{"save",            std::make_pair(false, 's')},
	{"save_as",         std::make_pair(true,  ' ')},
	{"save_config",     std::make_pair(false, ' ')},
	{"scroll_bar",      std::make_pair(true,  ' ')},
	{"open",            std::make_pair(true,  'o')},
	{"goto",            std::make_pair(true,  'l')},
	{"find",            std::make_pair(true,  'f')},
	{"find_next",       std::make_pair(false, ' ')},
	{"reload_config",   std::make_pair(false, ' ')},
	{"replace",         std::make_pair(true,  'r')},
	{"replace_next",    std::make_pair(false, ' ')},
	{"replace_all",     std::make_pair(true,  ' ')},
	{"ruler",           std::make_pair(true,  ' ')},
	{"theme",           std::make_pair(true,  ' ')},
	{"trim",            std::make_pair(true,  ' ')},
	{"trim_ws_on_save", std::make_pair(true,  ' ')}
};

std::vector<std::string> Commands::Parse(std::string p_cmd) {
	Utils::TrimStringLeft(p_cmd);

	bool inStr = false;
	std::string token;
	std::vector<std::string> tokens;

	// simple command parsing
	for (auto ch : p_cmd) {
		switch (ch) {
		case ' ':
			if (inStr)
				token += ch;
			else { // put empty tokens in the vector too
				tokens.push_back(token);
				token = "";
			}

			break;

		case '"': inStr  = not inStr; break;
		default:  token += ch; break;
		}
	}

	if (token.length() != 0)
		tokens.push_back(token);

	return tokens;
}

void Commands::Run(std::vector<std::string> p_tokens) {
	if (p_tokens.size() == 0)
		return;

	// remove empty tokens from the vector
	for (std::size_t i = 0; i < p_tokens.size(); ++ i) {
		if (p_tokens[i].length() == 0) {
			p_tokens.erase(p_tokens.begin() + i);
			-- i;
		}
	}

	std::string cmd = p_tokens.at(0);
	// remove the command name from the tokens, so we only get the arguments
	p_tokens.erase(p_tokens.begin());

	bool found = false;
	for (const auto &[name, info] : g_list) {
		if (name == cmd) {
			found = true;

			break;
		}
	}
	if (not found) {
		TopBar::Error("Unknown command '" + cmd + "'");

		return;
	}

	TopBar::Reset();

	if (cmd == "help") { // command help
		if (p_tokens.size() != 0) {
			TopBar::Error("'help' expects 0 arguments");

			return;
		}

		Editors::g_list.push_back(Editors::Construct("Help.md", DEFAULT_HELP, true));
		Editors::g_currentIdx = Editors::g_list.size() - 1;
	} else if (cmd == "ruler") { // command ruler
		if (p_tokens.size() != 1) {
			TopBar::Error("'ruler' expects 1 argument");

			return;
		}

		if (p_tokens.at(0) == "1")
			g_config.ruler = true;
		else if (p_tokens.at(0) == "0")
			g_config.ruler = false;
		else {
			TopBar::Error("'ruler' argument 1 has to be bool (1/0)");

			return;
		}

		Editors::UpdateAllEditorConfigs();
	} else if (cmd == "reload_config") { // command reload_config
		if (p_tokens.size() != 0) {
			TopBar::Error("'reload_config' expects 0 arguments");

			return;
		}

		ReadOrFixConfig();
		Editors::UpdateAllEditorConfigs();
	} else if (cmd == "save_config") { // command save_config
		if (p_tokens.size() != 0) {
			TopBar::Error("'save_config' expects 0 arguments");

			return;
		}

		WriteConfigFile();
	} else if (cmd == "defaults") { // command defaults
		if (p_tokens.size() != 0) {
			TopBar::Error("'defaults' expects 0 arguments");

			return;
		}

		Defaults();
		Editors::UpdateAllEditorConfigs();
	} else if (cmd == "theme") { // command theme
		if (p_tokens.size() != 1) {
			TopBar::Error("'theme' expects 1 argument");

			return;
		}

		g_config.theme = p_tokens.at(0);
		UpdateTheme();
		Editors::UpdateAllEditorConfigs();
	} else if (cmd == "scroll_bar") { // command scroll_bar
		if (p_tokens.size() != 1) {
			TopBar::Error("'scroll_bar' expects 1 argument");

			return;
		}

		if (p_tokens.at(0) == "1")
			g_config.scrollBar = true;
		else if (p_tokens.at(0) == "0")
			g_config.scrollBar = false;
		else {
			TopBar::Error("'scroll_bar' argument 1 has to be bool (1/0)");

			return;
		}

		Editors::UpdateAllEditorConfigs();
	} else if (cmd == "mark_column") { // command mark_column
		if (p_tokens.size() != 1) {
			TopBar::Error("'mark_column' expects 1 argument");

			return;
		}

		try {
			g_config.markedColumn = std::stoi(p_tokens.at(0));
		} catch(...) {
			TopBar::Error("'mark_column' argument 1 has to be int");

			return;
		}

		Editors::UpdateAllEditorConfigs();
	} else if (cmd == "indent_size") { // command indent_size
		if (p_tokens.size() != 1) {
			TopBar::Error("'indent_size' expects 1 argument");

			return;
		}

		try {
			g_config.indentSize = std::stoi(p_tokens.at(0));
		} catch(...) {
			TopBar::Error("'indent_size' argument 1 has to be int");

			return;
		}

		Editors::UpdateAllEditorConfigs();
	} else if (cmd == "exit") { // command exit
		if (p_tokens.size() != 0) {
			TopBar::Error("'exit' expects 0 arguments");

			return;
		}

		if (TopBar::g_answer) {
			g_quit = true;

			return;
		}

		bool unsaved = false;
		for (const auto &editor : Editors::g_list) {
			if (editor.buffer.IsModified()) {
				unsaved = true;

				break;
			}
		}

		if (unsaved) {
			TopBar::Ask("Quit? Unsaved changes will be lost");

			return;
		} else
			g_quit = true;
	} else if (cmd == "new") { // command new
		if (p_tokens.size() == 0)
			Editors::g_list.push_back(Editors::Construct("Untitled"));
		else {
			for (const auto &title : p_tokens)
				Editors::g_list.push_back(Editors::Construct(title));
		}

		Editors::g_currentIdx = Editors::g_list.size() - 1;
	} else if (cmd == "close") { // command close
		if (p_tokens.size() != 0) {
			TopBar::Error("'close' expects 0 arguments");

			return;
		}

		if (TopBar::g_state == TopBar::State::Input or TopBar::g_state == TopBar::State::Question)
			TopBar::Reset();
		else if (Editors::g_list.size() > 1) {
			if (not TopBar::g_answer and Editors::g_list.at(Editors::g_currentIdx).buffer.IsModified()) {
				TopBar::Ask("Close tab? Unsaved changes will be lost");

				return;
			}

			Editors::g_list.erase(Editors::g_list.begin() + Editors::g_currentIdx);

			if (Editors::g_currentIdx == Editors::g_list.size())
				-- Editors::g_currentIdx;
		} else {
			if (not TopBar::g_answer and Editors::g_list.at(0).buffer.IsModified()) {
				TopBar::Ask("Quit? Unsaved changes will be lost");

				return;
			}

			g_quit = true;
		}
	} else if (cmd == "save") { // command save
		if (p_tokens.size() != 0) {
			TopBar::Error("'save' expects 0 arguments");

			return;
		}

		Editor &editor = Editors::g_list.at(Editors::g_currentIdx);

		std::string path = editor.GetTitle();
		Utils::TrimString(path);

		if (path.length() == 0) {
			TopBar::Error("'save' incorrect file name");

			return;
		}

		if (g_config.trimOnSave) {
			for (auto &line : Editors::g_list.at(Editors::g_currentIdx).buffer.rawBuffer) {
				Utils::TrimStringRight(line);
				Utils::TrimStringRight(line, '\t');
			}

			editor.Update();
		}

		Utils::WriteFile(path, editor.buffer.rawBuffer);

		editor.buffer.FlagUnmodified();
	} else if (cmd == "save_as") { // command save_as
		if (p_tokens.size() != 1) {
			TopBar::Error("'save_as' expects 1 argument");

			return;
		}

		std::string path = p_tokens.at(0);
		Utils::TrimString(path);

		if (path.length() == 0) {
			TopBar::Error("'save_as' incorrect file name");

			return;
		}

		Editor &editor = Editors::g_list.at(Editors::g_currentIdx);

		if (g_config.trimOnSave) {
			for (auto &line : Editors::g_list.at(Editors::g_currentIdx).buffer.rawBuffer) {
				Utils::TrimStringRight(line);
				Utils::TrimStringRight(line, '\t');
			}

			editor.Update();
		}

		Utils::WriteFile(path, Editors::g_list.at(Editors::g_currentIdx).buffer.rawBuffer);

		editor.SetTitle(path);
		editor.buffer.FlagUnmodified();
	} else if (cmd == "open") { // command open
		if (p_tokens.size() < 1) {
			TopBar::Error("'open' expects at least 1 argument");

			return;
		}

		bool newTab = false;
		for (auto path : p_tokens) {
			Utils::UnbloatPath(path);

			if (path.length() == 0) {
				TopBar::Error("'open' incorrect file name");

				return;
			}

			if (Utils::PathExists(path)) {
				if (newTab) {
					Editors::g_list.push_back(Editors::Construct("Untitled"));
					Editors::g_currentIdx = Editors::g_list.size() - 1;
				} else
					newTab = true;

				Utils::ReadFile(path, Editors::g_list[Editors::g_currentIdx].buffer.rawBuffer);

				Editor &editor = Editors::g_list.at(Editors::g_currentIdx);

				editor.buffer.FlagUnmodified();
				editor.SetTitle(path);
				editor.buffer.SetCursor(Vec2Dw(0, 0));
			} else {
				TopBar::Error("'open' file '" + path + "' does not exist");

				return;
			}
		}
	} else if (cmd == "trim_ws_on_save") { // command trim_ws_on_save
		if (p_tokens.size() != 1) {
			TopBar::Error("'trim_ws_on_save' expects 1 argument");

			return;
		}

		if (p_tokens.at(0) == "1")
			g_config.trimOnSave = true;
		else if (p_tokens.at(0) == "0")
			g_config.trimOnSave = false;
		else {
			TopBar::Error("'trim_ws_on_save' argument 1 has to be bool (1/0)");

			return;
		}
	} else if (cmd == "goto") { // command goto
		if (p_tokens.size() != 1) {
			TopBar::Error("'goto' expects 1 argument");

			return;
		}

		try {
			std::size_t line    = std::stoi(p_tokens.at(0));
			Editor      &editor = Editors::g_list.at(Editors::g_currentIdx);

			if (line >= editor.buffer.Size())
				line = editor.buffer.Size() - 1;
			if (line == 0)
				line = 1;

			-- line;

			editor.buffer.SetCursor(Vec2Dw(0, line));
			editor.Update();
		} catch(...) {
			TopBar::Error("'goto' argument 1 has to be int");

			return;
		}

		Editors::UpdateAllEditorConfigs();
	} else if (cmd == "find") { // command find
		if (p_tokens.size() != 1) {
			TopBar::Error("'find' expects 1 argument");

			return;
		}

		std::size_t posY = 0;
		Editor &editor = Editors::g_list.at(Editors::g_currentIdx);

		for (const auto &line : editor.buffer.rawBuffer) {
			std::size_t pos = line.find(p_tokens.at(0));
			if (pos != std::string::npos) {
				editor.buffer.SetCursor(Vec2Dw(pos, posY));
				editor.buffer.MarkSelection();
				editor.buffer.SetCursor(Vec2Dw(pos + p_tokens.at(0).length(), posY));
				editor.Update();

				break;
			}

			++ posY;
		}
	} else if (cmd == "replace") { // command replace
		if (p_tokens.size() != 2) {
			TopBar::Error("'replace' expects 2 arguments");

			return;
		}

		std::size_t posY = 0;
		Editor &editor = Editors::g_list.at(Editors::g_currentIdx);

		for (const auto &line : editor.buffer.rawBuffer) {
			std::size_t pos = line.find(p_tokens.at(0));
			if (pos != std::string::npos) {
				editor.buffer.SetCursor(Vec2Dw(pos, posY));
				editor.buffer.MarkSelection();
				editor.buffer.SetCursorX(pos + p_tokens.at(0).length());

				editor.buffer.CursorDelete();
				editor.buffer.CursorLine().insert(editor.buffer.GetCursor().x, p_tokens.at(1));
				editor.buffer.SetCursorX(pos + p_tokens.at(1).length());
				editor.Update();

				break;
			}

			++ posY;
		}
	} else // TODO: implement all commands
		TopBar::Error("Command '" + cmd + "' not implemented");
}
