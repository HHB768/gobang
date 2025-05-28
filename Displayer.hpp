#ifndef __DISPLAYER_HPP__
#define __DISPLAYER_HPP__

#include "common.hpp"
#include "Logger.hpp"
#include "gui_common.hpp"

// gui(?) of cmd mode lol
namespace mfwu {

//  2 种改进思路
/*
    1. 全部与 BoardSize 无关的函数都 virtual 在 Displayer 这里
    2. 强制 Displayer 系列以 board / size 初始化，不写在模板里
    想搞 1，因为 ChessBoard 就是这个思路，进行到底！ XQ3
*/
// TODO: 在player里面提供displayer指针，各种player和archive自己new 0418
class Displayer_base_base {
public:
    static constexpr const char empty_position_char = '+';
    static constexpr const char white_piece_char = 'O';
    static constexpr const char white_sp_piece_char = '@';
    static constexpr const char black_piece_char = 'X';
    static constexpr const char black_sp_piece_char = '*';
    static constexpr const char unknown_status_piece_char = '!';
    static constexpr const char inner_border_char = ' ';
    static constexpr const char outer_border_char = '.';
    // static constexpr const char highlight_up_down_char = '-';
    static constexpr const char highlight_left_char = '[';
    static constexpr const char highlight_right_char = ']';

    virtual const std::vector<std::string>& get_framework() const = 0;
    virtual void unzip_tbl(const std::string_view& str, bool mode) = 0;
};  // endof class Displayer_base_base

#define DEFINE_SHAPES \
static constexpr const char empty_position_char = base_type::empty_position_char;\
static constexpr const char white_piece_char = base_type::white_piece_char;\
static constexpr const char white_sp_piece_char = base_type::white_sp_piece_char;\
static constexpr const char black_piece_char = base_type::black_piece_char;\
static constexpr const char black_sp_piece_char = base_type::black_sp_piece_char;\
static constexpr const char unknown_status_piece_char = base_type::unknown_status_piece_char;\
static constexpr const char inner_border_char = base_type::inner_border_char;\
static constexpr const char outer_border_char = base_type::outer_border_char;\
/* static constexpr const char highlight_up_down_char = base_type::highlight_up_down_cha; */\
static constexpr const char highlight_left_char = base_type::highlight_left_char;\
static constexpr const char highlight_right_char = base_type::highlight_right_char;

template <BoardSize Size=BoardSize::Small, bool Mode=true>
class Displayer_base : public Displayer_base_base {
    /*  
        Mode = true:
        board: 2 * 3
            A B C 
          . . . . .
        A . O + X .
        B . + O + .
          . . . . .

        framework:   row : 1 + 1 + 2 * 1 + 1 = 1 * (3 + 2)
                     col : 2 + 2 + 3 * 2 + 1 = 2 * (3 + 3)

        [i, j] -> [2 + 1 * i, 4 + 2 * j]
    */
public:
    static constexpr size_t size_   = static_cast<size_t>(Size);
    static constexpr size_t height_ = 1 * (3 * Mode + size_);
    static constexpr size_t width_  = (1 + Mode) * (3 * Mode + size_);
    using base_type = Displayer_base_base;
    DEFINE_SHAPES;

    Displayer_base() : framework_(height_, std::string(width_, inner_border_char)) {
        _init_framework();
        load_empty_board();
    }
    // Displayer_base(const std::vector<std::vector<std::shared_ptr<Position>>>& board_) 
    Displayer_base(const std::vector<std::vector<size_t>>& board_) 
        : framework_(height_, std::string(width_, inner_border_char)) {
        _init_framework();
        reconstruct(board_);
    } 

    const std::vector<std::string>& get_framework() const override {
        return framework_;
    };

    static size_t get_row_in_framework(int r) {
        return 2 * Mode + 1 * r;
    }
    static size_t get_col_in_framework(int c) {
        return 4 * Mode + (1 + Mode) * c;
    }

    char& get_pos_ref_in_framework(int r, int c) {
        return framework_[get_row_in_framework(r)][get_col_in_framework(c)];
    }
    static std::pair<size_t, size_t> get_pos_in_framework(int r, int c) {
        return {get_row_in_framework(r), get_col_in_framework(c)};
    }

    void print_empty_position(int r, int c) {
        // framework_[get_row_in_framework(r)][get_col_in_framework(c)] = empty_position_char;
        get_pos_ref_in_framework(r, c) = empty_position_char;
    }
    void print_white_piece(int r, int c) {
        // framework_[get_row_in_framework(r)][get_col_in_framework(c)] = white_piece_char;
        get_pos_ref_in_framework(r, c) = white_piece_char;
    }
    void print_white_sp_piece(int r, int c) {
        // framework_[get_row_in_framework(r)][get_col_in_framework(c)] = white_sp_piece_char;
        get_pos_ref_in_framework(r, c) = white_sp_piece_char;
    }
    void print_black_piece(int r, int c) {
        // framework_[get_row_in_framework(r)][get_col_in_framework(c)] = black_piece_char;
        get_pos_ref_in_framework(r, c) = black_piece_char;
    }
    void print_black_sp_piece(int r, int c) {
        // framework_[get_row_in_framework(r)][get_col_in_framework(c)] = black_sp_piece_char;
        get_pos_ref_in_framework(r, c) = black_sp_piece_char;
    }
    void print_unknown_status_piece(int r, int c) {
        // framework_[get_row_in_framework(r)][get_col_in_framework(c)] = unknown_status_piece_char;
        get_pos_ref_in_framework(r, c) = unknown_status_piece_char;
    }

    virtual void load_empty_board() {
        for (int i = 0; i < size_; i++) {
            for (int j = 0; j < size_; j++) {
                print_empty_position(i, j);
            }
        }
        // remove_highlight();  // CHECK
    }

protected:
    void update_directly(int i, int j, size_t status) {
        switch (status) {
        // Invalid = 0,
        // White   = 1,
        // WhiteSp = 2,
        // Black   = 3,
        // BlackSp = 4
        case static_cast<size_t>(Piece::Color::Invalid) : {  // empty
            print_empty_position(i, j);
        } break;
        case static_cast<size_t>(Piece::Color::White): {
            print_white_piece(i, j);
        } break;
        case static_cast<size_t>(Piece::Color::WhiteSp) : {
            print_white_sp_piece(i, j);
        } break;
        case static_cast<size_t>(Piece::Color::Black) : {
            print_black_piece(i, j);
        } break;
        case static_cast<size_t>(Piece::Color::BlackSp) : {
            print_black_sp_piece(i, j);
        } break;
        default:
            log_error("in %s, %s", __FILE__, __LINE__);
            logerr_unknown_piece_status();
            print_unknown_status_piece(i, j);
        }
    }
    std::vector<std::string> framework_;

private:
    void _init_framework() {
        if (Mode) {
            for (int j = 0; j < size_; j++) {
                framework_[0][get_col_in_framework(j)] = 'A' + j;
            }
            for (int i = 0; i < size_; i++) {
                framework_[get_row_in_framework(i)][0] = 'A' + i;
            }
            for (int j = 0; j <= size_; j++) {
                // framework_[get_row_in_framework(-1)][get_col_in_framework(j)] = outer_border_char;
                get_pos_ref_in_framework(-1, j) = outer_border_char;
            }
            for (int j = 0; j <= size_; j++) {
                // framework_[get_row_in_framework(size_)][get_col_in_framework(j)] = outer_border_char;
                get_pos_ref_in_framework((int)size_, j) = outer_border_char;
            }
            for (int i = 0; i <= size_; i++) {
                // framework_[get_row_in_framework(i)][get_col_in_framework(-1)] = outer_border_char;
                get_pos_ref_in_framework(i, -1) = outer_border_char;
            }
            for (int i = 0; i <= size_; i++) {
                // framework_[get_row_in_framework(i)][get_col_in_framework(size_)] = outer_border_char;
                get_pos_ref_in_framework(i, (int)size_) = outer_border_char;
            }
            // framework_[get_row_in_framework(-1)][get_col_in_framework(-1)] = outer_border_char;
            get_pos_ref_in_framework(-1, -1) = outer_border_char;
        }
    }
    void reconstruct(const std::vector<std::vector<size_t>>& board_) {
        for (int i = 0; i < size_; i++) {
            for (int j = 0; j < size_; j++) {
                update_directly(i, j, board_[i][j]);
            }
        }
    }

};  // endof class Displayer_base

#define DEFINE_SIZES \
static constexpr size_t size_   = base_type::size_;\
static constexpr size_t height_ = base_type::height_;\
static constexpr size_t width_  = base_type::width_;

template <BoardSize Size=BoardSize::Small>
class Displayer : public Displayer_base<Size, true> {
public:
    using base_type = Displayer_base<Size, true>;

    DEFINE_SHAPES; DEFINE_SIZES;

    Displayer() : base_type() {}
    Displayer(const std::vector<std::vector<size_t>>& board_) : base_type(board_) {} 

    virtual void show() const {
        std::stringstream ss;
        log_debug("Board: ");
        for (const std::string& line : this->framework_) {
            ss << line << "\n";
            log_debug(XQ4GB_TIMESTAMP, line.c_str());
        }
        std::cout << ss.str();
    }
    virtual void remove_last_sp(const Piece& last_piece) {  // no override ?
        remove_highlight(last_piece.row, last_piece.col);
        remove_sp(last_piece.row, last_piece.col);
    }
    virtual void update_new_sp(Piece last_piece) {
        add_sp(last_piece);
        add_highlight(last_piece);
    }
    // observer of chessboard::update
    virtual void update_piece() {
        // TODO
    }

    // use for cmd winnder display & infer_displayer update
    void update(int i, int j, size_t status) {
        base_type::update_directly(i, j, status);

        remove_highlight();
        if (status) { add_highlight(i, j); }
        // there's efficient approach, but i'm lazy enough...
        // TODO
    }

    std::string zip_tbl(const std::vector<std::vector<size_t>>& board) const {
        size_t status_num = board.size() * board.size();
        std::string ret; ret.reserve(status_num + 4);
        if (zip_mode_ == false) {
            ret += ": ";
            for (const auto& line : board) {
                for (const size_t& status : line) {
                    ret += '0' + status;
                }
            }
        } else {  // zip_mode_ = true
            ret += "* ";
            size_t last_status = -1;
            size_t more_num = 0;
            for (const auto& line : board) {
                for (const size_t& status : line) {
                    if (status == last_status) {
                        more_num++;
                    } else {  // different status
                        if (more_num) {
                            ret += '{';
                            ret += std::to_string(more_num);  // status < 10 is better
                            ret += '}';
                            more_num = 0;
                        }
                        last_status = status;
                        ret += '0' + status;
                    }
                }
            }
            if (more_num) {
                ret += '{';
                ret += std::to_string(more_num);
                ret += '}';
                more_num = 0;
            }
            if (ret.size() > status_num / 2) {
                zip_mode_ = false;
            }
        }
        // ret += '\n';
        return ret;
    }
    
    void unzip_tbl(const std::string_view& str, bool mode) override {
        // ensure you have initialized it first
        int i = 0, j = 0;
        if (mode == false) {
            for (int k = 0; k < str.size(); k++) {
                base_type::update_directly(i, j, str[k] - '0');
                advance(i, j);
                if (i >= height_) break;
            }
        } else {
            int last_num = -1;
            for (int k = 0; k < str.size(); k++) {
                if (str[k] == '{') {
                    int kc = k;
                    while (str[k] != '}') {  // once i wrote like this : k != '}' : k++ holy s🐻🐻t
                        k++;
                    }
                    kc = atoi(str.substr(kc + 1, k - kc - 1).data());
                    for (int ii = 0; ii < kc; ii++) {
                        base_type::update_directly(i, j, last_num);
                        advance(i, j); if (i >= height_) break;
                    }
                } else if (is_digit(str[k])) {
                    last_num = str[k] - '0';
                    base_type::update_directly(i, j, last_num);
                    advance(i, j); if (i >= height_) break;
                }
            }
        }
    }

    void load_empty_board() override {
        base_type::load_empty_board();
        this->remove_highlight();
    }

protected:
    virtual void remove_highlight() {
        for (std::string& line : this->framework_) {
            for (char& c : line) {
                if (c == highlight_left_char 
                    or c == highlight_right_char) {
                    c = inner_border_char;
                }
            }
        }
    }
    virtual void remove_highlight(int r, int c) {
        auto [row, col] = base_type::get_pos_in_framework(r, c);
        char* ch = &this->framework_[row][col - 1];
        if (*ch == highlight_left_char) {
            *ch = inner_border_char;
        }
        ch = &this->framework_[row][col + 1];
        if (*ch == highlight_right_char) {
            *ch = inner_border_char;
        }
    }
    virtual void add_highlight(const Piece& last_piece) {
        auto [row, col] = base_type::get_pos_in_framework(last_piece.row, 
                                               last_piece.col);
        // framework_[row - 1][col] = highlight_up_down_char;
        // framework_[row + 1][col] = highlight_up_down_char;
        this->framework_[row][col - 1] = highlight_left_char;
        this->framework_[row][col + 1] = highlight_right_char;
    }
    virtual void add_highlight(int r, int c) {
        // assert(...)
        auto [row, col] = base_type::get_pos_in_framework(r, c);
        // framework_[row - 1][col] = highlight_up_down_char;
        // framework_[row + 1][col] = highlight_up_down_char;
        this->framework_[row][col - 1] = highlight_left_char;
        this->framework_[row][col + 1] = highlight_right_char;
    }
    virtual void remove_sp() {
        for (auto line : this->framework_) {
            for (auto c : line) {
                if (c == white_sp_piece_char) {
                    c = white_piece_char;
                } else if (c == black_sp_piece_char) {
                    c = black_piece_char;
                }
            }
        }
    }
    virtual void remove_sp(int r, int c) {
        auto [row, col] = base_type::get_pos_in_framework(r, c);
        char& ch = this->framework_[row][col];
        if (ch == white_sp_piece_char) {
            ch = white_piece_char;
        } else if (ch == black_sp_piece_char) {
            ch = black_piece_char;
        }
    }
    virtual void add_sp(const Piece& last_piece) {
        auto [row, col] = base_type::get_pos_in_framework(last_piece.row, 
                                               last_piece.col);
        if (last_piece.color == Piece::Color::BlackSp) {
            this->framework_[row][col] = black_sp_piece_char;
        } else if (last_piece.color == Piece::Color::WhiteSp) {
            this->framework_[row][col] = white_sp_piece_char;
        } else {
            log_warn("Last piece is not colored as sp");
            log_warn(XQ4GB_TIMESTAMP, "trying to fix it with real color...");
            if (last_piece.get_status() == 0) {
                log_error("last piece is not placed yet");
            } else {
                add_sp(Piece{last_piece.row, last_piece.col, 
                       Piece::Color{Piece::get_real_status(last_piece.color) + 1}});  // CHECK: can i?
            }
        }
    }

private:
    void advance(int& i, int& j) {
        if (j < size_ - 1) {
            j++;
        } else {
            i++; j = 0;
        }
    }

    mutable bool zip_mode_ = true;  // 0 : "0000111", 1 : "0{3}1{2}"
};  // endof class Displayer

template <BoardSize Size=BoardSize::Small>
class CmdDisplayer : public Displayer<Size> {  // CmdDisplayer 和 Displayer 真的有区别吗，GuiDisplayer 实际也继承自 CmdPlayer吧？
                                               // 另外，把 log 直接在 Displayer 里面做了不行吗，不用存两个 framework 了
public:
    using base_type = Displayer<Size>;
    DEFINE_SHAPES; DEFINE_SIZES;

    CmdDisplayer() : base_type() {}
    CmdDisplayer(const std::vector<std::vector<size_t>>& board_) : base_type(board_) {}

    void show_without_log() const {
        std::stringstream ss;
        for (const std::string& line : this->framework_) {
            ss << line << "\n";
        }
        std::cout << ss.str();
    }
    void set_highlight(int r, int c) {
        base_type::add_highlight(r, c);
    }
};  // endof class CmdDisplayer

template <BoardSize Size=BoardSize::Small>
class InferDisplayer : public Displayer<Size> {  // 考虑继承自 CmdDisplayer
public:
    using base_type = Displayer<Size>;
    DEFINE_SHAPES; DEFINE_SIZES;

    InferDisplayer() = delete;  // you must assign a board to begin deduction
    InferDisplayer(const std::vector<std::vector<size_t>>& board_) : base_type(board_) {}

#ifndef __LOG_INFERENCE_ELSEWHERE__
    void log_inference(size_t depth, const std::vector<std::vector<size_t>>&) const {
        log_infer(depth, "deduction board:");
        for (const std::string& line : this->framework_) {
            log_infer(XQ4GB_TIMESTAMP, depth, line.c_str());
        }
    }
#else  // __LOG_INFERENCE_ELSEWHERE__
    void log_inference(size_t depth, const std::vector<std::vector<size_t>>& board) const {
        std::string zipped_board = base_type::zip_tbl(board);
        log_infer(depth, zipped_board.c_str());
    }
#endif  // __LOG_INFERENCE_ELSEWHERE__

};  // endof class InferDisplayer


class FuncBox : public Box {
public:
    using pic_type = Box::pic_type;

    FuncBox(int x0_, int x1_, int y0_, int y1_, 
        CommandType cmd_type_, const pic_type& pic_)
        : Box(x0_, x1_, y0_, y1_, cmd_type_, pic_) {}
    FuncBox(int x0_, int x1_, int y0_, int y1_, 
        CommandType cmd_type_, const std::vector<pic_type>& pics_)
        : Box(x0_, x1_, y0_, y1_, cmd_type_, pics_) {}
private: 
    Command get_cmd(const PositionPix& pos) const override {
        if (this->encircle(pos)) {
            return Command{get_cmd_type(), {}};
        }
        return Command{CommandType::INVALID, {}};
    }
};  // endof class FuncBox

template <BoardSize Size=BoardSize::Small>
class BoardBox : public Box {
public:
    using pic_type = Box::pic_type;
    
    BoardBox(int x0_, int x1_, int y0_, int y1_, 
        CommandType cmd_type_, const pic_type& pic_)
        : Box(x0_, x1_, y0_, y1_, cmd_type_, pic_) {}
    BoardBox(int x0_, int x1_, int y0_, int y1_, 
        CommandType cmd_type_, const std::vector<pic_type>& pics_)
        : Box(x0_, x1_, y0_, y1_, cmd_type_, pics_) {}
private:
    static constexpr int vertical_margin = 0;
    static constexpr int horizontal_margin = 0;
    static constexpr int grid_len = 10;

    Command get_cmd(const PositionPix& pos) const override {
        if (this->encircle(pos)) {
            auto ret = Command{CommandType::PIECE, {get_row(pos.vpix), get_col(pos.hpix)}};
            if (ret.pos.row == -1 or ret.pos.col == -1) return ret;
            return ret;
        }
        return Command{CommandType::INVALID, {}};
    }

    static int get_row(int vertical_pix) {
        int ret = round(((double)vertical_pix - vertical_margin) / grid_len);
        // if (!ChessBoard<Size>::is_valid_row(ret)) { return -1; }
        // should check this after returned to chessboard
        return ret;
    }
    static int get_col(int horizontal_pix) {
        int ret = round(((double)horizontal_pix - horizontal_margin) / grid_len);
        // if (!ChessBoard<Size>::is_valid_col(ret)) { return -1; }
        return ret;
    }
};  // endof class BoardBox

// only used in menu in main.cc
class MenuPage : public Page {
public:
    using pic_type = typename Page::pic_type;

    static constexpr int helper_pos[4] = {0, 0, 0, 0};
    static constexpr pic_type helper_mode = {};
    static constexpr pic_type helper_size = {};

    static constexpr int option_x0[3] = {0, 0, 0};
    static constexpr int option_x1[3] = {0, 0, 0};
    static constexpr int option_y0[3] = {0, 0, 0};
    static constexpr int option_y1[3] = {0, 0, 0};
    static constexpr pic_type option_mode[3] = {{}, {}, {}};
    static constexpr pic_type option_size[3] = {{}, {}, {}};

    MenuPage(PageType type, const std::string& str1="") 
        : Page(type, str1) {
        if (type == PageType::Menu1) {
            typename Box::ptr helper = std::make_shared<FuncBox>(
                helper_pos[0], helper_pos[1],
                helper_pos[2], helper_pos[3], 
                CommandType::INVALID, helper_mode
            );
            this->append(helper);
            for (size_t i = 0; i < 3; i++) {
                typename Box::ptr option = std::make_shared<FuncBox>(
                    option_x0[i], option_x1[i], 
                    option_y0[i], option_y1[i], 
                    CommandType{i}, option_mode[i]
                );
                this->append(option);
            }
        } else if (type == PageType::Menu2) {
            typename Box::ptr helper = std::make_shared<FuncBox>(
                helper_pos[0], helper_pos[0],
                helper_pos[0], helper_pos[0], 
                CommandType::INVALID, helper_size
            );
            this->append(helper);
            for (size_t i = 0; i < 3; i++) {
                typename Box::ptr option = std::make_shared<FuncBox>(
                    option_x0[i], option_x1[i], 
                    option_y0[i], option_y1[i], 
                    CommandType{i}, option_size[i]
                );
                this->append(option);
            }
        } else {
            log_error("Not a valid menu page");
        }
    }
};  // endof class MenuPage


template <BoardSize Size=BoardSize::Small>
class GamePage : public Page {
public:
    using pic_type = typename Page::pic_type;

    static constexpr int boardbox_pos[4] = {0, 0, 0, 0};
    static constexpr pic_type boardbox_pic = {};

    static constexpr int funcbox_x0[4] = {0, 0, 0, 0};
    static constexpr int funcbox_x1[4] = {0, 0, 0, 0};
    static constexpr int funcbox_y0[4] = {0, 0, 0, 0};
    static constexpr int funcbox_y1[4] = {0, 0, 0, 0};
    static constexpr pic_type funcbox_pics[4] = {{}, {}, {}, {}};

    GamePage(PageType type, const std::string& str1="")
        : Page(type, str1) {
        typename Box::ptr board_box = std::make_shared<BoardBox<Size>>(
            boardbox_pos[0], boardbox_pos[1],
            boardbox_pos[2], boardbox_pos[3], 
            CommandType::PIECE, boardbox_pic
        );
        this->append(board_box);
        typename Box::ptr restart_box = std::make_shared<FuncBox>(
            funcbox_x0[0], funcbox_x1[0],
            funcbox_y0[0], funcbox_y1[0], 
            CommandType::RESTART, funcbox_pics[0]
        );
        this->append(restart_box);
        typename Box::ptr menu_box = std::make_shared<FuncBox>(
            funcbox_x0[1], funcbox_x1[1],
            funcbox_y0[1], funcbox_y1[1], 
            CommandType::MENU, funcbox_pics[1]
        );
        this->append(menu_box);
        typename Box::ptr quit_box = std::make_shared<FuncBox>(
            funcbox_x0[2], funcbox_x1[2],
            funcbox_y0[2], funcbox_y1[2], 
            CommandType::QUIT, funcbox_pics[2]
        );
        this->append(quit_box);
        typename Box::ptr xq4gb_box = std::make_shared<FuncBox>(
            funcbox_x0[3], funcbox_x1[3],
            funcbox_y0[3], funcbox_y1[3], 
            CommandType::XQ4GB, funcbox_pics[3]
        );
        this->append(xq4gb_box);
    }

    void show() const override {

    }
};  // endof class GamePage

// 致敬传奇人机战神 vick       XQ4-3  25.04.11
class VickPage : public Page {
public:
    using pic_type = typename Page::pic_type;   
    static constexpr int num_of_vick_pic = 5;
    static constexpr int vick_pos[4] = {0, 0, 0, 0};
    static constexpr pic_type vick_pics[num_of_vick_pic] = {};

    VickPage(PageType type, const std::string& str1="")
        : Page(type, str1) {
        std::vector<pic_type> pics(vick_pics, vick_pics + num_of_vick_pic);
        typename Box::ptr vick_box = std::make_shared<FuncBox>(
            vick_pos[0], vick_pos[1],
            vick_pos[2], vick_pos[3], 
            CommandType::INVALID, pics // CHECK
        );
        // always put vick_box in the first idx
        this->append(vick_box);
    }

    void show() const override {
        for (auto&& pic : this->get_box(0)->get_pics()) {
            // show pic
        }
    }
};  // endof class VickPage


template <BoardSize Size=BoardSize::Small>
class GuiDisplayer : public Displayer<Size> {
public:
    using base_type = Displayer<Size>;
    // DEFINE_SHAPES; DEFINE_SIZES;

    GuiDisplayer() 
        : base_type(), game_page_(PageType::GamePlayPage, "Game playing page"), 
          black_victory_page_(PageType::BlackVickPage, "Black victory page"),
          white_victory_page_(PageType::WhiteVickPage, "White victory page"),
          menu1page_(PageType::Menu1, "Mode selection helper page"),
          menu2page_(PageType::Menu2, "Size selection helper page") {}
    GuiDisplayer(const std::vector<std::vector<size_t>>& board_) 
        : base_type(board_), game_page_("Game", "Game playing page"), 
          black_victory_page_(PageType::BlackVickPage, "Black victory page"),
          white_victory_page_(PageType::WhiteVickPage, "White victory page"),
          menu1page_(PageType::Menu1, "Mode selection helper page"),
          menu2page_(PageType::Menu2, "Size selection helper page") {}
    
    Command get_command() const {
        return page_->get_command();
    }
    void show() const override {
        page_->show();
    }
    // // 除了displayer里面cmdboardframework更新的那一部分（用于log）
    // // 还要加上自己在gui中展示的部分
    // void remove_last_sp(const Piece& last_piece) override {
    //     base_type::remove_last_sp(last_piece);
    //     // replace sp piece pic with normal piece
    // }
    // void update_new_sp(const Piece& last_piece) override {
    //     base_type::update_new_sp(last_piece);
    //     // replace normal piece pic with sp piece
    // }
    // virtual void update() {
    //     // TODO
    // }

    void switch_page(const PageType page=PageType::GamePlayPage) {  // TODO: define enum and pass integer
        switch (page) {
        case PageType::GamePlayPage : {
            page_ = &game_page_;
        } break;
        case PageType::BlackVickPage : {
            page_ = &black_victory_page_;
        } break;
        case PageType::WhiteVickPage : {
            page_ = &white_victory_page_;
        } break;
        case PageType::Menu1 : {
            page_ = &menu1page_;
        } break;
        case PageType::Menu2 : {
            page_ = &menu2page_;
        } break;
        default : {
            log_warn("Page type : %lu not found, page not switched",
                     static_cast<size_t>(page));
        }
        }
        
        page_->show();
    }

private:
    GamePage<Size> game_page_;
    VickPage black_victory_page_;
    VickPage white_victory_page_;
    MenuPage menu1page_;
    MenuPage menu2page_;
    Page* page_;  // do not manage memory with this ptr

};  // 

}  // endof namespace mfwu

#endif  // __DISPLAYER_HPP__