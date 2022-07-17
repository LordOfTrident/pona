#ifndef TOPBAR_HH__HEADER_GUARD__
#define TOPBAR_HH__HEADER_GUARD__

#include <string> // std::string
#include <vector> // std::vector
#include <cctype> // std::toupper

#include "ncurses.hh"
#include "types.hh"

namespace TopBar {
	enum class State {
		None = 0,
		Input,
		Error,
		Question
	}; // enum class State

	enum class Answer {
		None = 0,
		Yes,
		No
	}; // enum class Answer

	void Init();
	void Finish();

	void Input(NC::input_t p_input);
	void CommandInput(NC::input_t p_input);
	void QuestionInput(NC::input_t p_input);

	void Render();
	void RenderInputBar();
	void RenderMessage();
	void RenderTabs();
	void RenderMatchesList();

	void Update();
	void GetMatchingCommands();
	void GetMatchingThemes();
	std::size_t CalcCurrentToken();

	void ResizeWindow();

	void SetInput(const std::string &p_input);

	void Ask(const std::string &p_question);
	void Error(const std::string &p_msg);
	void Reset();
	void PosCursor();

	extern WINDOW *g_win, *g_matchListWin;

	extern State g_state;

	extern std::string g_input;
	extern Answer      g_answer;

	extern std::size_t g_cursorX, g_scrollX, g_length;

	extern std::vector<std::string> g_history;
	extern std::size_t g_historyPos;

	extern std::vector<std::string> g_matches, g_tokens;
	extern std::size_t g_currentTokenIdx;
} // namespace TopBar

#endif
