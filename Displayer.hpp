#ifndef __DISPLAYER_HPP__
#define __DISPLAYER_HPP__

#include "common.hpp"
#include "Logger.hpp"

// gui(?) of cmd mode lol
namespace mfwu {

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

template <BoardSize Size=BoardSize::Small, bool mode=true>
class Displayer_base : public Displayer_base_base {
    /*  
        mode = true:
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
    Displayer_base() : framework_(height_, std::string(width_, inner_border_char)) {
        _init_framework();
        load_empty_board();
    }
    Displayer_base(const std::vector<std::vector<std::shared_ptr<Position>>>& board_) 
        : framework_(height_, std::string(width_, inner_border_char)) {
        _init_framework();
        reconstruct(board_);
    } 
    static constexpr size_t size_   = static_cast<size_t>(Size);
    static constexpr size_t height_ = 1 * (3 * mode + size_);
    static constexpr size_t width_  = (1 + mode) * (3 * mode + size_);
    DEFINE_SHAPES;

    static size_t get_row_in_framework(int r) {
        return 2 * mode + 1 * r;
    }
    static size_t get_col_in_framework(int c) {
        return 4 * mode + (1 + mode) * c;
    }

    using base_type = Displayer_base_base;

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

protected:
    std::vector<std::string> framework_;

private:
    void _init_framework() {
        if (mode) {
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
    void load_empty_board() {
        for (int i = 0; i < size_; i++) {
            for (int j = 0; j < size_; j++) {
                print_empty_position(i, j);
            }
        }
        // remove_highlight();  // CHECK
    }

};  // endof class Displayer_base

#define DEFINE_SIZES \
static constexpr size_t size_   = base_type::size_;\
static constexpr size_t height_ = base_type::height_;\
static constexpr size_t width_  = base_type::width_;

template <BoardSize Size=BoardSize::Small>
class Displayer : public Displayer_base<Size, true> {
public:
    using base_type = Displayer_base<Size, true>
    DEFINE_SHAPES; DEFINE_SIZES;

    Displayer() : base_type() {}
    Displayer(const std::vector<std::vector<size_t>>& board_) : base_type(board_) {} 

    virtual void show() const {
        std::stringstream ss;
        log_debug("Board: ");
        for (const std::string& line : framework_) {
            ss << line << "\n";
            log_debug(XQ4GB_TIMESTAMP, line.c_str());
        }
        std::cout << ss.str();
    }
    virtual void remove_last_sp(const Piece& last_piece) {
        remove_highlight(last_piece.row, last_piece.col);
        remove_sp(last_piece.row, last_piece.col);
    }
    virtual void update_new_sp(Piece last_piece) {
        add_sp(last_piece);
        add_highlight(last_piece);
    }
    virtual void update() {
        // TODO
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

    void remove_highlight() {
        for (std::string& line : framework_) {
            for (char& c : line) {
                if (c == highlight_left_char 
                    or c == highlight_right_char) {
                    c = inner_border_char;
                }
            }
        }
    }
    void remove_highlight(int r, int c) {
        auto [row, col] = get_pos_in_framework(r, c);
        char* ch = &framework_[row][col - 1];
        if (*ch == highlight_left_char) {
            *ch = inner_border_char;
        }
        ch = &framework_[row][col + 1];
        if (*ch == highlight_right_char) {
            *ch = inner_border_char;
        }
    }
    void add_highlight(const Piece& last_piece) {
        auto [row, col] = get_pos_in_framework(last_piece.row, 
                                               last_piece.col);
        // framework_[row - 1][col] = highlight_up_down_char;
        // framework_[row + 1][col] = highlight_up_down_char;
        framework_[row][col - 1] = highlight_left_char;
        framework_[row][col + 1] = highlight_right_char;
    }
    void add_highlight(int r, int c) {
        // assert(...)
        auto [row, col] = get_pos_in_framework(r, c);
        // framework_[row - 1][col] = highlight_up_down_char;
        // framework_[row + 1][col] = highlight_up_down_char;
        framework_[row][col - 1] = highlight_left_char;
        framework_[row][col + 1] = highlight_right_char;
    }
    void remove_sp() {
        for (auto line : framework_) {
            for (auto c : line) {
                if (c == white_sp_piece_char) {
                    c = white_piece_char;
                } else if (c == black_sp_piece_char) {
                    c = black_piece_char;
                }
            }
        }
    }
    void remove_sp(int r, int c) {
        auto [row, col] = get_pos_in_framework(r, c);
        char& ch = framework_[row][col];
        if (ch == white_sp_piece_char) {
            ch = white_piece_char;
        } else if (ch == black_sp_piece_char) {
            ch = black_piece_char;
        }
    }
    void add_sp(const Piece& last_piece) {
        auto [row, col] = get_pos_in_framework(last_piece.row, 
                                               last_piece.col);
        if (last_piece.color == Piece::Color::BlackSp) {
            framework_[row][col] = black_sp_piece_char;
        } else if (last_piece.color == Piece::Color::WhiteSp) {
            framework_[row][col] = white_sp_piece_char;
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
};  // endof class Displayer

template <BoardSize Size=BoardSize::Small>
class CmdDisplayer : public Displayer<Size> {
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
        this->add_highlight(r, c);
    }
};  // endof class CmdDisplayer

template <BoardSize Size=BoardSize::Small>
class InferDisplayer : public Displayer<Size> {
public:
    using base_type = Displayer<Size>;
    DEFINE_SHAPES; DEFINE_SIZES;

    InferDisplayer() = delete;  // you must assign a board to begin deduction
    InferDisplayer(const std::vector<std::vector<size_t>>& board_) : base_type(board_) {}

#ifndef __LOG_INFERENCE_ELSEWHERE__
    void show(size_t depth, const std::vector<std::vector<size_t>>&) const override {
        log_infer(depth, "deduction board:");
        for (const std::string& line : framework_) {
            log_infer(XQ4GB_TIMESTAMP, depth, line.c_str());
        }
    }
#else  // __LOG_INFERENCE_ELSEWHERE__
    void show(size_t depth, const std::vector<std::vector<size_t>>& board) const override {
        std::string zipped_board = zip_tbl(board);
        log_infer(depth, zipped_board.c_str());
    }
#endif  // __LOG_INFERENCE_ELSEWHERE__
    std::string zip_tbl(const std::vector<std::vector<size_t>>& board) {
        size_t status_num = board.size() * board.size();
        std::string ret; ret.reserve(status_num + 4);
        if (zip_mode_ == false) {
            ret += ": ";
            for (const auto& line : board) {
                for (const size_t& status : line) {
                    ret += '0' + status;
                }
            }
        } else {  // zip_mode = true
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
    
    void unzip_tbl(const std::string_view& str, bool mode) {
        // ensure you have initialized it first
        int i = 0, j = 0;
        if (mode == false) {
            for (int k = 0; k < str.size(); k++) {
                update_directly(i, j, str[k] - '0');
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
                        update_directly(i, j, last_num);
                        advance(i, j); if (i >= height_) break;
                    }
                } else if (is_digit(str[k])) {
                    last_num = str[k] - '0';
                    update_directly(i, j, last_num);
                    advance(i, j); if (i >= height_) break;
                }
            }
        }
    }

    const std::vector<std::string>& get_framework() const {
        return this->framework_;
    };

    void update(int i, int j, size_t status) {
        update_directly(i, j, status);

        remove_highlight();
        if (status) add_highlight(i, j);
        // there's efficient approach, but i'm lazy enough...
    }

private:
    void advance(int& i, int& j) {
        if (j < size_ - 1) {
            j++;
        } else {
            i++; j = 0;
        }
    }

    mutable bool zip_mode_;  // 0 : "0000111", 1 : "0{3}1{2}"
};  // endof class InferDisplayer

struct PositionPix {
    int hpix, vpix;
    PositionPix() : hpix(0), vpix(0) {}
    PositionPix(int h, int v) : hpix(h), vpix(v) {}
    PositionPix(const PositionPix& pos) = default;
    ~PositionPix() {}
};  // endof struct PositionPix

struct Box {
public:
    using ptr = std::shared_ptr<Box>;
    using pic_type = PICTURE;

    Box(int x0_, int x1_, int y0_, int y1_, 
        CommandType cmd_type_, const pic_type& pic_)
        : x0(x0_), x1(x1_), y0(y0_), y1(y1_), 
          cmd_type(cmd_type_), pic(pic_) {
        assert(x0_ < x1_ && y0_ < y1_);
    }
    
    virtual Command get_cmd(const PositionPix& pos) const = 0;

    CommandType get_cmd_type() const {
        return cmd_type;
    }

    bool encircle(const PositionPix& pos) const {
        // in pix
        if (x0 <= pos.hpix && pos.hpix <= x1
            && y0 << pos.vpix && pos.vpix <= y1) {
            return true;
        }
        return false;
    }

    void show() {
        // show pic
    }

private:
    int x0, x1, y0, y1;
    const CommandType cmd_type;
    pic_type pic;
};  // endof struct Box

class FuncBox : public Box {
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
        if (!ChessBoard<Size>::is_valid_row(ret)) { return -1; }
        return ret;
    }
    static int get_col(int horizontal_pix) {
        int ret = round(((double)horizontal_pix - horizontal_margin) / grid_len);
        if (!ChessBoard<Size>::is_valid_col(ret)) { return -1; }
        return ret;
    }
};  // endof class BoardBox

class Page {
public:
    Page(const std::string& str, const std::string& str1="") 
        : name_(str), description_(str1) {}

    std::string get_name() const { return name_; }
    std::string get_description() const { return description_; }
    std::string append(typename Box::ptr b) {
        boxes_.push_back(b);
    }
    Command get_command() {
        const PositionPix& pos = wait_input();
        for (const typename Box::ptr& box : boxes_) {
            if (box->encircle(pos)) {
                return box->get_cmd(pos);
            }
        }
        return Command{CommandType::INVALID, {}};
    } 

private:
    const PositionPix& wait_input() {
        return {};
    }

    std::string name_;
    std::string description_;
    std::vector<typename Box::ptr> boxes_;
};  // endof class Page

class MenuPage : public Page {
public:
    static constexpr int helper_pos[4] = {0, 0, 0, 0};
    using pic_type = PICTURE;
    static constexpr pic_type helper_mode = {};
    static constexpr pic_type helper_size = {};

    static constexpr int option_x0[3] = {0, 0, 0};
    static constexpr int option_x1[3] = {0, 0, 0};
    static constexpr int option_y0[3] = {0, 0, 0};
    static constexpr int option_y1[3] = {0, 0, 0};
    static constexpr pic_type option_mode = {{}, {}, {}};
    static constexpr pic_type option_size = {{}, {}, {}};

    MenuPage(const std::string& str, const std::string& str1="") 
        : Page(str, str1) {
        if (str == std::string("Menu1")) {
            typename Box::ptr helper = std::make_shared<FuncBox>(
                helper_pos[0], helper_pos[0],
                helper_pos[0], helper_pos[0], 
                CommandType::INVALID, helper_mode
            );
            this->append(helper);
            for (int i = 0; i < 3; i++) {
                typename Box::ptr option = std::make_shared<FuncBox>(
                    option_x0[i], option_x1[i], 
                    option_y0[i], option_y1[i], 
                    CommandType{i}, option_mode[i]
                );
            }
        } else if (str == std::string("Menu2")) {
            typename Box::ptr helper = std::make_shared<FuncBox>(
                helper_pos[0], helper_pos[0],
                helper_pos[0], helper_pos[0], 
                CommandType::INVALID, helper_size
            );
            this->append(helper);
            for (int i = 0; i < 3; i++) {
                typename Box::ptr option = std::make_shared<FuncBox>(
                    option_x0[i], option_x1[i], 
                    option_y0[i], option_y1[i], 
                    CommandType{i}, option_size[i]
                );
            }
        } else {
            logerror("...");
        }
        
    }
};  // endof class MenuPage

template <BoardSize Size=BoardSize::Small>
class GuiDiplayer : public Displayer<Size> {
public:
    using base_type = Displayer<Size>;
    // DEFINE_SHAPES; DEFINE_SIZES;

    GuiDisplayer() : base_type() {}
    GuiDisplayer(const std::vector<std::vector<size_t>>& board_) : base_type(board_) {}
    
    Command get_command() const {
        page_->get_command();
    }
    void show() const override {
        // window refresh
    }
    // 除了displayer里面cmdboardframework更新的那一部分（用于log）
    // 还要加上自己在gui中展示的部分
    void remove_last_sp(const Piece& last_piece) override {
        base_type::remove_last_sp(last_piece);
        // replace sp piece pic with normal piece
    }
    void update_new_sp(const Piece& last_piece) override {
        base_type::update_new_sp(last_piece);
        // replace normal piece pic with sp piece
    }
    virtual void update() {
        // TODO
    }

private:
    Page memu1(  "Menu1",   "Menu - mode selection page");
    Page menu2(  "Menu2",   "Menu - size selection page");
    Page game(   "Game",    "Game playing page");
    Page victory("Victory", "Victory page");
    Page xq4gb(  "XQ4GB",   "XQ4GB - Q41");

     mode;
};  // 

}  // endof namespace mfwu

#endif  // __CMD_FRAMEWORK_HPP__