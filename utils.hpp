#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <bits/stdc++.h>  // temp

namespace mfwu {
    enum class BoardSize : size_t {
        Small  = 12,
        Middle = 20,
        Large  = 25
    };  // endof enum class BoardSize

    /* dual ints, -1 for invalid res */
    struct Position {
        int row, col;

        Position() : row(-1), col(-1) {}
        Position(int r, int c) : row(r), col(c) {}
        Position(const Position& pos) = default;

        void update(int r, int c) {
            row = r;
            col = c;
        }
        virtual size_t get_status() const { return 0; }
    
        virtual ~Position() {}
    };  // endof struct Position
    
    struct Piece : public Position {
        enum class Color : size_t {
            Invalid = 0,
            White   = 1,
            WhiteSp = 2,
            Black   = 3,
            BlackSp = 4
        };  // endof struct Color
        
        Color color;

        static bool is_same_color(Color a, Color b) {
            return is_same_color(static_cast<size_t>(a),
                                 static_cast<size_t>(b));
        }
        static bool is_same_color(size_t ca, size_t cb) {
            if (ca == 1 && cb == 2) return true;
            if (ca == 2 && cb == 1) return true;
            if (ca == 3 && cb == 4) return true;
            if (ca == 4 && cb == 3) return true;
            return ca == cb;
        }
        static size_t get_real_color(Color a) {
            return get_real_color(static_cast<size_t>(a));
        }
        static size_t get_real_color(size_t ca) {
            if (ca == 2 || ca == 4) return ca - 1;
            return ca;
        }

        void update(int r, int c, Color clr) {
            this->row = r;
            this->col = c;
            this->color = clr;
        }
        size_t get_status() const { return static_cast<size_t>(color); }
        
        Piece() : Position(), color(Color::Invalid) {}
        Piece(int r, int c, Color clr) 
            : Position(r, c), color(clr) {}
        Piece(const Position& pos, Color clr)
            : Position(pos), color(clr) {}
        virtual ~Piece() {}
    };  // endof struct Piece

    const std::vector<std::pair<int, int>> dirs = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };
    const std::pair<int, int> dir_right = dirs[0];
    const std::pair<int, int> dir_down  = dirs[1];
    const std::pair<int, int> dir_left  = dirs[2];
    const std::pair<int, int> dir_up    = dirs[3];
    const std::pair<int, int> dir_down_right = dirs[4];
    const std::pair<int, int> dir_down_left  = dirs[5];
    const std::pair<int, int> dir_up_rihgt   = dirs[6];
    const std::pair<int, int> dir_up_left    = dirs[7];

    struct count_res_4 {
        int left_right, up_down;
        int up_left_down_right, up_right_down_left;
    };  // endof struct count_res_4
    struct count_res_8 {
        int right, down, left, up;
        int down_right, down_left, up_right, up_left;
    };  // endof struct count_res_8

    enum class GameMode : size_t {
        PVE = 0,
        PVP = 1,
        EVE = 2
    };  // endof enum class GameMode

    enum class CommandType : size_t {
        PIECE = 0,
        RESTART = 1,
        MENU = 2,
        QUIT = 3,
        INVALID = 4
    };  // endof enum class CommandType
    struct Command {
        CommandType type;
        Position pos;
    };  // endof struct Command

    // constexpr char*
    enum class GameStatus : size_t {
        NORMAL = 0,
        RESTART = 1,
        MENU = 2,
        QUIT = 3,
        INVALID = 4
    };  // endof enum class GameStatus

    constexpr const char* QUIT_CMD1 = "\\QUIT";
    constexpr const char* QUIT_CMD2 = "\\Q";
    constexpr const char* QUIT_CMD3 = "\\quit";
    constexpr const char* RESTART_CMD1 = "\\RESTART";
    constexpr const char* RESTART_CMD2 = "\\R";
    constexpr const char* RESTART_CMD3 = "\\restart";
    constexpr const char* MENU_CMD1 = "\\MENU";
    constexpr const char* MENU_CMD2 = "\\M";
    constexpr const char* MENU_CMD3 = "\\menu";
    constexpr const char* CMD_HELPER = "Key in \\RESTART or \\MENU or \\QUIT if you want";
    constexpr const char* INPUT_HELPER = "Key in a pair of character to play, e.g., AB for the first row & the second col";
    constexpr const char* NEW_GC_ERROR = "An error occurs when we new GameController()";
    constexpr const char* CMD_CLEAR = "clear screen\n";  // \033[2J\033[1;1H
    void cmd_clear() { std::cout << CMD_CLEAR; }
    constexpr const char* UNKNOWN_PIECE_STATUS = "Unknown piece status";
    constexpr const char* UNKNOWN_COMMAND_TYPE = "Unknown CommandType by HumanPlayer";
    constexpr const char* MODE_SELECTION_HELPER = "Plz key in your game mode: A.1. PVE, B.2. PVP, C.3. EVE, "
                                                  "default: A";
    constexpr const char* SIZE_SELECTION_HELPER = "Plz key in your scale of board: A.1 Small, B.2 Middle, C.3. Large, "
                                                  "default: A";


    bool is_digit(char c) {
        return c <= '9' and c >= '0';
    }
    bool is_uppercase(char c) {
        return c <= 'Z' and c >= 'A';
    }
    bool is_lowercase(char c) {
        return c <= 'z' and c >= 'a';
    }

    GameMode print_mode_choice_help_cmd() {
        cmd_clear();
        std::cout << MODE_SELECTION_HELPER << "\n";
        std::string gamemode;
        signed char mode = -1;
        while (mode < 0) {
            std::cin >> gamemode;
            if (gamemode.size() == 0) {
                return GameMode::PVE;
            }
            mode = gamemode[0];
            if (is_digit(mode)) {
                mode -= '0';
            } else if (is_uppercase(mode)) {
                mode -= 'A' - 1;
            } else if (is_lowercase(mode)) {
                mode -= 'a' - 1;
            } else {
                std::cout << "INVALID MODE SELECTION\n";
                mode = -1; continue;
            }
            switch (mode) {
                case 1 : return GameMode::PVE;
                case 2 : return GameMode::PVP;
                case 3 : return GameMode::EVE;
                default : {
                    mode = -2;
                    std::cout << "Key in 1/2/3 or A/B/C plz\n";
                }
            }
        }
        
        return GameMode::EVE;
    }
    
    BoardSize print_size_choice_help_cmd() {
        cmd_clear();
        std::cout << SIZE_SELECTION_HELPER << "\n";
        std::string boardsize;
        signed char size = -1;
        while (size < 0) {
            std::cin >> boardsize;
            if (boardsize.size() == 0) {
                return BoardSize::Small;
            }
            size = boardsize[0];
            if (is_digit(size)) {
                size -= '0';
            } else if (is_uppercase(size)) {
                size -= 'A' - 1;
            } else if (is_lowercase(size)) {
                size -= 'a' - 1;
            } else {
                std::cout << "INVALID SIZE SELECTION\n";
                size = -1; continue;
            }
            switch (size) {
                case 1 : return BoardSize::Small;
                case 2 : return BoardSize::Middle;
                case 3 : return BoardSize::Large;
                default : {
                    size = -2;
                    std::cout << "Key in 1/2/3 or A/B/C plz\n";
                }
            }
        }
    
        return BoardSize::Large;
    }
    
    GameMode print_mode_choice_help_gui() {
        cmd_clear();
    
        return GameMode::PVE;
    }
    
    BoardSize print_size_choice_help_gui() {
        cmd_clear();
    
        return BoardSize::Small;
    }

}  // endof namespace mfwu


#endif  // __UTILS_HPP__