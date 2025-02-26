#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <bits/stdc++.h>  // temp

namespace mfwu {
    enum class BoardSize : size_t {
        Small  = 12,
        Middle = 20,
        Large  = 25
    };  // endof enum class BoardSize
    
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

        void update(int r, int c, Color clr) {
            this->row = r;
            this->col = c;
            this->color = clr;
        }
        size_t get_status() const { return static_cast<size_t>(color); }
        
        Piece(int r, int c, Color clr) 
            : Position(r, c), color(clr) {}
        Piece(const Position& pos, Color clr)
            : Position(pos), color(clr) {}
        virtual ~Piece() {}
    };  // endof struct Piece

}  // endof namespace mfwu


#endif  // __UTILS_HPP__