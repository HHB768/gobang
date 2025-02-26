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
    
    struct Piece : Position {
        enum class Color {
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

    constexpr std::vector<std::pair<int, int>> dirs = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };
    constexpr std::pair<int, int> dir_right = dirs[0];
    constexpr std::pair<int, int> dir_down  = dirs[1];
    constexpr std::pair<int, int> dir_left  = dirs[2];
    constexpr std::pair<int, int> dir_up    = dirs[3];
    constexpr std::pair<int, int> dir_down_right = dirs[4];
    constexpr std::pair<int, int> dir_down_left  = dirs[5];
    constexpr std::pair<int, int> dir_up_rihgt   = dirs[6];
    constexpr std::pair<int, int> dir_up_left    = dirs[7];
}  // endof namespace mfwu


#endif  // __UTILS_HPP__