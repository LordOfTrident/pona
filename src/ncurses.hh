#ifndef NCURSESW_HH__HEADER_GUARD__
#define NCURSESW_HH__HEADER_GUARD__

#include <cstdint>       // std::uint64_t, std::uint8_t
#include <vector>        // std::vector
#include <unordered_map> // std::unordered_map
#include <ncurses.h>     // terminal i/o

#ifndef NCURSES_WIDECHAR
#	pragma message "ncursesw is required but not available"
#endif // NCURSES_WIDECHAR

namespace NC {
	namespace Color {
		enum Color : short {
			Default = -1,

			Black   = COLOR_BLACK,
			Red     = COLOR_RED,
			Green   = COLOR_GREEN,
			Yellow  = COLOR_YELLOW,
			Blue    = COLOR_BLUE,
			Magenta = COLOR_MAGENTA,
			Cyan    = COLOR_CYAN,
			White   = COLOR_WHITE,

			Grey,
			BrightRed,
			BrightGreen,
			BrightYellow,
			BrightBlue,
			BrightMagenta,
			BrightCyan,
			BrightWhite,

			Count
		}; // enum Color
	}

	struct RGB {
		RGB(std::uint8_t p_r = 0, std::uint8_t p_g = 0, std::uint8_t p_b = 0);

		std::uint8_t r, g, b;
	};

	using input_t = std::uint64_t;

	extern std::unordered_map<WINDOW*, int> g_lastColorPair;

	input_t Input();

	int  WAddWCh(WINDOW *p_win, wchar_t p_wCh);
	int  MoveWAddWCh(WINDOW *p_win, int p_y, int p_x, wchar_t p_wCh);
	void WSetColor(WINDOW *p_win, int p_color);

	namespace Key {
		enum Key : input_t {
			Tab    = 9,
			Enter  = 10,
			Escape = 27,
			Backspace = 127,

			Up    = KEY_UP,
			Down  = KEY_DOWN,
			Right = KEY_RIGHT,
			Left  = KEY_LEFT,

			NextPage = KEY_NPAGE,
			PrevPage = KEY_PPAGE,

			Resize = KEY_RESIZE,
			Mouse  = KEY_MOUSE
		}; // enum Key


		constexpr input_t Ctrl(input_t p_key) {
			switch (p_key) {
			case Up:    return static_cast<input_t>(-2);
			case Down:  return static_cast<input_t>(-3);
			case Right: return static_cast<input_t>(-4);
			case Left:  return static_cast<input_t>(-5);
			default:    return p_key & 31;
			}
		}

		constexpr input_t Alt(input_t p_key) {
			return static_cast<input_t>(-9) - p_key;
		}

		constexpr input_t Shift(input_t p_key) {
			switch (p_key) {
			case Up:    return 337;
			case Down:  return 336;
			case Left:  return 393;
			case Right: return 402;

			case Ctrl(Up):    return static_cast<input_t>(-6);
			case Ctrl(Down):  return static_cast<input_t>(-7);
			case Ctrl(Left):  return static_cast<input_t>(-8);
			case Ctrl(Right): return static_cast<input_t>(-9);
			default: return p_key - 33;
			}
		}
	}

	namespace ColorPair {
		enum ColorPair : short {
			Default = 0,

			TopBar,
			TopBar_Input,
			TopBar_Error,
			TopBar_Tab,
			TopBar_TabCurrent,

			TopBar_List,
			TopBar_ListCurrent,

			Editor,
			Editor_Current,
			Editor_Ruler,
			Editor_RulerCurrent,
			Editor_ScrollBar,
			Editor_ScrollButton,
			Editor_ScrollButtonAlt,
			Editor_ScrollBack,
			Editor_InfoBar,
			Editor_InfoBarSeparator,
			Editor_MarkedColumn,
			Editor_Selection,

			Count
		}; // enum Color
	} // namespace Color
} // namespace NC

#endif
