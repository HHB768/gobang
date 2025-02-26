#ifndef __CHESSBOARD_HPP__
#define __CHESSBOARD_HPP__

#include "utils.hpp"


namespace mfwu {

class Player;

class ChessBoard_base {
public:
    using Color = typename Piece::Color;
    static constexpr Color Invalid = Color::Invalid;
    static constexpr Color Black = Color::Black;
    static constexpr Color White = Color::White;
    static constexpr Color BlackSp = Color::BlackSp;
    static constexpr Color WhiteSp = Color::WhiteSp;

    ChessBoard_base() {}
    virtual ~ChessBoard_base() {}

    virtual void update(Position pos) = 0;
protected:
    Position last_piece_;
    bool status_;
};  // endof class ChessBoard_base

template <BoardSize Size=BoardSize::Small>
class ChessBoard : ChessBoard_base {
public:
    ChessBoard() 
        : board_(Size, std::vector<std::unique_ptr<Postion>>(Size, nullptr)) {
        _init_board();
    }
    ChessBoard(const std::vector<std::vector<size_t>>> input_board) 
        : board_(Size, std::vector<std::unique_ptr<Postion>>(Size, nullptr)) {
        for (size_t i = 0; i < static_cast<size_t>(Size); i++) {
            for (size_t j = 0; j < static_cast<size_t>(Size); j++) {
                switch (input_board[i][j]) {
                case static_cast<size_t>(Color::Invalid) : {
                    board_[i][j] = std::make_unique<Position>(i, j);
                } break;
                case static_cast<size_t>(Color::White) : {
                    board_[i][j] = std::make_unique<Position>(Piece(i, j, Color::White));
                } break;
                case static_cast<size_t>(Color::Black) : {
                    board_[i][j] = std::make_unique<Position>(Piece(i, j, Color::Black));
                } break;
                case static_cast<size_t>(Color::BlackSp) : {
                    board_[i][j] = std::make_unique<Position>(Piece(i, j, Color::BlackSp));
                } break;
                case static_cast<size_t>(Color::WhiteSp) : {
                    board_[i][j] = std::make_unique<Position>(Piece(i, j, Color::WhiteSp));
                } break;
                default:
                    std::cout << "Position [" << i << ", " << j << "] init failed: UNKOWN PIECE COLOR\n"
                    // TODO: LOG IT
                    board_[i][j] = std::make_unique<Position>(i, j);
                }
            }
        }
    }
    ChessBoard(const ChessBoard& board) = delete;
    ChessBoard(ChessBoard&& board) = default;

    ChessBoard operator=(const ChessBoard& board) = delete;
    ChessBoard operator=(ChessBoard&& board) = default;

    ~ChessBoard() {}
    
    void reset() {
        _init_board();
    }

    void update(const Piece& piece) {
        board_[piece.row][piece.row] 
            = std::make_unique<Position>(piece);
    }

    void print_board() const {
        for (size_t i = 0; i < static_cast<size_t>(Size); i++) {
            for (size_t j = 0; j < static_cast<size_t>(Size); j++) {
                std::cout << board_[i][j]->get_status() << " ";
            }
            std::cout << "\n";
        }
    }
    
private:
    void _init_board() {
        for (size_t i = 0; i < static_cast<size_t>(Size); i++) {
            for (size_t j = 0; j < static_cast<size_t>(Size); j++) {
                board_[i][j] = std::make_unique<Position>(i, j);
            }
        }
    }    

    std::vector<std::vector<std::unique_ptr<Position>>> board_;
    
};  // endof class ChessBoard

template <BoardSize Size=BoardSize::Small>
class GUIBoard : ChessBoard<Size> {

};  // endof class GUIBoard

template <BoardSize Size=BoardSize::Small>
class CMDBoard : ChessBoard<Size> {

};  // endof class CMDBoard

}  // endof namespace mfwu

#endif  // __CHESSBOARD_HPP__