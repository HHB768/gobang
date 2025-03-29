#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <bits/stdc++.h>  // temp
#include "constdef.hpp"

namespace mfwu {
    enum class BoardSize : size_t {
        Small  = 13,
        Middle = 21,
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

        bool operator==(const Position& p) {
            return row == p.row && col == p.col;
        }
        bool operator!=(const Position& p) {
            return !(*this == p);
        }
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
        static size_t get_real_status(Color a) {
            return get_real_status(static_cast<size_t>(a));
        }
        size_t get_real_status() const {
            return get_real_status(color);
        }
        static size_t get_real_status(size_t ca) {
            if (ca == 2 || ca == 4) return ca - 1;  // 👈😁 急了，自己写的 enum class 现在又懒得用了
                                                    // 你就直接写数字吧，到时出神秘 bug 一出一个不吱声
                                                    // XQX 25.03.28
            return ca;
        }
        static size_t get_op_real_status(Color a) {
            return get_op_real_status(static_cast<size_t>(a));
        }
        static size_t get_op_real_status(size_t ca) {
            size_t color = get_real_status(ca);
            return color == static_cast<size_t>(Color::Black) ?
                            static_cast<size_t>(Color::White) :
                            static_cast<size_t>(Color::Black) ;
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

        bool operator==(const Piece& p) const {
            return    this->row == p.row 
                   && this->col == p.col 
                   && this->color == p.color;
        }
        bool operator!=(const Piece& p) const {
            return !(*this == p);
        }
    };  // endof struct Piece
    const Piece invalid_piece = Piece{-1, -1, Piece::Color::Invalid};

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

    const std::vector<std::pair<int, int>> half_dirs = {
        {1, 0}, {0, 1}, {1, 1}, {1, -1}
    };
    const std::vector<size_t> score_map = {
        1048576,
        32768,
        32,
        20,
        12,
        9,
        8,
        0
    };

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
        INVALID = 4,
        XQ4GB = 5
    };  // endof enum class CommandType
    const std::unordered_map<size_t, std::string> CommandTypeDescription = {
        {0, "PIECE"}, {1, "RESTART"}, {2, "MENU"},
        {3, "QUIT"}, {4, "INVALID"}, {5, "XQ4GB"}
    };
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
        INVALID = 4,
        XQ4GB = 5
    };  // endof enum class GameStatus
    const std::unordered_map<size_t, std::string> GameStatusDescription = {
        {0, "NORMAL"}, {1, "RESTART"}, {2, "MENU"},
        {3, "QUIT"}, {4, "INVALID"}, {5, "XQ4GB"}
    };

    inline bool is_digit(char c) {
        return c <= '9' and c >= '0';
    }
    inline bool is_uppercase(char c) {
        return c <= 'Z' and c >= 'A';
    }
    inline bool is_lowercase(char c) {
        return c <= 'z' and c >= 'a';
    }
    inline void tolower(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    }
    inline void toupper(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        // wrong usage:
        // std::string ret; ret.reserve(str.size());
        // std::transform(str.begin(), str.end(), ret.begin(), ::toupper);
        // return ret;
        // reason: ret.begin() == ret.end();
    }

    inline GameMode print_mode_choice_help_cmd() {
        cmd_clear();
        std::cout << HELPER_SELECT_MODE << "\n";
        std::string gamemode;
        signed char mode = -1;
        while (mode < 0) {
            std::cin >> gamemode;
            if (gamemode.size() == 0) {
                return GameMode::PVE;
            }
            if (gamemode.size() >= 2) {
                std::cout << HELPER_INVALIDMODE_1 << "\n";
                mode = -1; continue;
            }
            mode = gamemode[0];
            if (is_digit(mode)) {
                mode -= '0';
            } else if (is_uppercase(mode)) {
                mode -= 'A' - 1;
            } else if (is_lowercase(mode)) {
                mode -= 'a' - 1;
            } else {
                std::cout << HELPER_INVALIDMODE_2 << "\n";
                mode = -2; continue;
            }
            
            switch (mode) {
                case 1 : return GameMode::PVE;
                case 2 : return GameMode::PVP;
                case 3 : return GameMode::EVE;
                default : {
                    std::cout << HELPER_INVALIDMODE_3 << "\n";
                    mode = -3; continue;
                }
            }
        }
        
        return GameMode::EVE;
    }
    
    inline BoardSize print_size_choice_help_cmd() {
        cmd_clear();
        std::cout << HELPER_SELECT_SIZE << "\n";
        std::string boardsize;
        signed char size = -1;
        while (size < 0) {
            std::cin >> boardsize;
            if (boardsize.size() == 0) {
                return BoardSize::Small;
            }
            if (boardsize.size() >= 2) {
                std::cout << HELPER_INVALIDSIZE_1 << "\n";
                size = -1; continue;
            }
            size = boardsize[0];
            if (is_digit(size)) {
                size -= '0';
            } else if (is_uppercase(size)) {
                size -= 'A' - 1;
            } else if (is_lowercase(size)) {
                size -= 'a' - 1;
            } else {
                std::cout << HELPER_INVALIDSIZE_2 << "\n";
                size = -2; continue;
            }
            switch (size) {
                case 1 : return BoardSize::Small;
                case 2 : return BoardSize::Middle;
                case 3 : return BoardSize::Large;
                default : {
                    std::cout << HELPER_INVALIDSIZE_3 << "\n";
                    size = -3; continue;
                }
            }
        }
    
        return BoardSize::Large;
    }
    
    inline GameMode print_mode_choice_help_gui() {
        cmd_clear();
    
        return GameMode::PVE;
    }
    
    inline BoardSize print_size_choice_help_gui() {
        cmd_clear();
    
        return BoardSize::Small;
    }

    inline void append_time_info(std::string& str) {
        time_t now = time(0);
        tm* lt = localtime(&now);
        str += std::to_string(1900 + lt->tm_year);
            str += '-'; str += std::to_string(1 + lt->tm_mon);
            str += '-'; str += std::to_string(lt->tm_mday);
            str += '_'; str += std::to_string(lt->tm_hour);
            str += 'h'; str += std::to_string(lt->tm_min);
            str += 'm'; str += std::to_string(lt->tm_sec); 
            str += 's';
    }
}  // endof namespace mfwu


#endif  // __UTILS_HPP__