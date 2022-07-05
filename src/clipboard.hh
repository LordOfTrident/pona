#ifndef CLIPBOARD_HH__HEADER_GUARD__
#define CLIPBOARD_HH__HEADER_GUARD__

#include <string>    // std::string
#include <vector>    // std::vector
#include <cstdio>    // std::FILE, popen, pclose, std::fputs, std::fgetc
#include <stdexcept> // std::runtime_error

namespace Clipboard {
	enum class For {
		Writing,
		Reading,
	}; // enum class For

	void OpenXClipFile(const std::string &p_for);
	void CloseXClipFile();

	void Init();

	bool HasXClip();

	void OpenStream(For p_for);
	void CloseStream();

	void WriteString(const std::string &p_str);
	void ReadAll(std::vector<std::string> &p_buffer);

	extern std::vector<std::string> g_internal;

	extern std::FILE *g_file;
	extern For g_for;

	extern bool g_hasXClip;
}

#endif
