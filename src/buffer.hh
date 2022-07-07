#ifndef BUFFER_HH__HEADER_GUARD__
#define BUFFER_HH__HEADER_GUARD__

#include <string>    // std::string
#include <vector>    // std::vector
#include <algorithm> // std::count

#include "utils.hh"
#include "types.hh"

class Buffer {
public:
	Buffer(const std::vector<std::string> &p_buffer, std::size_t p_sizeLimit = -1);

	char  At(const Vec2Dw &p_pos) const;
	char &At(const Vec2Dw &p_pos);

	std::size_t Size() const;

	const Vec2Dw &GetCursor() const;
	void SetCursor(const Vec2Dw &p_cursor);
	void SetCursorX(std::size_t p_x);
	void SetCursorY(std::size_t p_y);

	void CursorRight();
	void CursorLeft();
	void CursorUp();
	void CursorDown();

	void CursorWordRight();
	void CursorWordLeft();
	void CursorFullUp();
	void CursorFullDown();

	void CursorInsert(char p_ch);
	void CursorDelete();
	void CursorSplit();

	void UpdateCursorX();
	void UpdateCursorY();
	void UpdateCursor();

	void MarkSelection();
	void UnmarkSelection();
	const Vec2Dw &GetSelectionStart();
	const Vec2Dw &GetSelectionEnd();
	bool HasSelection();

	const std::string &CursorLine() const;
	std::string       &CursorLine();

	char  CursorCh() const;
	char &CursorCh();

	bool IsModified() const;
	void FlagUnmodified();

	std::uint8_t indentSize;

	std::vector<std::string> rawBuffer;

private:
	std::size_t CountLineTabs();

	bool   m_modified, m_selection;
	Vec2Dw m_cursor,   m_selectStart;
	std::size_t m_sizeLimit;

	std::size_t m_furthestCursorX;
}; // class Buffer

#endif
