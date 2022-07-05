#include "editor.hh"
#include "app.hh"

Editor::Editor(
	const Vec2D &p_size, const Vec2D &p_pos, const std::string &p_title,
	const std::vector<std::string> &p_contents
):
	buffer(p_contents),

	m_title(p_title),

	m_ruler(true),
	m_scrollBar(true),
	m_markedColumn(0),

	m_scrolledUp(false),
	m_scrolledDown(false),

	m_winSize(p_size),
	m_winPos(p_pos)
{
	m_win = newwin(m_winSize.y, m_winSize.x, m_winPos.y, m_winPos.x);
	refresh();
}

const std::string &Editor::GetTitle() const {
	return m_title;
}

void Editor::SetTitle(const std::string &p_title) {
	m_title = p_title;
}

void Editor::Update() {
	buffer.UpdateCursor();

	m_rulerWidth    = m_ruler? std::to_string(buffer.Size()).length() + 1 : 0;
	m_maxLineLength = m_winSize.x - m_rulerWidth - m_scrollBar;
	m_maxLines      = m_winSize.y - 1;
	m_cursorTabs    = std::count(buffer.CursorLine().begin(), buffer.CursorLine().begin() +
	                             buffer.GetCursor().x, '\t') * (buffer.indentSize - 1);

	// fix horizontal scroll
	if (buffer.GetCursor().x + m_cursorTabs < m_scroll.x)
		m_scroll.x = buffer.GetCursor().x + m_cursorTabs;
	else if (buffer.GetCursor().x + m_cursorTabs - m_scroll.x >= m_maxLineLength - 1)
		m_scroll.x = buffer.GetCursor().x + m_cursorTabs - m_maxLineLength + 1;

 	// fix vertical scroll
 	std::size_t cursorScreenPosY = buffer.GetCursor().y - m_scroll.y;

	if (buffer.GetCursor().y < m_scroll.y + 3 and m_scroll.y > 0)
		m_scroll.y = buffer.GetCursor().y <= 3? 0 : buffer.GetCursor().y - 3;
	else if (cursorScreenPosY > m_maxLines - 1 and buffer.GetCursor().y >= buffer.Size() - 1)
		m_scroll.y = buffer.GetCursor().y - m_maxLines + 1;
	else if (cursorScreenPosY > m_maxLines - 4 and m_scroll.y + m_maxLines < buffer.Size())
		m_scroll.y = buffer.GetCursor().y - m_maxLines + 4;

	if (buffer.Size() > m_maxLines) {
		if (m_scroll.y + m_maxLines > buffer.Size())
			m_scroll.y = buffer.Size() - m_maxLines;
	}
}

void Editor::ResizeWindow(const Vec2D &p_size) {
	m_winSize = p_size;

	wresize(m_win, m_winSize.y, m_winSize.x);
}

void Editor::ReposWindow(const Vec2D &p_pos) {
	m_winPos = p_pos;

	mvwin(m_win, m_winPos.y, m_winPos.x);
}

void Editor::PosCursor() {
	move(buffer.GetCursor().y + m_winPos.y - m_scroll.y,
	     buffer.GetCursor().x + m_winPos.x + m_rulerWidth - m_scroll.x + m_cursorTabs);
}

void Editor::EnableRuler(bool p_ruler) {
	m_ruler = p_ruler;
}

void Editor::EnableScrollBar(bool p_scrollBar) {
	m_scrollBar = p_scrollBar;
}

void Editor::SetIndentSize(std::uint8_t p_indentSize) {
	if (p_indentSize == 0)
		p_indentSize = 1;
	else if (p_indentSize > 16)
		p_indentSize = 16;

	buffer.indentSize = p_indentSize;
}

void Editor::SetMarkedColumn(std::size_t p_markedColumn) {
	m_markedColumn = p_markedColumn;
}

void Editor::SetReadOnly(bool p_readOnly) {
	m_readOnly = p_readOnly;
}

void Editor::ScrollUp() {
	m_scrolledUp = true;
	buffer.SetCursorY(m_scroll.y >= 5? m_scroll.y - 5 : 0);
}

void Editor::ScrollDown() {
	m_scrolledDown = true;
	if (m_scroll.y + m_maxLines < buffer.Size()) {
		std::size_t calc = m_scroll.y + m_maxLines + 4;
		buffer.SetCursorY(calc < buffer.Size()? calc : buffer.Size() - 1);
	}
}

void Editor::CopySelection() {
	if (not buffer.HasSelection())
		return;

	Clipboard::OpenStream(Clipboard::For::Writing);

	const Vec2Dw &selectStart = buffer.GetSelectionStart();
	const Vec2Dw &selectEnd   = buffer.GetSelectionEnd();

	if (selectStart.y == selectEnd.y)
		Clipboard::WriteString(buffer.rawBuffer.at(selectStart.y).substr(selectStart.x,
		                     selectEnd.x - selectStart.x));
	else {
		Clipboard::WriteString(buffer.rawBuffer.at(selectStart.y).substr(selectStart.x) + '\n');

		for (std::size_t i = selectStart.y + 1; i < selectEnd.y; ++ i)
			Clipboard::WriteString(buffer.rawBuffer.at(i) + '\n');

		Clipboard::WriteString(buffer.rawBuffer.at(selectEnd.y).substr(0, selectEnd.x));
	}

	Clipboard::CloseStream();
}

void Editor::PasteSelection() {
	if (m_readOnly)
		return;

	Clipboard::OpenStream(Clipboard::For::Reading);

	if (buffer.HasSelection())
		buffer.CursorDelete();

	std::vector<std::string> pasteBuffer;
	Clipboard::ReadAll(pasteBuffer);

	if (pasteBuffer.size() == 1) {
		buffer.rawBuffer.at(buffer.GetCursor().y).insert(buffer.GetCursor().x, pasteBuffer.at(0));
		buffer.SetCursorX(buffer.GetCursor().x + pasteBuffer.at(0).length());
	} else if (pasteBuffer.size() > 1) {
		std::string split = buffer.CursorLine().substr(buffer.GetCursor().x);
		buffer.CursorLine().erase(buffer.GetCursor().x);
		buffer.CursorLine() += pasteBuffer.at(0);

		if (pasteBuffer.size()) {
			buffer.rawBuffer.insert(buffer.rawBuffer.begin() + buffer.GetCursor().y + 1,
			                        pasteBuffer.begin() + 1, pasteBuffer.end());
		}

		buffer.SetCursorY(buffer.GetCursor().y + pasteBuffer.size() - 1);
		buffer.SetCursorX(buffer.CursorLine().length());
		buffer.CursorLine() += split;
	}

	Clipboard::CloseStream();
}

void Editor::Input(NC::input_t p_input) {
	switch (p_input) {
	case NC::Key::Mouse:
		MEVENT mouseEvent;
		if (getmouse(&mouseEvent) != OK)
			break;

		mouseEvent.x -= m_winPos.x;
		mouseEvent.y -= m_winPos.y;

		if (mouseEvent.bstate & BUTTON1_PRESSED) {
			if (m_scrollBar and buffer.Size() > m_maxLines and mouseEvent.x == m_winSize.x - 1) {
				// scroll by 8 lines up/down
				if (mouseEvent.y == 0)
					ScrollUp();
				else if (mouseEvent.y == static_cast<int>(m_maxLines) - 1)
					ScrollDown();
			} else {
				buffer.SetCursor(Vec2Dw(mouseEvent.x + m_scroll.x - m_rulerWidth,
				                        mouseEvent.y + m_scroll.y));

				// tabs mess things up, so we need to count up
				std::size_t posX = 0, i = 0;
				for (; i < buffer.CursorLine().length(); ++ i) {
					if (buffer.CursorLine().at(i) == '\t')
						posX += buffer.indentSize;
					else
						++ posX;

					if (posX > buffer.GetCursor().x)
						break;
				}

				buffer.UnmarkSelection();
				buffer.SetCursorX(i);
			}
		}

		break;

	case NC::Key::Backspace: if (not m_readOnly) buffer.CursorDelete(); break;
	case NC::Key::Enter:     if (not m_readOnly) buffer.CursorSplit();  break;

	// cursor movement
	case NC::Key::Right: buffer.UnmarkSelection(); buffer.CursorRight(); break;
	case NC::Key::Left:  buffer.UnmarkSelection(); buffer.CursorLeft();  break;
	case NC::Key::Up:    buffer.UnmarkSelection(); buffer.CursorUp();    break;
	case NC::Key::Down:  buffer.UnmarkSelection(); buffer.CursorDown();  break;

	// cursor movement + selection
	case NC::Key::Shift(NC::Key::Right):
		if (not buffer.HasSelection())
			buffer.MarkSelection();

		buffer.CursorRight();

		break;

	case NC::Key::Shift(NC::Key::Left):
		if (not buffer.HasSelection())
			buffer.MarkSelection();

		buffer.CursorLeft();

		break;

	case NC::Key::Shift(NC::Key::Up):
		if (not buffer.HasSelection())
			buffer.MarkSelection();

		buffer.CursorUp();

		break;

	case NC::Key::Shift(NC::Key::Down):
		if (not buffer.HasSelection())
			buffer.MarkSelection();

		buffer.CursorDown();

		break;

	case NC::Key::Ctrl(NC::Key::Right): buffer.UnmarkSelection(); buffer.CursorWordRight(); break;
	case NC::Key::Ctrl(NC::Key::Left):  buffer.UnmarkSelection(); buffer.CursorWordLeft();  break;
	case NC::Key::Ctrl(NC::Key::Up):    buffer.UnmarkSelection(); buffer.CursorFullUp();    break;
	case NC::Key::Ctrl(NC::Key::Down):  buffer.UnmarkSelection(); buffer.CursorFullDown();  break;

	case NC::Key::Shift(NC::Key::Ctrl(NC::Key::Right)):
		if (not buffer.HasSelection())
			buffer.MarkSelection();

		buffer.CursorWordRight();

		break;

	case NC::Key::Shift(NC::Key::Ctrl(NC::Key::Left)):
		if (not buffer.HasSelection())
			buffer.MarkSelection();

		buffer.CursorWordLeft();

		break;

	case NC::Key::Shift(NC::Key::Ctrl(NC::Key::Up)):
		if (not buffer.HasSelection())
			buffer.MarkSelection();

		buffer.CursorFullUp();

		break;

	case NC::Key::Shift(NC::Key::Ctrl(NC::Key::Down)):
		if (not buffer.HasSelection())
			buffer.MarkSelection();

		buffer.CursorFullDown();

		break;

	// specials
	case NC::Key::Ctrl('a'):
		buffer.CursorFullDown();
		buffer.MarkSelection();
		buffer.CursorFullUp();

		break;

	case NC::Key::Alt('e'): buffer.SetCursorX(buffer.CursorLine().length()); break;

	case NC::Key::Ctrl('x'):
		if (m_readOnly or not buffer.HasSelection())
			break;

		CopySelection();
		buffer.CursorDelete();

		break;

	case NC::Key::Ctrl('c'): CopySelection();  break;
	case NC::Key::Ctrl('v'): PasteSelection(); break;

	case NC::Key::PrevPage: ScrollUp();   break;
	case NC::Key::NextPage: ScrollDown(); break;

	default:
		if (((p_input >= 32 and p_input <= 127) or (p_input == NC::Key::Tab)) and not m_readOnly)
			buffer.CursorInsert(p_input);

		break;
	}
}

void Editor::Render() {
	wbkgdset(m_win, COLOR_PAIR(NC::ColorPair::Editor));
	werase(m_win);

	RenderContents();
	RenderScrollBar();
	RenderInfoBar();

	wrefresh(m_win);
}

void Editor::RenderInfoBar() {
	NC::WSetColor(m_win, NC::ColorPair::Editor_InfoBar);

	mvwhline(m_win, m_winSize.y - 1, 0, ' ', m_winSize.x);

	std::string posStr = std::to_string(buffer.GetCursor().y + 1) +
	                     ':' + std::to_string(buffer.GetCursor().x + 1);
	mvwaddstr(m_win, m_winSize.y - 1, m_winSize.x - posStr.length() - 1, posStr.c_str());
	mvwaddstr(m_win, m_winSize.y - 1, 1, "^H ");

	NC::WSetColor(m_win, NC::ColorPair::Editor_InfoBarSeparator);
	waddch(m_win, '|');

	NC::WSetColor(m_win, NC::ColorPair::Editor_InfoBar);
	waddstr(m_win, " ASCII ");

	NC::WSetColor(m_win, NC::ColorPair::Editor_InfoBarSeparator);
	waddch(m_win, '|');

	NC::WSetColor(m_win, NC::ColorPair::Editor_InfoBar);
	waddstr(m_win, " LF ");

	NC::WSetColor(m_win, NC::ColorPair::Editor_InfoBarSeparator);
	waddch(m_win, '|');

	NC::WSetColor(m_win, NC::ColorPair::Editor_InfoBar);
	waddstr(m_win, m_readOnly? " R " : " RW ");

	NC::WSetColor(m_win, NC::ColorPair::Editor_InfoBarSeparator);
	waddch(m_win, '|');

	NC::WSetColor(m_win, NC::ColorPair::Editor_InfoBar);
	waddstr(m_win, " txt ");
}

void Editor::RenderScrollBar() {
	if (m_scrollBar) {
		NC::WSetColor(m_win, NC::ColorPair::Editor_ScrollBack);

		wattron(m_win, A_ALTCHARSET);
		mvwvline(m_win, 0, m_winSize.x - 1, ACS_CKBOARD, m_winSize.y);
		wattroff(m_win, A_ALTCHARSET);

		if (buffer.Size() > m_maxLines) {
			std::size_t maxSize = m_maxLines - 2;
			// calculate scrollbar position
			float fPos  = static_cast<float>(m_scroll.y) / buffer.Size() * m_maxLines;
			float fSize = static_cast<float>(m_maxLines) / buffer.Size() * m_maxLines;

			// change the range so the scroll bar fits between the buttons
			int pos  = fPos * maxSize / m_maxLines;
			int size = std::ceil(fSize * maxSize / m_maxLines);

			NC::WSetColor(m_win, NC::ColorPair::Editor_ScrollBar);

			wattron(m_win, A_ALTCHARSET);
			mvwvline(m_win, pos + 1, m_winSize.x - 1, ACS_CKBOARD, size);
			wattroff(m_win, A_ALTCHARSET);

			// draw scroll buttons
			NC::WSetColor(m_win, m_scrolledUp? NC::ColorPair::Editor_ScrollButtonAlt :
			              NC::ColorPair::Editor_ScrollButton);

			NC::MoveWAddWCh(m_win, 0, m_winSize.x - 1, 0x25B2);

			NC::WSetColor(m_win, m_scrolledDown? NC::ColorPair::Editor_ScrollButtonAlt :
			              NC::ColorPair::Editor_ScrollButton);

			NC::MoveWAddWCh(m_win, m_maxLines - 1, m_winSize.x - 1, 0x25BC);
		}

		m_scrolledUp   = false;
		m_scrolledDown = false;
	}
}

void Editor::RenderContents() {
	Vec2Dw pos;
	for (pos.y = m_scroll.y; pos.y < buffer.Size() and pos.y < m_scroll.y + m_maxLines; ++ pos.y) {
		wmove(m_win, pos.y - m_scroll.y, 0);

		bool isCursorLine = pos.y == buffer.GetCursor().y;

		if (m_ruler) {
			std::string lineCountStr = std::to_string(pos.y + 1);

			if (isCursorLine) {
				NC::WSetColor(m_win, NC::ColorPair::Editor_RulerCurrent);
				wattron(m_win, A_BOLD);
			} else
				NC::WSetColor(m_win, NC::ColorPair::Editor_Ruler);

			// draw spaces to align the ruler line count
			std::size_t length = m_rulerWidth - 1 - lineCountStr.length();
			if (length > 0)
				waddstr(m_win, std::string(length, ' ').c_str());

			waddstr(m_win, (lineCountStr + ' ').c_str());

			if (isCursorLine)
				wattroff(m_win, A_BOLD);
		}

		RenderLine(pos, isCursorLine);
	}

	RenderBelowContents();
}

void Editor::RenderLine(Vec2Dw &p_pos, bool p_isCursorLine) {
	const std::string &ref = buffer.rawBuffer.at(p_pos.y);

	short lineColor = p_isCursorLine? NC::ColorPair::Editor_Current : NC::ColorPair::Editor;
	NC::WSetColor(m_win, lineColor);

	std::string line = ref;
	line = std::regex_replace(line, std::regex("\t"), std::string(buffer.indentSize, ' '));

	Vec2Dw selectStart = buffer.GetSelectionStart();
	Vec2Dw selectEnd   = buffer.GetSelectionEnd();

	selectStart.x += std::count(ref.begin(), ref.begin() + selectStart.x, '\t') *
	                 (buffer.indentSize - 1);
	selectEnd.x += std::count(ref.begin(), ref.begin() + selectEnd.x, '\t') *
	               (buffer.indentSize - 1);

	for (p_pos.x = m_scroll.x; p_pos.x - m_scroll.x < m_maxLineLength; ++ p_pos.x) {
		bool markColumn = m_markedColumn and p_pos.x == m_markedColumn;
		bool isBeyondMarkedColumn = m_markedColumn and p_pos.x > m_markedColumn;
		if ((markColumn or isBeyondMarkedColumn) and not p_isCursorLine)
			NC::WSetColor(m_win, NC::ColorPair::Editor_MarkedColumn);

		bool inSelection = false;
		if (buffer.HasSelection()) {
			if (selectStart.y < p_pos.y and selectEnd.y > p_pos.y)
				inSelection = true;
			else if (
				selectStart.y == p_pos.y and selectEnd.y == p_pos.y and
				selectStart.x <= p_pos.x and selectEnd.x >  p_pos.x
			)
				inSelection = true;
			else if (
				selectStart.y == p_pos.y and selectEnd.y != p_pos.y and
				selectStart.x <= p_pos.x
			)
				inSelection = true;
			else if (
				selectEnd.y == p_pos.y and selectStart.y != p_pos.y and
				selectEnd.x > p_pos.x
			)
				inSelection = true;
		}

		if (p_pos.x < line.length()) {
			if (inSelection)
				NC::WSetColor(m_win, NC::ColorPair::Editor_Selection);
			else
				NC::WSetColor(m_win, lineColor);

			char ch = line.at(p_pos.x);

			if (markColumn and ch == ' ') {
				// draw the mark column if the char is just a space
				wattron(m_win, A_ALTCHARSET);

				mvwaddch(m_win, p_pos.y - m_scroll.y, p_pos.x +
				         m_rulerWidth - m_scroll.x, ACS_VLINE);

				wattroff(m_win, A_ALTCHARSET);
			} else if (ch < 32 or ch > 126) {
				// draw error char
				NC::WAddWCh(m_win, 0xFFFD);
			} else
				waddch(m_win, line.at(p_pos.x));
		} else {
			if (inSelection and p_pos.x == line.length()) {
				NC::WSetColor(m_win, NC::ColorPair::Editor_Selection);
				waddch(m_win, ' ');
			} else if (buffer.HasSelection()) {
				NC::WSetColor(m_win, lineColor);

				if (markColumn) { // draw the mark column
					wattron(m_win, A_ALTCHARSET);

					mvwaddch(m_win, p_pos.y - m_scroll.y,
					        p_pos.x + m_rulerWidth - m_scroll.x, ACS_VLINE);

					wattroff(m_win, A_ALTCHARSET);
				} else if (p_isCursorLine or isBeyondMarkedColumn)
					waddch(m_win, ' '); // draw a space so the selection line and the
					                    // marked column background are drawn
			}
		}
	}
}

void Editor::RenderBelowContents() {
	// render the rest of the ruler and marked column if contents dont cover
	// the entire editor screen
	if (buffer.Size() < m_maxLines) {
		Vec2Dw pos(m_markedColumn + m_rulerWidth - m_scroll.x, buffer.Size() - m_scroll.y);

		wattron(m_win, A_ALTCHARSET);

		if (pos.x >= m_rulerWidth) {
			for (; pos.y < m_scroll.y + m_maxLines; ++ pos.y) {
				if (m_markedColumn) {
					NC::WSetColor(m_win, NC::ColorPair::Editor_MarkedColumn);

					mvwhline(m_win, pos.y, pos.x, ' ', m_maxLineLength - pos.x);
					mvwaddch(m_win, pos.y, pos.x, ACS_VLINE);
				}

				NC::WSetColor(m_win, NC::ColorPair::Editor_Ruler);

				mvwhline(m_win, pos.y, 0, ' ', m_rulerWidth);
			}
		}

		wattroff(m_win, A_ALTCHARSET);
	}
}
