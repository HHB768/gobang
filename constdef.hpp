#ifndef __CONSTDEF_HPP__
#define __CONSTDEF_HPP__

#include <iostream>

namespace mfwu {

constexpr const size_t NUM_OF_PIECE_TO_WIN = 5;
constexpr const size_t NoPtW = NUM_OF_PIECE_TO_WIN;
constexpr const size_t NOPTW = NUM_OF_PIECE_TO_WIN;

constexpr const size_t INFERENCE_DEPTH = 3;
constexpr const size_t DEDUCTION_DEPTH = INFERENCE_DEPTH;

// special commands to control games in cmd mode
constexpr const char* QUIT_CMD1 = "\\QUIT";
constexpr const char* QUIT_CMD2 = "\\Q";
constexpr const char* QUIT_CMD3 = "\\quit";
constexpr const char* RESTART_CMD1 = "\\RESTART";
constexpr const char* RESTART_CMD2 = "\\R";
constexpr const char* RESTART_CMD3 = "\\restart";
constexpr const char* MENU_CMD1 = "\\MENU";
constexpr const char* MENU_CMD2 = "\\M";
constexpr const char* MENU_CMD3 = "\\menu";
constexpr const char* XQ4GB_CMD = "\\XQ4GB";
// -----------------------------------------

// helpers for users
constexpr const char* HELPER_RETURN2MENU = "Key in \\RESTART or \\MENU or \\QUIT if you want";
constexpr const char* HELPER_PLACE_PIECE = "Key in a pair of character to play, \n"
                                           "e.g., AB for the first row & the second col";
constexpr const char* HELPER_SELECT_MODE = "Plz key in your game mode: \n"
                                           "A.1. PVE (default), B.2. PVP, C.3. EVE";
constexpr const char* HELPER_SELECT_SIZE = "Plz key in your scale of board: \n"
                                           "A.1. Small (default), B.2. Middle, C.3. Large";
constexpr const char* HELPER_PRESS_ANY_KEY = "Press any key to continue...";

constexpr const char* HELPER_INVALIDMODE_1 = "Invalid mode selection: input includes multiple chars T.T \n"
                                             "Just key in A/B/C or 1/2/3";
constexpr const char* HELPER_INVALIDMODE_2 = "Invalid mode selection: input is not a digit or character >.< \n"
                                             "Just key in A/B/C or 1/2/3";
constexpr const char* HELPER_INVALIDMODE_3 = "Invalid mode selection: input is not in alternative options -.- \n"
                                             "Just key in A/B/C or 1/2/3";
constexpr const char* HELPER_INVALIDSIZE_1 = "Invalid size selection: input includes multiple chars T.T \n"
                                             "Just key in A/B/C or 1/2/3";
constexpr const char* HELPER_INVALIDSIZE_2 = "Invalid size selection: input is not a digit or character >.< \n"
                                             "Just key in A/B/C or 1/2/3";
constexpr const char* HELPER_INVALIDSIZE_3 = "Invalid size selection: input is not in alternative options -.- \n"
                                             "Just key in A/B/C or 1/2/3";
constexpr const char* HELPER_OCCUPIED_POSITION = "invalid position: occupied";
constexpr const char* HELPER_INVALID_POSITION  = "invalid piece position, plz try again";
// ---------------------------------------------------------------------------

// error messages
constexpr const char* ERROR_NEW_GC = "An error occurs when we new GameController()";
constexpr const char* ERROR_UNKNOWN_COMMAND_TYPE = "Unknown CommandType by HumanPlayer";
constexpr const char* ERROR_UNKNOWN_PIECE_STATUS = "Unknown piece status";
constexpr const char* ERROR_UNKNOWN_GAME_STATUS  = "Unknown game status";

// -------------------------------------------------------------------------

// warn messages
constexpr const char* WARN_MULTIPLE_SP = "Multiple sp piece applied on the board";
constexpr const char* WARN_INVALID_POS = "Invalid pos is found when init the board";


// constexpr const char* CMD_CLEAR = "clear screen\n";
constexpr const char* CMD_CLEAR = "\033[2J\033[1;1H";
inline void cmd_clear() { std::cout << CMD_CLEAR; }

constexpr const time_t XQ4GB_TIMESTAMP = 1741352220;
constexpr const float eps = 0.01F;

}  // endof namespace mfwu
#endif  // __CONSTDEF_HPP__