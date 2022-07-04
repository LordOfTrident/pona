#include "ini.hh"

INI::Struct::Struct() {}
INI::Struct::Struct(const Sections &p_sections) {
	m_sections = p_sections;
}

INI::Struct& INI::Struct::operator =(const Struct &p_struct) {
	m_sections = p_struct.m_sections;

	return *this;
}

INI::Section& INI::Struct::operator [](const std::string &p_section) {
	return m_sections[p_section];
}

const INI::Section& INI::Struct::At(const std::string &p_section) const {
	return m_sections.at(p_section);
}

INI::Section& INI::Struct::At(const std::string &p_section) {
	return m_sections.at(p_section);
}

const std::string& INI::Struct::At(const std::string &p_section, const std::string &p_key) const {
	return m_sections.at(p_section).at(p_key);
}

std::string& INI::Struct::At(const std::string &p_section, const std::string &p_key) {
	return m_sections.at(p_section).at(p_key);
}

bool INI::Struct::Contains(const std::string &p_section) const {
	return m_sections.count(p_section);
}

bool INI::Struct::Contains(const std::string &p_section, const std::string &p_key) const {
	if (m_sections.count(p_section))
		return m_sections.at(p_section).count(p_key);

	return false;
}

void INI::Struct::Clear() {
	m_sections.clear();
}

void INI::Struct::Clear(const std::string &p_section) {
	m_sections.at(p_section).clear();
}

// Conversion functions
double INI::Struct::AsNumber(const std::string &p_section, const std::string &p_key) {
	return std::stod(m_sections.at(p_section).at(p_key));
}

std::int64_t INI::Struct::AsInteger(const std::string &p_section, const std::string &p_key) {
	return static_cast<std::int64_t>(std::stoll(m_sections.at(p_section).at(p_key)));
}

std::string INI::Struct::AsString(const std::string &p_section, const std::string &p_key) {
	return m_sections.at(p_section).at(p_key);
}

bool INI::Struct::AsBool(const std::string &p_section, const std::string &p_key) {
	const std::string &val = m_sections.at(p_section).at(p_key);

	if (val == "true")
		return true;
	else if (val == "false")
		return false;

	throw std::runtime_error("Attempt to use AsBool() on a non-bool value");
}

std::string INI::Struct::Stringify() const {
	std::string out = "";

	for (const auto &[name, sect] : m_sections) {
		if (name[0] == ' ' or name.back() == ' ')
			out += "[\"" + Escape(name) + "\"]\n";
		else
			out += '[' + Escape(name) + "]\n";

		for (const auto &[key, value] : sect) {
			if (key[0] == ' ' or key.back() == ' ')
				out += '"' + Escape(key) + '"';
			else
				out += Escape(key);

			out += "=";

			if (value[0] == ' ' or value.back() == ' ')
				out += "\"" + Escape(value) + "\"\n";
			else
				out += Escape(value) + "\n";
		}
	}

	return out;
}

void INI::Struct::Parse(const std::string &p_text) {
	Clear();

	std::string line = "";
	std::string currentSection = GlobalSection;
	std::size_t currentLine = 0;

	for (const auto &ch : p_text + '\n') {
		switch (ch) {
		case '\n':
			++ currentLine;
			Trim(line);

			switch (line[0]) {
			case ';': case '#': break;
			case '[':
				{
					std::size_t pos = line.find_first_of(']');
					if (pos == std::string::npos)
						throw std::runtime_error("Sector name closing expected at line " +
						                         std::to_string(currentLine));

					if (pos != line.length() - 1)
						throw std::runtime_error("Unexpected character at line " +
						                         std::to_string(currentLine));

					currentSection = line.substr(1, pos - 1);
					Trim(currentSection);
					RemoveApostrophes(currentSection);
				}

				break;

			default:
				if (line == "")
					break;

				{
					std::size_t pos = 0;

					do {
						pos = line.find_first_of('=', pos + 1);
						if (pos == std::string::npos)
							throw std::runtime_error("Expected assignment at line " +
							                         std::to_string(currentLine));
					} while (line.at(pos - 1) == '\\');

					std::string key = line.substr(0, pos);
					std::string value = "";
					line = line.substr(pos + 1);

					if (Unescape(std::string(key), key))
						throw std::runtime_error("Expected assignment at line " +
						                         std::to_string(currentLine));

					Unescape(line, value);

					Trim(key);
					Trim(value);
					RemoveApostrophes(key);
					RemoveApostrophes(value);

					if (key == "")
						throw std::runtime_error("Key name expected at line " +
						                         std::to_string(currentLine));

					m_sections[currentSection][key] = value;
				}

				break;
			}

			line = "";

			break;

		default: line += ch; break;
		}
	}
}

void INI::Struct::Trim(std::string &p_str) {
	std::size_t start = p_str.find_first_not_of(" \t");

	if (start == std::string::npos) {
		p_str = "";

		return;
	}

	std::size_t end = p_str.find_last_not_of(" \t");

	p_str = p_str.substr(start, end + 1 - start);
}

void INI::Struct::RemoveApostrophes(std::string &p_str) {
	if (p_str[0] == '"' and p_str.back() == '"')
		p_str = p_str.substr(1, p_str.length() - 2);
}

std::string INI::Struct::Escape(const std::string &p_str) const {
	std::string escaped = "";

	for (auto ch : p_str) {
		switch (ch) {
		case '\n': escaped += "\\n";  break;
		case '\r': escaped += "\\r";  break;
		case '\t': escaped += "\\t";  break;
		case '\f': escaped += "\\f";  break;
		case '\b': escaped += "\\b";  break;
		case '\a': escaped += "\\a";  break;
		case '\0': escaped += "\\0";  break;
		case '"':  escaped += "\\\""; break;
		default:   escaped += ch;     break;
		}
	}

	return escaped;
}

bool INI::Struct::Unescape(const std::string &p_line, std::string &p_str) const {
	bool escape = false;
	p_str = "";

	for (const char &ch : p_line) {
		if (escape) {
			escape = false;

			switch (ch) {
			case 'n':  p_str += '\n'; break;
			case 'r':  p_str += '\r'; break;
			case 't':  p_str += '\t'; break;
			case 'f':  p_str += '\f'; break;
			case 'b':  p_str += '\b'; break;
			case 'a':  p_str += '\a'; break;
			case '0':  p_str += '\0'; break;
			case '"':  p_str += '\"'; break;
			case '\\': p_str += '\\'; break;
			case ';':  p_str += ';';  break;
			}
		}

		switch (ch) {
		case ';': case '#': return true;
		case '\\': escape = true; break;
		default:   p_str += ch;   break;
		}
	}

	return false;
}
