#ifndef NCURSESW_HH__HEADER_GUARD__
#define NCURSESW_HH__HEADER_GUARD__

#include <cstdint>       // std::uint64_t
#include <vector>        // std::vector
#include <unordered_map> // std::unordered_map
#include <ncurses.h>     // terminal i/o

#ifndef NCURSES_WIDECHAR
#	pragma message "ncursesw is required but not available"
#endif // NCURSES_WIDECHAR

namespace NC {
	using input_t = std::uint64_t;

	extern std::unordered_map<WINDOW*, int> g_lastColorPair;

	input_t Input();

	int  WAddWCh(WINDOW *p_win, wchar_t p_wCh);
	int  MoveWAddWCh(WINDOW *p_win, int p_y, int p_x, wchar_t p_wCh);
	void WSetColor(WINDOW *p_win, int p_color);

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
			BrightWhite
		}; // enum Color
	}

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

		constexpr input_t Ctrl(int p_key) {
			if (p_key == Up)
				return static_cast<input_t>(-2);
			else if (p_key == Down)
				return static_cast<input_t>(-3);
			else if (p_key == Right)
				return static_cast<input_t>(-4);
			else if (p_key == Left)
				return static_cast<input_t>(-5);

			return p_key & 31;
		}

		constexpr input_t Alt(int p_key) {
			return static_cast<input_t>(-5) - p_key;
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

			ReservedEnd
		}; // enum Color
	} // namespace Color
} // namespace NC

#endif
