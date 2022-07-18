#include "buffer.hh"

Buffer::Buffer(const std::vector<std::string> &p_buffer, std::size_t p_sizeLimit):
	indentSize(4),

	rawBuffer(p_buffer),
	m_modified(false),
	m_selection(false),
	m_sizeLimit(p_sizeLimit),

	m_furthestCursorX(0)
{}

char Buffer::At(const Vec2Dw &p_pos) const {
	return rawBuffer.at(p_pos.y).at(p_pos.x);
}

char &Buffer::At(const Vec2Dw &p_pos) {
	return rawBuffer.at(p_pos.y).at(p_pos.x);
}

std::size_t Buffer::Size() const {
	return rawBuffer.size();
}

const Vec2Dw &Buffer::GetCursor() const {
	return m_cursor;
}

void Buffer::SetCursor(const Vec2Dw &p_cursor) {
	m_cursor = p_cursor;
	UpdateCursor();

	// we are keeping track of a m_furthestCursorX variable because of a subtle but really
	// important feature. Take this buffer
	//   1234567
	//   12
	//   1234|67
	// as an example, where the cursor is |. When you move the cursor up twice, it should
	// end up on the 3 right? like so:
	//   12|4567
	//   12
	//   1234567
	// because it jumps down on the line `12` and then goes up to the same X position. but in
	// most modern text editors, this does not happen. Instead, the cursor will be at where
	// it started, at the 5 like so:
	//   1234|67
	//   12
	//   1234567

	// this is the purpose of this variable; to keep track of that position
	m_furthestCursorX = m_cursor.x + CountLineTabs();
}

void Buffer::SetCursorX(std::size_t p_x) {
	m_cursor.x = p_x;
	UpdateCursorX();

	m_furthestCursorX = m_cursor.x + CountLineTabs();
}

void Buffer::SetCursorY(std::size_t p_y) {
	m_cursor.y = p_y;
	UpdateCursorY();
}

void Buffer::CursorRight() {
	if (m_cursor.x < CursorLine().length())
		++ m_cursor.x;
	else if (m_cursor.y < rawBuffer.size() - 1) {
		++ m_cursor.y;
		m_cursor.x = 0;
	}

	m_furthestCursorX = m_cursor.x + CountLineTabs();
}

void Buffer::CursorLeft() {
	if (m_cursor.x > 0)
		-- m_cursor.x;
	else if (m_cursor.y > 0) {
		-- m_cursor.y;
		m_cursor.x = CursorLine().length();
	}

	m_furthestCursorX = m_cursor.x + CountLineTabs();
}

void Buffer::CursorUp() {
	if (m_cursor.y > 0) {
		-- m_cursor.y;
		m_cursor.x = m_furthestCursorX;

		std::size_t posX = 0, i = 0;
		for (; i < CursorLine().length(); ++ i) {
			if (CursorLine().at(i) == '\t')
				posX += indentSize;
			else
				++ posX;

			if (posX > m_cursor.x)
				break;
		}

		m_cursor.x = i;
	} else {
		m_cursor.x = 0;
		m_furthestCursorX = m_cursor.x;
	}
}

void Buffer::CursorDown() {
	if (m_cursor.y < rawBuffer.size() - 1) {
		++ m_cursor.y;
		m_cursor.x = m_furthestCursorX;

		std::size_t posX = 0, i = 0;
		for (; i < CursorLine().length(); ++ i) {
			if (CursorLine().at(i) == '\t')
				posX += indentSize;
			else
				++ posX;

			if (posX > m_cursor.x)
				break;
		}

		m_cursor.x = i;
	} else {
		m_cursor.x = CursorLine().length();
		m_furthestCursorX = m_cursor.x + CountLineTabs();
	}
}

void Buffer::CursorWordRight() {
	while (Utils::IsWhitespace(CursorCh())) {
		CursorRight();

		if (m_cursor.x >= CursorLine().length())
			return;
	}

	do {
		if (m_cursor.x >= CursorLine().length()) {
			CursorRight();

			return;
		}

		CursorRight();
	} while (Utils::IsWordCh(CursorCh()));
}

void Buffer::CursorWordLeft() {
	CursorLeft();

	while (Utils::IsWhitespace(CursorCh())) {
		if (m_cursor.x == 0)
			return;

		CursorLeft();
	}

	CursorLeft();
	while (Utils::IsWordCh(CursorCh())) {
		if (m_cursor.x == 0)
			return;

		if (Utils::IsWordCh(CursorCh()))
			CursorLeft();
	}
	CursorRight();
}

void Buffer::CursorFullUp() {
	m_cursor.y = 0;
	m_cursor.x = 0;
}

void Buffer::CursorFullDown() {
	m_cursor.y = rawBuffer.size() - 1;
	m_cursor.x = CursorLine().length();
}


void Buffer::CursorInsert(char p_ch) {
	if (m_selection)
		CursorDelete();

	CursorLine().insert(m_cursor.x, std::string(1, p_ch));
	++ m_cursor.x;

	m_furthestCursorX = m_cursor.x + CountLineTabs();
	m_modified = true;
}

void Buffer::CursorDelete() {
	if (m_selection) {
		Vec2Dw selectStart  = GetSelectionStart();
		Vec2Dw selectionEnd = GetSelectionEnd();

		if (selectStart.y == selectionEnd.y) {
			std::string linePart1 = CursorLine().substr(0, selectStart.x);
			std::string linePart2 = CursorLine().substr(selectionEnd.x);

			CursorLine() = linePart1 + linePart2;
			m_cursor.x   = linePart1.length();
		} else {
			rawBuffer.erase(rawBuffer.begin() + selectStart.y + 1,
			                rawBuffer.begin() + selectionEnd.y);

			selectionEnd.y = selectStart.y + 1;

			std::string linePart1 = rawBuffer.at(selectStart.y).substr(0, selectStart.x);
			std::string linePart2 = rawBuffer.at(selectionEnd.y).substr(selectionEnd.x);

			rawBuffer.erase(rawBuffer.begin() + selectionEnd.y);

			m_cursor.y   = selectStart.y;
			CursorLine() = linePart1 + linePart2;
			m_cursor.x   = linePart1.length();
		}

		m_selection = false;
	} else {
		if (m_cursor.x > 0) {
			CursorLine().erase(m_cursor.x - 1, 1);
			-- m_cursor.x;
		} else if (m_cursor.y > 0) {
			std::string oldLine = CursorLine();

			rawBuffer.erase(rawBuffer.begin() + m_cursor.y);
			-- m_cursor.y;
			m_cursor.x = CursorLine().length();

			CursorLine() += oldLine;
		}
	}

	m_furthestCursorX = m_cursor.x + CountLineTabs();
	m_modified = true;
}

void Buffer::CursorSplit() {
	if (m_selection)
		CursorDelete();

	std::string newLine = CursorLine().substr(m_cursor.x);
	if (newLine.length() > 0)
		CursorLine().erase(m_cursor.x);

	std::size_t i = 0;
	while (CursorLine()[i] == '\t') // not using .at() because i dont want an out of range exception
		++ i;

	++ m_cursor.y;
	rawBuffer.insert(rawBuffer.begin() + m_cursor.y, newLine);
	CursorLine() = std::string(i, '\t') + CursorLine();
	m_cursor.x = i;

	m_furthestCursorX = m_cursor.x + CountLineTabs();
	m_modified = true;
}

void Buffer::UpdateCursorX() {
	if (m_cursor.x > CursorLine().length())
		m_cursor.x = CursorLine().length();
}

void Buffer::UpdateCursorY() {
	if (m_cursor.y >= rawBuffer.size())
		m_cursor.y = rawBuffer.size() - 1;
}

void Buffer::UpdateCursor() {
	// Y has to be updated first, else UpdateCursorX() may throw
	UpdateCursorY();
	UpdateCursorX();
}

void Buffer::MarkSelection() {
	m_selectStart = m_cursor;
	m_selection      = true;
}

void Buffer::UnmarkSelection() {
	m_selection = false;
}

Vec2Dw &Buffer::GetSelectionStart() {
	if (m_cursor.y > m_selectStart.y)
		return m_selectStart;
	else if (m_cursor.y == m_selectStart.y) {
		if (m_cursor.x > m_selectStart.x)
			return m_selectStart;
		else
			return m_cursor;
	} else
		return m_cursor;
}

Vec2Dw &Buffer::GetSelectionEnd() {
	return &GetSelectionStart() == &m_cursor? m_selectStart : m_cursor;
}

const Vec2Dw &Buffer::GetSelectionStart() const {
	if (m_cursor.y > m_selectStart.y)
		return m_selectStart;
	else if (m_cursor.y == m_selectStart.y) {
		if (m_cursor.x > m_selectStart.x)
			return m_selectStart;
		else
			return m_cursor;
	} else
		return m_cursor;
}

const Vec2Dw &Buffer::GetSelectionEnd() const {
	return &GetSelectionStart() == &m_cursor? m_selectStart : m_cursor;
}

bool Buffer::HasSelection() {
	return m_selection;
}

const std::string &Buffer::CursorLine() const {
	return rawBuffer.at(m_cursor.y);
}

std::string &Buffer::CursorLine() {
	return rawBuffer.at(m_cursor.y);
}

char Buffer::CursorCh() const {
	// .at() throws, i do not want it to throw in this case
	return CursorLine()[m_cursor.x];
}

char &Buffer::CursorCh() {
	return CursorLine()[m_cursor.x];
}

bool Buffer::IsModified() const {
	return m_modified;
}

void Buffer::FlagUnmodified() {
	m_modified = false;
}

std::size_t Buffer::CountLineTabs() {
	return std::count(CursorLine().begin(), CursorLine().begin() +
	                  m_cursor.x, '\t') * (indentSize - 1);
}
