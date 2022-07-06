#include "utils.hh"

std::fs::path Utils::g_home = "";

void Utils::TrimStringRight(std::string &p_str, char p_trimCh) {
	p_str.erase(p_str.find_last_not_of(p_trimCh) + 1);
}

void Utils::TrimStringLeft(std::string &p_str, char p_trimCh) {
	p_str.erase(0, p_str.find_first_not_of(p_trimCh));
}

void Utils::TrimString(std::string &p_str, char p_trimCh) {
	TrimStringRight(p_str, p_trimCh);
	TrimStringLeft(p_str, p_trimCh);
}

bool Utils::StringEndsWith(const std::string &p_str, const std::string &p_suffix) {
	return p_str.rfind(p_suffix) == p_str.length() - p_suffix.length();
}

bool Utils::StringStartsWith(const std::string &p_str, const std::string &p_prefix) {
	return p_str.find(p_prefix, 0) == 0;
}

void Utils::RemoveSuffix(std::string &p_str, const std::string &p_suffix) {
	if (not StringEndsWith(p_str, p_suffix))
		return;

	p_str = p_str.substr(0, p_str.length() - p_suffix.length());
}

void Utils::RemovePrefix(std::string &p_str, const std::string &p_prefix) {
	if (not StringStartsWith(p_str, p_prefix))
		return;

	p_str = p_str.substr(p_prefix.length());
}

const std::fs::path &Utils::Home() {
	if (g_home.u8string().size() == 0) {
		const char *raw = std::getenv("HOME");
		if (raw == nullptr)
			throw std::runtime_error("std::getenv() returned nullptr");

		g_home = raw;
	}

	return g_home;
}

void Utils::UnbloatPath(std::string &p_path) {
	TrimString(p_path);

	// change paths like //home///user/ into /home/user
	TrimStringRight(p_path, '/');

	bool prevWasSeparator = false;
	for (std::size_t i = 0; i < p_path.length(); ++ i) {
		if (p_path[i] == '/') {
			if (prevWasSeparator) {
				p_path.erase(i, 1);
				-- i;
			} else
				prevWasSeparator = true;
		} else
			prevWasSeparator = false;
	}
}

void Utils::FixPath(std::fs::path &p_path) {
	std::string pathStr = p_path.u8string();

	UnbloatPath(pathStr);

	if (pathStr.substr(0, 2) == "~/")
		p_path = Home()/pathStr.substr(2);
	else
		p_path = pathStr;
}

std::string Utils::PathToFileName(const std::fs::path &p_path) {
	// /home/user/file.txt to file.txt
	std::string pathStr = p_path.u8string();
	return pathStr.substr(pathStr.find_last_of('/') + 1);
}

bool Utils::PathExists(std::fs::path p_path) {
	FixPath(p_path);

	return std::fs::exists(p_path);
}

void Utils::ReadFile(std::fs::path p_path, std::vector<std::string> &p_buffer) {
	FixPath(p_path);

	std::ifstream file(p_path);

	if (file.is_open()) {
		p_buffer.clear();

		std::string line;
		while (std::getline(file, line)) {
			if (line.back() == '\r') // if the file uses CRLF, convert it to LF
				line = line.substr(0, line.length() - 1);

			p_buffer.push_back(line);
		}

		file.close();
	} else
		throw std::runtime_error("Failed to open file '" + p_path.u8string() + "' for reading");
}

void Utils::ReadFile(std::fs::path p_path, std::string &p_buffer) {
	FixPath(p_path);

	std::ifstream file(p_path);

	if (file.is_open()) {
		p_buffer.clear();

		std::string line;
		while (std::getline(file, line)) {
			if (line.back() == '\r')
				line = line.substr(0, line.length() - 1);

			p_buffer += line + '\n';
		}

		file.close();
	} else
		throw std::runtime_error("Failed to open file '" + p_path.u8string() + "' for reading");
}

void Utils::WriteFile(std::fs::path p_path, const std::vector<std::string> &p_buffer) {
	FixPath(p_path);

	std::ofstream file(p_path);

	if (file.is_open()) {
		for (const auto &line : p_buffer)
			file << line << '\n';

		file.close();
	} else
		throw std::runtime_error("Failed to open file '" + p_path.u8string() + "' for writing");
}

void Utils::WriteFile(std::fs::path p_path, const std::string &p_contents) {
	FixPath(p_path);

	std::ofstream file(p_path);

	if (file.is_open()) {
		file << p_contents;

		file.close();
	} else
		throw std::runtime_error("Failed to open file '" + p_path.u8string() + "' for writing");
}

bool Utils::IsWhitespace(char p_ch) {
	return std::isspace(p_ch);
}

bool Utils::IsWordCh(char p_ch) {
	return p_ch == '_' or std::isalnum(p_ch);
}

bool Utils::IsLinkCh(char p_ch) {
	switch (p_ch) {
	// these are all the characters allowed in an URL, right?
	case '-': case '.': case '_': case '~': case '?':
	case '/': case '#': case ':': case '%': case '@':
	case '=': case '+': case '$': case '*': case '&':
	case ';': case ',':
		return true;

		break;

	default: return std::isalnum(p_ch); break;
	}
}

wchar_t Utils::UTF8ToUTF32(const char p_ch[5]) {
    if ((p_ch[0] & 128) == 0)
        return p_ch[0] & 127;
    else if ((p_ch[0] & 224) == 192)
        return
            (p_ch[0] & 31) << 6 |
            (p_ch[1] & 63);
    else if ((p_ch[0] & 240) == 224)
        return
            (p_ch[0] & 15) << 12 |
            (p_ch[1] & 63) << 6  |
            (p_ch[2] & 63);
    else
        return
            (p_ch[0] & 7)  << 18 |
            (p_ch[1] & 63) << 12 |
            (p_ch[2] & 63) << 6  |
            (p_ch[3] & 63);
}

std::string Utils::ToString(bool p_value) {
	return p_value? "true" : "false";
}
