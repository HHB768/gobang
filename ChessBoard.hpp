#ifndef __CHESSBOARD_HPP__
#define __CHESSBOARD_HPP__

#include "utils.hpp"
#include "CmdFramework.hpp"

namespace mfwu {

// class Player;

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

    virtual void reset() = 0;
    const Piece& get_last_piece() const { return last_piece_; }
    virtual void update(const Piece& piece) = 0;
    virtual size_t size() const = 0;
    virtual size_t get_status(int row, int col) const = 0;
    virtual Command get_command() = 0;

    virtual void show() const = 0;
    virtual void refresh() = 0;


    virtual int count_left(const Piece&) const = 0;
    virtual int count_right(const Piece&) const = 0;
    virtual int count_up(const Piece&) const = 0;
    virtual int count_down(const Piece&) const = 0;
    virtual int count_up_left(const Piece&) const = 0;
    virtual int count_up_right(const Piece&) const = 0;
    virtual int count_down_left(const Piece&) const = 0;
    virtual int count_down_right(const Piece&) const = 0;

    virtual int count_dir(const Piece& piece, const std::pair<int, int>& dir) const = 0;
    virtual void count_dir(const Piece& piece, count_res_8* res) const = 0;
    virtual void count_dir(const Piece& piece, count_res_4* res) const = 0;

protected:
    Piece last_piece_;
    bool status_;
};  // endof class ChessBoard_base

template <BoardSize Size=BoardSize::Small>
class ChessBoard : public ChessBoard_base {
public:
    static constexpr size_t size_ = static_cast<size_t>(Size);
    ChessBoard() 
        : board_(static_cast<size_t>(Size), 
            std::vector<std::shared_ptr<Position>>(
                static_cast<size_t>(Size))) {
        _init_board();
    }
    ChessBoard(const std::vector<std::vector<size_t>>& input_board) 
        : board_(Size, std::vector<std::shared_ptr<Position>>(Size)) {
        for (size_t i = 0; i < static_cast<size_t>(Size); i++) {
            for (size_t j = 0; j < static_cast<size_t>(Size); j++) {
                switch (input_board[i][j]) {
                case static_cast<size_t>(Color::Invalid) : {
                    board_[i][j] = std::make_shared<Position>(i, j);
                } break;
                case static_cast<size_t>(Color::White) : {
                    board_[i][j] = std::make_shared<Position>(Piece(i, j, Color::White));
                } break;
                case static_cast<size_t>(Color::Black) : {
                    board_[i][j] = std::make_shared<Position>(Piece(i, j, Color::Black));
                } break;
                case static_cast<size_t>(Color::BlackSp) : {
                    board_[i][j] = std::make_shared<Position>(Piece(i, j, Color::BlackSp));
                } break;
                case static_cast<size_t>(Color::WhiteSp) : {
                    board_[i][j] = std::make_shared<Position>(Piece(i, j, Color::WhiteSp));
                } break;
                default:
                    std::cout << "Position [" << i << ", " << j << "] init failed: UNKOWN PIECE COLOR\n";
                    // TODO: LOG IT
                    board_[i][j] = std::make_shared<Position>(i, j);
                }
            }
        }
    }
    ChessBoard(const ChessBoard& board) = default;
    ChessBoard(ChessBoard&& board) = default;

    ChessBoard& operator=(const ChessBoard& board) = default;
    ChessBoard& operator=(ChessBoard&& board) = default;

    virtual ~ChessBoard() {}
    
    void reset() override {
        _init_board();
    }

    virtual void update(const Piece& piece) override {
        board_[last_piece_.row][last_piece_.col] 
            = std::make_shared<Position>(Piece{last_piece_.row, last_piece_.col, 
                                               Piece::Color{last_piece_.get_status() - 1}});
        board_[piece.row][piece.col] 
            = std::make_shared<Position>(piece);
    }
    size_t get_status(int row, int col) const {
        assert(is_valid_row_col(row, col));
        return board_[row][col]->get_status();
    }

    std::string serialize() const {
        std::stringstream ss;
        for (size_t i = 0; i < len(); i++) {
            for (size_t j = 0; j < len(); j++) {
                ss<< board_[i][j]->get_status() << " ";
            }
            ss << "\n";
        }
        return ss.str();
    }

    size_t len() const { return size(); }
    size_t size() const override { return static_cast<size_t>(Size); }

    int count_left(const Piece& piece) const override {
        int row = piece.row;
        int col = piece.col - 1;
        size_t status = piece.get_status();
        int cnt = 0;
        while (is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            col--;
        }
        return cnt;
    }
    int count_right(const Piece& piece) const override {
        int row = piece.row;
        int col = piece.col + 1;
        size_t status = piece.get_status();
        int cnt = 0;
        while (is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            col++;
        }
        return cnt;
    }
    int count_up(const Piece& piece) const override {
        int row = piece.row - 1;
        int col = piece.col;
        size_t status = piece.get_status();
        int cnt = 0;
        while (is_valid_row(row)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            row--;
        }
        return cnt;
    }
    int count_down(const Piece& piece) const override {
        int row = piece.row + 1;
        int col = piece.col;
        size_t status = piece.get_status();
        int cnt = 0;
        while (is_valid_row(row)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            row++;
        }
        return cnt;
    } 
    int count_up_left(const Piece& piece) const override {
        int row = piece.row - 1;
        int col = piece.col - 1;
        size_t status = piece.get_status();
        int cnt = 0;
        while (is_valid_row(row) && is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            row--;
            col--;
        }
        return cnt;
    }
    int count_up_right(const Piece& piece) const override {
        int row = piece.row - 1;
        int col = piece.col + 1;
        size_t status = piece.get_status();
        int cnt = 0;
        while (is_valid_row(row) && is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            row--;
            col++;
        }
        return cnt;
    }
    int count_down_left(const Piece& piece) const override {
        int row = piece.row + 1;
        int col = piece.col - 1;
        size_t status = piece.get_status();
        int cnt = 0;
        while (is_valid_row(row) && is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            row++;
            col--;
        }
        return cnt;
    } 
    int count_down_right(const Piece& piece) const override {
        int row = piece.row + 1;
        int col = piece.col + 1;
        size_t status = piece.get_status();
        int cnt = 0;
        while (is_valid_row(row) && is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            row++;
            col++;
        }
        return cnt;
    }
    int count_dir(const Piece& piece, const std::pair<int, int>& dir) const override {
        int row = piece.row + dir.first;
        int col = piece.col + dir.second;
        size_t status = piece.get_status();
        int cnt = 0;
        while (is_valid_row(row) && is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } { break; }
            row += dir.first;
            col += dir.second;
        }
        return cnt;
    }
    void count_dir(const Piece& piece, count_res_8* res) const override {
        res->right = count_right(piece);
        res->down = count_down(piece);
        res->left = count_left(piece);
        res->up = count_up(piece);
        res->down_right = count_down_right(piece);
        res->down_left = count_down_left(piece);
        res->up_right = count_up_right(piece);
        res->up_left = count_up_left(piece);
    }
    void count_dir(const Piece& piece, count_res_4* res) const override {
        res->left_right = count_left(piece) + count_right(piece);
        res->up_down    = count_up(piece) + count_down(piece);
        res->up_left_down_right = count_up_left(piece) + count_down_right(piece);
        res->up_right_down_left = count_up_right(piece) + count_down_left(piece);
    }

    virtual void show() const = 0;
    virtual void refresh() = 0;
protected:
    virtual void show_board() const = 0;
    std::vector<std::vector<std::shared_ptr<Position>>> board_;
private:
    void _init_board() {
        for (size_t i = 0; i < len(); i++) {
            for (size_t j = 0; j < len(); j++) {
                board_[i][j] = std::make_shared<Position>(i, j);
            }
        }
    } 
    
    static bool is_valid_row(int row) {  // i want them static
        return row >= 0 and row < static_cast<size_t>(Size);
    }
    static bool is_valid_col(int col) {
        return col >= 0 and col < static_cast<size_t>(Size);
    }
    static bool is_valid_row_col(int row, int col) {
        return is_valid_row(row) && is_valid_col(col);
    }
};  // endof class ChessBoard

// template <BoardSize Size=BoardSize::Small>
// class GUIBoard : public ChessBoard<Size> {
// public:
//     Command get_command() override {
//         return Command{CommandType::PIECE, {0, 0}};
//     }

//     void show() const override {

//     }
//     void refresh() override {
//         show();
//     }

//     using Archive_type = std::string;

// };  // endof class GUIBoard

template <BoardSize Size=BoardSize::Small>
class CMDBoard : public ChessBoard<Size> {
public:
    CMDBoard() 
        : ChessBoard<Size>(), framework_() {}
    CMDBoard(const std::vector<std::vector<size_t>>& input_board) 
        : ChessBoard<Size>(input_board), framework_(this->board_) {
    }
    CMDBoard(const CMDBoard& board) = default;  // really work?
    CMDBoard(CMDBoard&& board) = default;

    CMDBoard& operator=(const CMDBoard& board) = default;
    CMDBoard& operator=(CMDBoard&& board) = default;

    void update(const Piece& piece) override {
        ChessBoard<Size>::update(piece);
        framework_.update_step(this->last_piece_);
    }

    Command get_command() override {
        std::string input_str;
        std::cout << "wait input\n";
        std::cin >> input_str;
        std::cout << "input\n";
        Command ret = CMDBoard::validate_input(input_str);
        if (ret.pos.row < 0 or ret.pos.col < 0) {
            std::cout << "invalid input, plz try again\n";
            ret = this->get_command();
        }
        return ret;
    }
    void show() const override {
        cmd_clear();
        show_board();
        std::cout << CMD_HELPER << "\n";
        std::cout << INPUT_HELPER << "\n";
    }
    void refresh() override {
        show();
    }

private:
    static Command validate_input(const std::string& str) {
        if (str == std::string(QUIT_CMD1)
            || str == std::string(QUIT_CMD2)
            || str == std::string(QUIT_CMD3)) {
            return Command{CommandType::QUIT, {}};
        } else if (str == std::string(MENU_CMD1)
            || str == std::string(MENU_CMD2)
            || str == std::string(MENU_CMD3)) {
            return Command{CommandType::MENU, {}};
        } else if (str == std::string(RESTART_CMD1)
            || str == std::string(RESTART_CMD2)
            || str == std::string(RESTART_CMD3)) {
            return Command{CommandType::RESTART, {}};
        }
        if (str.size() != 2) return Command{CommandType::INVALID, {}};
        return Command{CommandType::PIECE, {get_int(str[0]), get_int(str[1])}};
    }
    static int get_int(char c) {
        int res = -1;
        if ('a' <= c && c <= 'z') {
            res = c - 'a';
        } else if ('A' <= c && c <= 'Z') {
            res = c - 'A';
        } else {
            return -1;
        }
        if (res >= static_cast<size_t>(Size)) { return -1; }
        return res;
    }
    void show_board() const {
        // TODO: better design:
        // init a vec<vec<char> frwk first and list a set of 
        // methods to locate the position of [i, j] on the board_  25.03.03
        // TODO: better design:
        // make framework a class
        framework_.show();
    }
    
    DisplayFramework<Size> framework_;
};  // endof class CMDBoard

}  // endof namespace mfwu

#endif  // __CHESSBOARD_HPP__