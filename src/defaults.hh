#ifndef DEFAULTS_HH__HEADER_GUARD__
#define DEFAULTS_HH__HEADER_GUARD__

#define DEFAULT_HELP { \
		"# Pona help file", \
		"> [github link](https://github.com/LordOfTrident/pona)", \
		"", \
		"## Quick-start", \
		"- `^Q`     - Quit the editor/close a tab", \
		"- `^E`     - Open the command line", \
		"- `^S`     - Save the current file", \
		"- `^O`     - Open a file", \
		"- `ALT+.`  - Next tab", \
		"- `ALT+,`  - Previous tab", \
		"", \
		"> `^<KEY>` - Ctrl+<KEY>", \
		"", \
		"## Other keybinds", \
		"- `ALT+E`  - Go to the end of the line", \
		"- `^A`     - Select everything", \
		"- `PPAGE,` - Scroll up", \
		"- `NPAGE,` - Scroll down", \
		"", \
		"## Info bar", \
		"It is the bottom bar under your editor window. This bar contains some basic info about", \
		"the file. Lets take it apart.", \
		"", \
		"> ^H | ASCII | LF | RW | txt", \
		"", \
		"This is an example of some basic info the bar might show.", \
		"- `^H`    is telling you what to press when you are lost", \
		"- `ASCII` shows what encoding you are editing in", \
		"- `LF`    is the symbols used for new lines (on windows, its usually CRLF)", \
		"- `RW`    tells you whether the file is for both writing, reading (RW) or reading only (R)", \
		"- `txt`   shows the file type", \
		"", \
		"## List of commands", \
		"- `help` (`^H`)               - Show the pona help", \
		"- `ruler <bool:on>`           - Enable/disable the ruler", \
		"- `scroll_bar <bool:on>`      - Enable/disable the scroll bar", \
		"- `save_config`               - Save the config as it is", \
		"- `reload_config`             - Reload the config file", \
		"- `defaults`                  - Set the config back to defaults", \
		"- `theme` <std:name>          - Set the theme (all themes in ~/.config/pona/themes)", \
		"- `indent_size` <int:size>    - Set the indent size (min 1, max 16)", \
		"- `mark_column` <int:column>  - Mark the column", \
		"- `exit`                      - Exit the editor", \
		"- `new` [str:title...] (^T)   - Open new tab(s)", \
		"- `close` (^Q)                - Close the current tab", \
		"- `save` (^S)                 - Save the current file", \
		"- `save_as` <str:name>        - Save the current file as ...", \
		"- `open` <str:name...> (^O)   - Open file(s)", \
		"- `goto` <int:line> (^L)      - Go to a line", \
		"- `find` <str:text> (^F)      - Find the string in the file", \
		"- `find_next` (^N)            - TODO: Implement", \
		"- `replace` (^R)              - TODO: Implement", \
		"- `replace_next` (^N)         - TODO: Implement", \
		"- `replace_all`               - TODO: Implement", \
		"- `trim`                      - TODO: Implement", \
		"- `trim_ws_on_save` <bool:on> - Enable/disable trimming trailing whitespaces on save", \
		"", \
		"> `[]` - Optional parameter", \
		"> `<>` - Required parameter", \
		"> `...` - Can be repeated any amount of times", \
		"> `str` - String parameter", \
		"> `bool` - Boolean parmeter (0/1)", \
		"> `int` - Integer parmeter" \
	}

#define DEFAULT_THEME_DEFAULT { \
		"[top_bar]", \
		"fg = black", \
		"bg = white", \
		"", \
		"input_fg = white", \
		"input_bg = blue", \
		"error_fg = yellow", \
		"error_bg = red", \
		"", \
		"tab_fg         = black", \
		"tab_bg         = white", \
		"tab_current_fg = blue", \
		"tab_current_bg = white", \
		"", \
		"list_fg         = black", \
		"list_bg         = white", \
		"list_current_fg = blue", \
		"list_current_bg = white", \
		"", \
		"[editor]", \
		"fg         = white", \
		"bg         = black", \
		"current_fg = white", \
		"current_bg = black", \
		"", \
		"ruler_fg         = yellow", \
		"ruler_bg         = black", \
		"ruler_current_fg = white", \
		"ruler_current_bg = black", \
		"", \
		"marked_column_fg = red", \
		"marked_column_bg = black", \
		"", \
		"selection_fg = black", \
		"selection_bg = white", \
		"", \
		"[scroll_bar]", \
		"fg            = cyan", \
		"bg            = blue", \
		"button_fg     = cyan", \
		"button_bg     = blue", \
		"button_alt_fg = white", \
		"button_alt_bg = blue", \
		"back_fg       = black", \
		"back_bg       = blue", \
		"", \
		"[info_bar]", \
		"fg = black", \
		"bg = white", \
		"", \
		"separator_fg = black", \
		"separator_bg = white" \
	}

#define DEFAULT_THEME_CLASSIC { \
		"[top_bar]", \
		"fg = black", \
		"bg = white", \
		"", \
		"input_fg = white", \
		"input_bg = black", \
		"error_fg = yellow", \
		"error_bg = red", \
		"", \
		"tab_fg         = black", \
		"tab_bg         = white", \
		"tab_current_fg = black", \
		"tab_current_bg = green", \
		"", \
		"list_fg         = black", \
		"list_bg         = white", \
		"list_current_fg = black", \
		"list_current_bg = green", \
		"", \
		"[editor]", \
		"fg         = white", \
		"bg         = blue", \
		"current_fg = white", \
		"current_bg = blue", \
		"", \
		"ruler_fg         = yellow", \
		"ruler_bg         = black", \
		"ruler_current_fg = white", \
		"ruler_current_bg = black", \
		"", \
		"marked_column_fg = red", \
		"marked_column_bg = blue", \
		"", \
		"selection_fg = blue", \
		"selection_bg = white", \
		"", \
		"[scroll_bar]", \
		"fg            = cyan", \
		"bg            = cyan", \
		"button_fg     = blue", \
		"button_bg     = cyan", \
		"button_alt_fg = blue", \
		"button_alt_bg = green", \
		"back_fg       = cyan", \
		"back_bg       = blue", \
		"", \
		"[info_bar]", \
		"fg = black", \
		"bg = white", \
		"", \
		"separator_fg = black", \
		"separator_bg = white" \
	}

#define DEFAULT_THEME_TILDE { \
		"[top_bar]", \
		"fg = black", \
		"bg = cyan", \
		"", \
		"input_fg = white", \
		"input_bg = black", \
		"error_fg = red", \
		"error_bg = black", \
		"", \
		"tab_fg         = black", \
		"tab_bg         = cyan", \
		"tab_current_fg = white", \
		"tab_current_bg = black", \
		"", \
		"list_fg         = black", \
		"list_bg         = white", \
		"list_current_fg = white", \
		"list_current_bg = black", \
		"", \
		"[editor]", \
		"fg         = white", \
		"bg         = blue", \
		"current_fg = white", \
		"current_bg = blue", \
		"", \
		"ruler_fg         = yellow", \
		"ruler_bg         = blue", \
		"ruler_current_fg = white", \
		"ruler_current_bg = blue", \
		"", \
		"marked_column_fg = cyan", \
		"marked_column_bg = blue", \
		"", \
		"selection_fg = blue", \
		"selection_bg = white", \
		"", \
		"[scroll_bar]", \
		"fg            = black", \
		"bg            = black", \
		"button_fg     = white", \
		"button_bg     = black", \
		"button_alt_fg = cyan", \
		"button_alt_bg = black", \
		"back_fg       = white", \
		"back_bg       = black", \
		"", \
		"[info_bar]", \
		"fg = black", \
		"bg = cyan", \
		"", \
		"separator_fg = blue", \
		"separator_bg = cyan" \
	}

#define DEFAULT_THEME_16 { \
		"[top_bar]", \
		"fg = black", \
		"bg = cyan", \
		"", \
		"input_fg = white", \
		"input_bg = default", \
		"error_fg = red", \
		"error_bg = default", \
		"", \
		"tab_fg         = black", \
		"tab_bg         = cyan", \
		"tab_current_fg = black", \
		"tab_current_bg = bright_cyan", \
		"", \
		"list_fg         = black", \
		"list_bg         = white", \
		"list_current_fg = black", \
		"list_current_bg = bright_white", \
		"", \
		"[editor]", \
		"fg         = default", \
		"bg         = default", \
		"current_fg = default", \
		"current_bg = grey", \
		"", \
		"ruler_fg         = grey", \
		"ruler_bg         = default", \
		"ruler_current_fg = white", \
		"ruler_current_bg = grey", \
		"", \
		"marked_column_fg = grey", \
		"marked_column_bg = default", \
		"", \
		"selection_fg = black", \
		"selection_bg = white", \
		"", \
		"[scroll_bar]", \
		"fg            = black", \
		"bg            = grey", \
		"button_fg     = black", \
		"button_bg     = green", \
		"button_alt_fg = black", \
		"button_alt_bg = cyan", \
		"back_fg       = black", \
		"back_bg       = default", \
		"", \
		"[info_bar]", \
		"fg = black", \
		"bg = green", \
		"", \
		"separator_fg = grey", \
		"separator_bg = green" \
	}

#endif
