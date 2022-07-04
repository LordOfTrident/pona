#ifndef INI_HH__HEADER_GUARD__
#define INI_HH__HEADER_GUARD__

#include <string>        // std::string
#include <unordered_map> // std::unordered_map
#include <cstdlib>       // std::size_t
#include <cstdint>       // std::int64_t
#include <stdexcept>     // std::runtime_error

// modified inicxx
// https://github.com/lordoftrident/inicxx

namespace INI {
	using Section  = std::unordered_map<std::string, std::string>;
	using Sections = std::unordered_map<std::string, Section>;

	// default global ini section with no name
	constexpr char GlobalSection[1] = "";

	class Struct {
	public:
		Struct();
		Struct(const Sections &p_sections);

		Struct&  operator =(const Struct &p_struct);
		Section& operator [](const std::string &p_section);

		const Section& At(const std::string &p_section) const;
		Section&       At(const std::string &p_section);

		const std::string& At(const std::string &p_section, const std::string &p_key) const;
		std::string&       At(const std::string &p_section, const std::string &p_key);

		bool Contains(const std::string &p_section) const;
		bool Contains(const std::string &p_section, const std::string &p_key) const;

		void Clear();
		void Clear(const std::string &p_section);

		// conversion functions
		double       AsNumber(const std::string &p_section, const std::string &p_key);
		std::int64_t AsInteger(const std::string &p_section, const std::string &p_key);
		std::string  AsString(const std::string &p_section, const std::string &p_key);
		bool         AsBool(const std::string &p_section, const std::string &p_key);

		std::string Stringify() const;
		void Parse(const std::string &p_text);

	private:
		void Trim(std::string &p_str);
		void RemoveApostrophes(std::string &p_str);

		std::string Escape(const std::string &p_str) const;
		bool        Unescape(const std::string &p_line, std::string &p_str) const;

		Sections m_sections;
	}; // class Struct
} // namespace INI

#endif
