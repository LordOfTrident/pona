#ifndef EDITOR_HH__HEADER_GUARD__
#define EDITOR_HH__HEADER_GUARD__

#include <string>    // std::string
#include <vector>    // std::vector
#include <cmath>     // std::ceil
#include <cstdint>   // std::uint8_t
#include <algorithm> // std::count
#include <regex>     // std::regex_replace

#include "ncurses.hh"
#include "types.hh"
#include "utils.hh"
#include "buffer.hh"
#include "clipboard.hh"

class Editor {
public:
	Editor(
		const Vec2D &p_size, const Vec2D &p_pos, const std::string &p_title,
		const std::vector<std::string> &p_contents
	);

	const std::string &GetTitle() const;
	void SetTitle(const std::string &p_title);

	void Update();
	void ResizeWindow(const Vec2D &p_size);
	void ReposWindow(const Vec2D &p_pos);

	void PosCursor();

	void EnableRuler(bool p_ruler);
	void EnableScrollBar(bool p_scrollBar);
	void SetIndentSize(std::uint8_t p_indentSize);
	void SetMarkedColumn(std::size_t p_markedColumn);
	void SetSeparator(wchar_t p_separator);
	void SetReadOnly(bool p_readOnly);

	void ScrollUp();
	void ScrollDown();

	void CopySelection();
	void PasteSelection();

	bool IndentMoveLeft();
	bool IndentMoveRight();

	void Input(NC::input_t p_input);
	void Render();

	Buffer buffer;

private:
	void RenderInfoBar();
	void RenderScrollBar();
	void RenderContents();
	void RenderLine(Vec2Dw &p_pos, bool p_isCursorLine);
	void RenderBelowContents();

	WINDOW *m_win;

	std::string m_title;

	bool m_ruler, m_scrollBar, m_readOnly;
	std::size_t m_markedColumn;
	wchar_t     m_separator;

	bool m_scrolledUp, m_scrolledDown;
	std::size_t m_rulerWidth, m_maxLineLength, m_maxLines, m_cursorTabs;
	Vec2D  m_winSize, m_winPos;
	Vec2Dw m_scroll;
};

#endif
