#include "clipboard.hh"

std::vector<std::string> Clipboard::g_internal;

std::FILE *Clipboard::g_file;
Clipboard::For Clipboard::g_for;

bool Clipboard::g_hasXClip = true;

void Clipboard::OpenXClipFile(const std::string &p_for) {
	std::string cmd = std::string("xclip -") + (p_for == "r"? 'o' : 'i') +
	                  " -sel clip 2> /dev/null";

	g_file = popen(cmd.c_str(), p_for.c_str());
	if (g_file == nullptr)
		throw std::runtime_error("popen() returned nullptr");
}

void Clipboard::CloseXClipFile() {
	int ret = pclose(g_file);
	g_file = nullptr;

	if (ret == 127 or ret == 32512)
		throw std::runtime_error("XClip is not available");
}

void Clipboard::Init() {
	try {
		OpenXClipFile("w");
		CloseXClipFile();
	} catch (const std::exception &p_except) {
		g_hasXClip = false;
	}
}

bool Clipboard::HasXClip() {
	return g_hasXClip;
}

void Clipboard::OpenStream(For p_for) {
	g_for = p_for;

	const char *openFor;
	switch (p_for) {
	case For::Reading: openFor = "r"; break;
	default:
		if (not g_hasXClip)
			g_internal.clear();

		openFor = "w";

		break;
	}

	OpenXClipFile(openFor);
}

void Clipboard::CloseStream() {
	if (g_hasXClip)
		CloseXClipFile();
}

void Clipboard::WriteString(const std::string &p_str) {
	if (g_for != For::Writing)
		throw std::runtime_error("Clipboard stream not open for writing, called WriteLine()");

	if (g_hasXClip)
		std::fputs((p_str).c_str(), g_file);
	else {
		std::string str = p_str;
		if (str.back() == '\n')
			str = str.substr(0, str.length() - 1);

		g_internal.push_back(str);
	}
}

void Clipboard::ReadAll(std::vector<std::string> &p_buffer) {
	if (g_for != For::Reading)
		throw std::runtime_error("Clipboard stream not open for reading, called ReadAll()");

	p_buffer.clear();

	if (g_hasXClip) {
		p_buffer.push_back("");

		int ch;
		while ((ch = std::fgetc(g_file)) != EOF) {
			if (ch == '\n') {
				p_buffer.push_back("");

				continue;
			}

			p_buffer.back() += (char)ch;
		}
	} else {
		for (const auto &line : g_internal)
			p_buffer.push_back(line);
	}
}
