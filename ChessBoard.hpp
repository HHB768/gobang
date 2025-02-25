#ifndef __CHESSBOARD_HPP__
#define __CHESSBOARD_HPP__

#include "utils.hpp"


namespace mfwu {

class Player;

class ChessBoard_base {
public:
    static constexpr bool Black = Piece::Black;
    static constexpr bool White = Piece::White;

    ChessBoard_base() {}
    virtual ~ChessBoard_base() {}

    void update(Position pos) {
        return ;
    }

    Position last_piece_;
    bool status_;

};  // endof class ChessBoard_base

template <BoardSize Size=BoardSize::Small>
class ChessBoard : ChessBoard_base {
public:
    ChessBoard() 
        : board_(Size, std::vector<std::unique_ptr<Postion>>(Size, nullptr)) {
        for (size_t i = 0; i < static_cast<size_t>(Size); i++) {
            for (size_t j = 0; j < static_cast<size_t>(Size); j++) {
                board_[i][j] = std::make_unique<Position>(i, j);
            }
        }
    }
    ChessBoard(const std::vector<std::vector<std::unique_ptr<Position>>>) 
        : board_(Size, std::vector<std::unique_ptr<Postion>>(Size, nullptr)) {
        for (size_t i = 0; i < static_cast<size_t>(Size); i++) {
            for (size_t j = 0; j < static_cast<size_t>(Size); j++) {
                board_[i][j] = std::make_unique<Position>(i, j);
            }
        }
    }
    ~ChessBoard() {

    }
    


private:
    

    std::vector<std::vector<std::unique_ptr<Position>>> board_;
    
};  // endof class ChessBoard

}  // endof namespace mfwu

#endif  // __CHESSBOARD_HPP__