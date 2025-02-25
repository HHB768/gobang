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
    
        virtual ~Position() {}
    };  // endof struct Position
    
    struct Piece : Position {
        static constexpr bool Black = 1;
        static constexpr bool White = 0;
    
        bool color;
        
        virtual ~Piece() {}
    };  // endof struct Piece

}  // endof namespace mfwu


#endif  // __UTILS_HPP__