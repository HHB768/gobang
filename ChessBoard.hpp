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
    const Piece& get_last_piece() const { return last_piece_; }
protected:
    virtual void update(Position pos) = 0;
    virtual void size() = 0;
    virtual Position wait_input() const = 0;

    virtual int count_left(const Piece&) const = 0;
    virtual int count_right(const Piece&) const = 0;
    virtual int count_up(const Piece&) const = 0;
    virtual int count_down(const Piece&) const = 0;
    virtual int count_up_left(const Piece&) const = 0;
    virtual int count_up_right(const Piece&) const = 0;
    virtual int count_down_left(const Piece&) const = 0;
    virtual int count_down_right(const Piece&) const = 0;

    Piece last_piece_;
    bool status_;
};  // endof class ChessBoard_base

template <BoardSize Size=BoardSize::Small>
class ChessBoard : public ChessBoard_base {
public:
    ChessBoard() 
        : board_(static_cast<size_t>(Size), 
            std::vector<std::unique_ptr<Postion>>(
                static_cast<size_t>(Size), nullptr)) {
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
    size_t get_status(int row, int col) const {
        return board_[i][j]->get_status();
    }

    void print_board() const {
        for (size_t i = 0; i < len(); i++) {
            for (size_t j = 0; j < len(); j++) {
                std::cout << board_[i][j]->get_status() << " ";
            }
            std::cout << "\n";
        }
    }

    size_t len() const { return size(); }
    size_t size() const { return static_cast<size_t>(Size); }

    int count_left(const Piece& piece) const {
        int row = piece.row;
        int col = piece.col - 1;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            }
            col--;
        }
        return cnt;
    }
    int count_right(const Piece& piece) const {
        int row = piece.row;
        int col = piece.col + 1;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            }
            col++;
        }
        return cnt;
    }
    int count_up(const Piece& piece) const {
        int row = piece.row - 1;
        int col = piece.col;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_row(row)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            }
            row--;
        }
        return cnt;
    }
    int count_down(const Piece& piece) const {
        int row = piece.row + 1;
        int col = piece.col;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_row(row)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            }
            row++;
        }
        return cnt;
    } 
    int count_up_left(const Piece& piece) const {
        int row = piece.row - 1;
        int col = piece.col - 1;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            }
            col++;
        }
        return cnt;
    }
    int count_up_right(const Piece&) = 0;
    int count_down_left(const Piece&) = 0;
    int count_down_right(const Piece&) = 0;
    
private:
    void _init_board() {
        for (size_t i = 0; i < len(); i++) {
            for (size_t j = 0; j < len(); j++) {
                board_[i][j] = std::make_unique<Position>(i, j);
            }
        }
    } 
    
    bool is_valid_row(int row) const {
        return row >= 0 and row < size();
    }
    bool is_valid_col(int col) const {
        return col >= 0 and col < size();
    }
    bool is_valid_row_col(int row, int col) const {
        return is_valid_row(row) && is_valid_col(col);
    }

    std::vector<std::vector<std::unique_ptr<Position>>> board_;
    
};  // endof class ChessBoard

template <BoardSize Size=BoardSize::Small>
class GUIBoard : public ChessBoard<Size> {
public:
    Position wait_input() {

    }
};  // endof class GUIBoard

template <BoardSize Size=BoardSize::Small>
class CMDBoard : ChessBoard<Size> {
    Position wait_input() const override {
        std::string input_str;
        std::cin >> input_str;
        Position ret = CMDBoard::validate_input(input_str);
        if (ret.row < 0 or ret.col < 0) {
            std::cout << "invalid input, plz try again\n";
            ret = this->wait_input();
        }
        return ret;
    }
    static Position validate_input(const std::string& str) {
        if (str.size() != 2) return {-1, -1};
        return {get_int(str[0]), get_int(str[1])};
    }
    static int get_int(char c) {
        int res = -1
        if ('a' <= c && c <= 'z') {
            res = c - 'a';
        } else if ('A' <= c && c <= 'Z') {
            res = c - 'A';
        } else {
            return -1;
        }
        if (res >= this->size()) { return -1; }
        return res;
    }
};  // endof class CMDBoard

}  // endof namespace mfwu

#endif  // __CHESSBOARD_HPP__