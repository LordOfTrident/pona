#include "ncurses.hh"

std::unordered_map<WINDOW*, int> NC::g_lastColorPair;

NC::RGB::RGB(std::uint8_t p_r, std::uint8_t p_g, std::uint8_t p_b):
	r(p_r),
	g(p_g),
	b(p_b)
{}

NC::input_t NC::Input() {
	int     rawInput = getch();
	input_t input    = rawInput;

	// enable nodelay so the input sequence is thrown at us
	// and we know where it ends
	nodelay(stdscr, true);

	std::vector<int> inputSequence;
	do {
		inputSequence.push_back(rawInput);

		rawInput = getch();
	} while (rawInput != ERR);

	nodelay(stdscr, false);

	// parse the escape sequence
	if (inputSequence.size() == 6 and inputSequence.at(0) == 27) {
		if (
			inputSequence.at(1) == '[' and inputSequence.at(2) == '1' and
			inputSequence.at(3) == ';'
		) {
			switch (inputSequence.at(4)) {
			case '5':
				// CTRL + arrow key sequence, tested on ST
				switch (inputSequence.at(5)) {
				case 'A': input = Key::Ctrl(Key::Up);    break;
				case 'B': input = Key::Ctrl(Key::Down);  break;
				case 'C': input = Key::Ctrl(Key::Right); break;
				case 'D': input = Key::Ctrl(Key::Left);  break;
				default: break;
				}

				break;

			case '6':
				// CTRL + SHIFT + arrow key sequence, tested on ST
				switch (inputSequence.at(5)) {
				case 'A': input = Key::Shift(Key::Ctrl(Key::Up));    break;
				case 'B': input = Key::Shift(Key::Ctrl(Key::Down));  break;
				case 'C': input = Key::Shift(Key::Ctrl(Key::Right)); break;
				case 'D': input = Key::Shift(Key::Ctrl(Key::Left));  break;
				default: break;
				}

				break;

			default: break;
			}
		}
	} else if (inputSequence.size() == 2) {
		// alt + key sequences
		switch (inputSequence.at(0)) {
		// tested on st
		case 27:  input = Key::Alt(inputSequence.at(1));       break;
		// these two tested on xterm, xfce and gnome terminals
		// (symbols return 194 but the letters i tested return 195)
		case 194: input = Key::Alt(inputSequence.at(1) - 128); break;
		case 195: input = Key::Alt(inputSequence.at(1) - 96);  break;
		}
	} else {
		// CTRL + arrow key codes, tested on xterm, xfce and gnome terminals
		switch (input) {
		case 566: input = Key::Ctrl(Key::Up);    break;
		case 525: input = Key::Ctrl(Key::Down);  break;
		case 560: input = Key::Ctrl(Key::Right); break;
		case 545: input = Key::Ctrl(Key::Left);  break;

		// CTRL + SHIFT + arrow key codes, tested on xterm, xfce and gnome terminals
		case 567: input = Key::Shift(Key::Ctrl(Key::Up));    break;
		case 526: input = Key::Shift(Key::Ctrl(Key::Down));  break;
		case 561: input = Key::Shift(Key::Ctrl(Key::Right)); break;
		case 546: input = Key::Shift(Key::Ctrl(Key::Left));  break;

		// sometimes, ncurses gives KEY_ENTER (which is NOT 10), sometimes
		// it gives '\n' (10) (its just as weird for KEY_BACKSPACE)
		// so i just turn the weird ncurses macros to the normal 10 and 127 codes
		case KEY_ENTER:     input = Key::Enter;     break;
		case KEY_BACKSPACE: input = Key::Backspace; break;
		default: break;
		}
	}

	return input;
}

int NC::WAddWCh(WINDOW *p_win, wchar_t p_wCh) {
	// just to shorten code
	cchar_t cCh;
	wchar_t wChs[CCHARW_MAX] = {p_wCh, 0, 0, 0, 0};

	setcchar(&cCh, wChs, 0, 0, nullptr);
	return wadd_wch(p_win, &cCh);
}

int NC::MoveWAddWCh(WINDOW *p_win, int p_y, int p_x, wchar_t p_wCh) {
	wmove(p_win, p_y, p_x);
	return WAddWCh(p_win, p_wCh);
}

void NC::WSetColor(WINDOW *p_win, int p_color) {
	// make maintaining colors easier
	if (g_lastColorPair.count(p_win))
		wattroff(p_win, COLOR_PAIR(g_lastColorPair.at(p_win)));

	wattron(p_win, COLOR_PAIR(p_color));

	g_lastColorPair[p_win] = p_color;
}
