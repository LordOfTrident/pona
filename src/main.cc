#include <iostream>  // std::cerr
#include <stdexcept> // std::exception
#include <cstdlib>   // std::exit, EXIT_FAILURE
#include <string>    // std::string

#include "app.hh"

// for generating nice fatal error output :)
void FailureExit(const std::string &p_class, const std::string &p_what) {
	try {
		Finish();
	} catch (const std::exception &p_except) {
		endwin();

		std::cerr << "\x1b[0mCould not clean up due to a \x1b[1;91mfatal internal error\x1b[0m.\n"
		          << std::endl;
	}

	std::cerr << "\x1b[0mProgram ended due to an \x1b[1;91minternal error\x1b[0m,\n"
	             "please report this to the developers.\n"
	             "(\x1b[4;92mhttps://github.com/lordoftrident/pona\x1b[0m)\n";
	std::cerr << "\n\x1b[1;91mThrown std::" + p_class << '\n';
	std::cerr << "what():\x1b[0m " << p_what << std::endl;

	std::exit(EXIT_FAILURE);
}

void HelpUsage() {
	std::cout << "Usage: pona [OPTION] [FILE...]\n"
	             "-h, --h, --help, -?   Show this usage help\n"
	             "\nFor help with navigating around the editor\n"
	             "press CTRL+H inside the editor.\n"
	             "(\x1b[4;92mhttps://github.com/lordoftrident/pona\x1b[0m)" << std::endl;
}

int main(int p_argc, const char *p_argv[]) {
	// pass in the arguments but skip the first one (its always the program path)
	std::vector<std::string> args(p_argv + 1, p_argv + p_argc);

	for (const auto &arg : args) {
		if (arg == "-h" or arg == "--h" or arg == "--help" or arg == "-?") {
			HelpUsage();

			return 0;
		}
	}

	try {
		Init(args, "Pona");
		Run();
		Finish();
	} catch (const std::runtime_error &p_except) {
		FailureExit("runtime_error", p_except.what());
	} catch (const std::exception &p_except) {
		FailureExit("exception", p_except.what());
	} catch (...) {
		FailureExit("UNKNOWN", "...");
	}

	return 0;
}

