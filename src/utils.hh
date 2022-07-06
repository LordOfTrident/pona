#ifndef UTILS_HH__HEADER_GUARD__
#define UTILS_HH__HEADER_GUARD__

#include <string>     // std::string, std::getline
#include <vector>     // std::vector
#include <fstream>    // std::ifstream, std::ofstream
#include <cctype>     // std::isspace std::isalnum
#include <cstdlib>    // std::getenv
#include <stdexcept>  // std::runtime_error
#include <filesystem> // std::filesystem

namespace std {
	namespace fs = std::filesystem;
}

namespace Utils {
	extern std::fs::path g_home;

	void TrimStringRight(std::string &p_str, char p_trimCh = ' ');
	void TrimStringLeft(std::string &p_str, char p_trimCh = ' ');
	void TrimString(std::string &p_str, char p_trimCh = ' ');

	bool StringEndsWith(const std::string &p_str, const std::string &p_suffix);
	bool StringStartsWith(const std::string &p_str, const std::string &p_prefix);

	void RemoveSuffix(std::string &p_str, const std::string &p_suffix);
	void RemovePrefix(std::string &p_str, const std::string &p_prefix);

	const std::fs::path &Home();
	void UnbloatPath(std::string &p_path);
	void FixPath(std::fs::path &p_path);
	std::string PathToFileName(const std::fs::path &p_path);

	bool PathExists(std::fs::path p_path);

	void ReadFile(std::fs::path p_path, std::vector<std::string> &p_buffer);
	void ReadFile(std::fs::path p_path, std::string &p_buffer);
	void WriteFile(std::fs::path p_path, const std::vector<std::string> &p_buffer);
	void WriteFile(std::fs::path p_path, const std::string &p_contents);

	bool IsWhitespace(char p_ch);
	bool IsWordCh(char p_ch);
	bool IsLinkCh(char p_ch);

	wchar_t UTF8ToUTF32(const char p_ch[5]);

	std::string ToString(bool p_value);
}

#endif
