#ifndef __CONSTDEF_HPP__
#define __CONSTDEF_HPP__

#include <iostream>
#include "Logger.hpp"

namespace mfwu {

constexpr const size_t NUM_OF_PIECE_TO_WIN = 5;
constexpr const size_t NoPtW = NUM_OF_PIECE_TO_WIN;
constexpr const size_t NOPTW = NUM_OF_PIECE_TO_WIN;

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
// ---------------------------------------------------------------------------

// error messages
constexpr const char* ERROR_NEW_GC = "An error occurs when we new GameController()";
constexpr const char* ERROR_UNKNOWN_COMMAND_TYPE = "Unknown CommandType by HumanPlayer";
constexpr const char* ERROR_UNKNOWN_PIECE_STATUS = "Unknown piece status";
constexpr const char* ERROR_UNKNOWN_GAME_STATUS  = "Unknown game status";

// logerr shortcuts
#define gc_error_exit(mode, size) do {  \
    log_error(ERROR_NEW_GC);  \
    log_error(XQ4GB_TIMESTAMP, "mode: %ld, size: %ld", \
              static_cast<size_t>(mode), static_cast<size_t>(size));  \
    exit(-10086);  \
} while (0);
inline void logerr_unknown_cmdtype()      { log_error(ERROR_UNKNOWN_COMMAND_TYPE); }
inline void logerr_unknown_piece_status() { log_error(ERROR_UNKNOWN_PIECE_STATUS); }
inline void logerr_unknown_game_status()  { log_error(ERROR_UNKNOWN_GAME_STATUS);  }
// -------------------------------------------------------------------------

// warn messages
constexpr const char* WARN_MULTIPLE_SP = "Multiple sp piece applied on the board";
constexpr const char* WARN_INVALID_POS = "Invalid pos is found when init the board";

// logwarn shortcuts
inline void logwarn_multiple_sp(int i, int j) { 
    log_warn(WARN_MULTIPLE_SP); 
    log_warn(XQ4GB_TIMESTAMP, 
             "Position [%d, %d] init failed: MULTIPLE SP PIECE",
             i, j);
    log_warn(XQ4GB_TIMESTAMP, "Init as real-color piece");
}
inline void logwarn_invalid_pos(int i, int j) { 
    log_warn(WARN_INVALID_POS);
    log_warn(XQ4GB_TIMESTAMP, 
             "Position [%d, %d] init failed: UNKOWN PIECE COLOR",
             i, j);
    log_warn(XQ4GB_TIMESTAMP, "Init as empty position"); 
}

// constexpr const char* CMD_CLEAR = "clear screen\n";
constexpr const char* CMD_CLEAR = "\033[2J\033[1;1H";
inline void cmd_clear() { std::cout << CMD_CLEAR; }

constexpr const time_t XQ4GB_TIMESTAMP = 1741352220;

}  // endof namespace mfwu
#endif  // __CONSTDEF_HPP__