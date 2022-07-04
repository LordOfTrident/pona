#include "app.hh"

WINDOW *TopBar::g_win = nullptr, *TopBar::g_matchListWin = nullptr;

TopBar::State TopBar::g_state = TopBar::State::None;

std::string TopBar::g_input;
bool        TopBar::g_answer = false;

std::size_t TopBar::g_cursorX = 0, TopBar::g_scrollX = 0, TopBar::g_length;

std::vector<std::string> TopBar::g_history;
std::size_t TopBar::g_historyPos = -1;

std::vector<std::string> TopBar::g_matches, TopBar::g_tokens;
std::size_t TopBar::g_currentTokenIdx;

void TopBar::Init() {
	g_win          = newwin(1, g_winSize.x, 0, 0);
	g_matchListWin = newwin(1, 1, 1, 3);

	g_length = g_winSize.x - 6;
}

void TopBar::Finish() {
	delwin(g_win);
	delwin(g_matchListWin);
}

void TopBar::Input(NC::input_t p_input) {
	switch (g_state) {
	case State::Input:    CommandInput(p_input);  break;
	case State::Question: QuestionInput(p_input); break;
	default: break;
	}
}

void TopBar::CommandInput(NC::input_t p_input) {
	switch (p_input) {
	case NC::Key::Enter:
		g_state   = State::None;
		g_cursorX = 0;
		g_scrollX = 0;

		if (g_input.length() != 0) {
			// if there is any input, save it to the history and run it
			g_history.push_back(g_input);

			g_input   = "";
			g_matches = {};

			Commands::Run(g_tokens);
		}

		break;

	case NC::Key::Ctrl('q'): Reset(); break;

	case NC::Key::Right: // cursor right/left movement
		if (g_cursorX < g_input.length())
			++ g_cursorX;

		break;

	case NC::Key::Left:
		if (g_cursorX > 0)
			-- g_cursorX;

		break;

	case NC::Key::Up: // history movement
		if (g_history.size() == 0)
			break;

		if (g_historyPos == static_cast<std::size_t>(-1)) { // -1 means it is not in the history
			g_historyPos = g_history.size() - 1;

			SetInput(g_history.at(g_historyPos));
		} else if (g_historyPos > 0) {
			-- g_historyPos;

			SetInput(g_history.at(g_historyPos));
		}

		break;

	case NC::Key::Down:
		if (g_history.size() == 0)
			break;

		if (g_historyPos == g_history.size() - 1) {
			g_historyPos = -1;

			SetInput("");
		} else if (g_historyPos != static_cast<std::size_t>(-1)) {
			++ g_historyPos;

			SetInput(g_history.at(g_historyPos));
		}

		break;

	case NC::Key::Tab: // use the top command match
		if (g_matches.size() != 0) {
			// we dont want a range error
			if (g_currentTokenIdx == g_tokens.size())
				g_tokens.push_back(g_matches.at(0));
			else
				g_tokens.at(g_currentTokenIdx) = g_matches.at(0);

			g_input   = "";
			g_cursorX = 0;

			// construct the new command input string
			bool start = true;
			std::size_t idx = 0;
			for (const auto &token : g_tokens) {
				if (token.length() == 0) {
					-- g_currentTokenIdx;

					continue;
				}

				if (not start)
					g_input += ' ';
				else
					start = false;

				g_input += token;

				// move the cursor appropriately
				if (g_currentTokenIdx >= idx) {
					// add 1 because of the space between tokens
					g_cursorX += token.length() + 1;

					// we dont want the cursor to be offset
					// so we subtract 1 if we are at the final cursor position
					if (g_currentTokenIdx == idx)
						-- g_cursorX;
				}

				++ idx;
			}
		}

		break;

	case NC::Key::Backspace:
		if (g_cursorX > 0) {
			g_input.erase(g_cursorX - 1, 1);
			-- g_cursorX;
		}

		break;

	default:
		if (p_input >= 32 and p_input <= 127) {
			g_input.insert(g_cursorX, std::string(1, p_input));
			++ g_cursorX;
		}

		break;
	}
}

void TopBar::QuestionInput(NC::input_t p_input) {
	switch (p_input) {
	case 'y': case 'Y':
		g_state  = State::None;
		g_answer = true;
		g_input  = "";

		// run the command which asked this
		Commands::Run(g_tokens);

		g_answer = false;

		break;

	case 'n': case 'N':
		g_state  = State::None;
		g_answer = false;
		g_input  = "";

	default: break;
	}
}



void TopBar::Render() {
	wbkgdset(g_win, COLOR_PAIR(NC::ColorPair::TopBar));
	werase(g_win);

	switch (g_state) {
	case State::Input: RenderInputBar(); break;
	case State::Question:
	case State::Error: RenderMessage(); break;
	default: RenderTabs(); break;
	}

	wrefresh(g_win);
}

void TopBar::RenderInputBar() {
	wattron(g_win, A_BOLD);
	mvwaddch(g_win, 0, 1, '>');
	wattroff(g_win, A_BOLD);

	NC::WSetColor(g_win, NC::ColorPair::TopBar_Input);

	mvwaddch(g_win, 0, 3, ' ');
	for (std::size_t i = 0; i < g_length; ++ i) {
		if (g_scrollX + i < g_input.length())
			waddch(g_win, g_input.at(g_scrollX + i));
		else
			waddch(g_win, ' ');
	}
	waddch(g_win, ' ');

	RenderMatchesList();
}

void TopBar::RenderMessage() {
	wattron(g_win, A_BOLD);
	mvwaddch(g_win, 0, 1, g_state == State::Error? '!' : '?');
	wattroff(g_win, A_BOLD);

	NC::WSetColor(g_win, g_state == State::Error? NC::ColorPair::TopBar_Error :
	              NC::ColorPair::TopBar_Input);

	// if it is a question, add (y/n) to the text
	std::string text = g_input + (g_state == State::Question? " (y/n)" : "");

	mvwaddch(g_win, 0, 3, ' ');
	for (std::size_t i = 0; i < g_length; ++ i) {
		if (i < text.length())
			waddch(g_win, text[i]);
		else
			waddch(g_win, ' ');
	}
	waddch(g_win, ' ');
}

void TopBar::RenderTabs() {
	wmove(g_win, 0, 1);

	NC::WSetColor(g_win, NC::ColorPair::TopBar_Tab);

	for (const auto &editor : Editors::g_list) {
		bool isSelected = &Editors::g_list.at(Editors::g_currentIdx) == &editor;
		if (isSelected)
			NC::WSetColor(g_win, NC::ColorPair::TopBar_TabCurrent);

		waddch(g_win, ' ');

		if (editor.buffer.IsModified()) {
			wattron(g_win, A_BOLD);
			waddch(g_win, '*');
			wattroff(g_win, A_BOLD);
		}

		waddstr(g_win, editor.GetTitle().c_str());
		waddch(g_win, ' ');

		if (isSelected)
			NC::WSetColor(g_win, NC::ColorPair::TopBar_Tab);

		waddch(g_win, ' ');
	}
}

void TopBar::RenderMatchesList() {
	// draw the matches list if it is not empty
	if (g_matches.size() != 0) {
		// find the list width
		std::size_t longest = 0;
		bool hasShortcut = false;
		for (const auto &match : g_matches) {
			if (match.length() > longest)
				longest = match.length();

			if (Commands::g_list.count(match)) {
				if (not hasShortcut and Commands::g_list.at(match).second != ' ')
					hasShortcut = true;
			}
		}

		if (hasShortcut)
			longest += 3;

		wresize(g_matchListWin, g_matches.size(), longest + 2);

		wbkgdset(g_matchListWin, COLOR_PAIR(NC::ColorPair::TopBar_List));
		werase(g_matchListWin);

		// draw the list
		int posY = 0;
		for (const auto &match : g_matches) {
			if (posY == 0) { // first (selected) match
				NC::WSetColor(g_matchListWin, NC::ColorPair::TopBar_ListCurrent);
				mvwhline(g_matchListWin, posY, 0, ' ', longest + 2);
			} else
				NC::WSetColor(g_matchListWin, NC::ColorPair::TopBar_List);

			mvwaddstr(g_matchListWin, posY, 1, match.c_str());

			if (Commands::g_list.count(match)) {
				if (hasShortcut and Commands::g_list.at(match).second != ' ') {
					wattron(g_matchListWin, A_BOLD);
					mvwaddch(g_matchListWin, posY, longest - 1, '^');
					waddch(g_matchListWin, std::toupper(Commands::g_list.at(match).second));
					wattroff(g_matchListWin, A_BOLD);
				}
			}

			++ posY;
		}

		wrefresh(g_matchListWin);
	}
}

void TopBar::Update() {
	if (g_state == State::Input) {
		// fix horizontal scroll
		if (g_cursorX < g_scrollX)
			g_scrollX = g_cursorX;
		else if (g_cursorX - g_scrollX >= g_length)
			g_scrollX = g_cursorX - g_length;

		g_tokens = Commands::Parse(g_input);

		// construct the matches list
		g_matches = {};
		if (g_tokens.size() != 0) {
			// offset the list position by the amount of spaces
			// in front of the first command
			std::size_t pos = g_input.find_first_not_of(' ');
			// if we are at the first token, we obviously want to find matching COMMANDS
			// all the other tokens are command ARGUMENTS
			if (g_cursorX <= pos + g_tokens.at(0).length() and g_tokens.at(0) != "")
				GetMatchingCommands();
			else if (g_tokens.at(0) == "theme") // if the command is theme, ...
				GetMatchingThemes();
		}
	}
}

void TopBar::GetMatchingCommands() {
	// if we are trying to find matching commands, we are obviously at
	// the first token which is the command name itself
	g_currentTokenIdx = 0;

	// again, we offset the list position by the amount of spaces
	// in front of the command
	std::size_t pos = g_input.find_first_not_of(' ');
	if (static_cast<std::size_t>(getbegx(g_matchListWin)) != 3 + pos - g_scrollX)
		mvwin(g_matchListWin, 1, 3 + pos - g_scrollX);

	const std::string &token = g_tokens.at(0);

	// find matches
	for (const auto &[name, shortcut] : Commands::g_list) {
		if (Utils::StringStartsWith(name, token) and name != token)
			g_matches.push_back(name);
	}
}

void TopBar::GetMatchingThemes() {
	// this function returns the position of the start of the current token in the command line
	std::size_t pos = CalcCurrentToken();

	mvwin(g_matchListWin, 1, 3 + pos - g_scrollX);

	// avoid range errors
	std::string token;
	if (g_currentTokenIdx < g_tokens.size())
		token = g_tokens.at(g_currentTokenIdx);

	// loop through the themes directory
	for (const auto& entry : std::fs::directory_iterator(ConfigPath()/"themes")) {
		if (entry.is_directory())
			continue;

		// strip the file path so we only get the plain file name
		std::string name = Utils::PathToFileName(entry.path().u8string());

		// its a theme file only if it ends with .ini
		if (Utils::StringEndsWith(name, ".ini")) {
			Utils::RemoveSuffix(name, ".ini");

			if (Utils::StringStartsWith(name, token) and name != token)
				g_matches.push_back(name);
		}
	}
}

std::size_t TopBar::CalcCurrentToken() {
	g_currentTokenIdx = 0;

	std::size_t posX = 0, pos = g_input.find_first_not_of(' ');
	for (const auto &token : g_tokens) {
		// if we went beyond the cursor position, break out
		// without applying the calculation
		if (pos + posX + token.length() + 1 > g_cursorX)
			break;

		posX += token.length() + 1;
		++ g_currentTokenIdx;
	}

	return pos + posX;
}

void TopBar::ResizeWindow() {
	wresize(g_win, 1, g_winSize.x);

	g_length = g_winSize.x - 6;
}

void TopBar::SetInput(const std::string &p_input) {
	g_input   = p_input;
	g_cursorX = g_input.length();
}

void TopBar::Ask(const std::string &p_question) {
	g_state = State::Question;
	g_input = p_question;
}

void TopBar::Error(const std::string &p_msg) {
	g_state = State::Error;
	g_input = p_msg;
}

void TopBar::Reset() {
	g_input   = "";
	g_state   = State::None;
	g_cursorX = 0;
	g_scrollX = 0;
	g_matches = {};
	g_historyPos = -1;
}

void TopBar::PosCursor() {
	move(0, g_cursorX - g_scrollX + 4);
}
