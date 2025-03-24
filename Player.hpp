#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include "utils.hpp"
#include "ChessBoard.hpp"

namespace mfwu {

class ChessBoard_base;

class Player {
public:
    Player() : board_(nullptr) {}
    Player(std::shared_ptr<ChessBoard_base> board, Piece::Color color) 
        : board_(board), player_color_(color) {}

    virtual CommandType play() = 0;

    virtual void place(const Position& pos) {
        place(Piece(pos, this->player_color_));
    }

    Piece::Color& get_color() {
        return player_color_;
    }

    const Piece::Color& get_color_const() const {
        return player_color_;
    }

protected:
    std::shared_ptr<ChessBoard_base> board_;  // i dont want unexpected modifications
                                              // but cannnot set const chessboard
    Piece::Color player_color_;

private:
    void place(const Piece& piece) {
        this->board_->update(piece);
    }
};  // endof class Player


}  // endof namespace mfwu

#endif  // __PLAYER_HPP__