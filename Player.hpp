#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include "utils.hpp"
#include "ChessBoard.hpp"

namespace mfwu {

class ChessBoard_base;

class Player {
public:
    virtual void play() = 0;

    virtual void place(Position pos) {
        board_->update(pos);
    }

protected:
    std::shared_ptr<ChessBoard_base> board_;
};  // endof class Player


}  // endof namespace mfwu

#endif  // __PLAYER_HPP__