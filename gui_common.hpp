#ifndef __GUI_COMMON_HPP__
#define __GUI_COMMON_HPP__

#include "common.hpp"

namespace mfwu {

struct PositionPix {
    int hpix, vpix;
    PositionPix() : hpix(0), vpix(0) {}
    PositionPix(int h, int v) : hpix(h), vpix(v) {}
    PositionPix(const PositionPix& pos) = default;
    ~PositionPix() {}
};  // endof struct PositionPix

enum class PageType : size_t {
    GamePlayPage = 0,
    BlackVickPage = 1,
    WhiteVickPage = 2,
    Menu1 = 3,
    Menu2 = 4
};  // endof enum class PageType

class PICTURE {};

class Picture {
public:
    Picture(const char* filename, int height=1, int width=1) {
        load(filename, height, width);
    }
    Picture(const std::string& filename, int height=1, int width=1) {
        load(filename, height, width);
    }
    Picture(PICTURE pic, int height, int width) 
        : pic_(pic), height_(height), width_(width) {}
    
    void load(const char* filename, int height, int width) {
        
    }
    void load(const std::string& filename, int height, int width) {
        
    }
    void resize(int height, int width) {
        height_ = height;
        width_ = width;
    }

private:
    PICTURE pic_;
    int height_;
    int width_;
};  // endof class Picture

constexpr const char* res_path = "./resource/";
const std::string pic_path = std::string(res_path) + "pictures/";

constexpr size_t BoardSize2PiecePicSize(BoardSize size) {
    switch (size) {
    case BoardSize::Small : {
        return 30UL;
    } break;
    case BoardSize::Middle : {
        return 25UL;
    } break;
    case BoardSize::Large : {
        return 20UL;
    } break;
    default : 
        std::cerr << "Unknow BoardSize, cannot get piece pic size\n";
    }
    return 0x10UL;
}

struct BoardDimension {
    size_t height;
    size_t width;
};  // endof struct BoardDimension

constexpr BoardDimension BoardSize2CmdPicSize(BoardSize size) {
    size_t height = 50, width = 20;
    switch (size) {
    case BoardSize::Small : {
        height = 50UL;
    } break;
    case BoardSize::Middle : {
        height = 45UL;
    } break;
    case BoardSize::Large : {
        height = 40UL;
    } break;
    default : 
        std::cerr << "Unknow BoardSize, cannot get cmd pic size\n";
        height = 10UL;
    }
    return { height, width };
}

constexpr BoardDimension BoardSize2BoardPicSize(BoardSize size) {
    size_t bsz = BoardSize2PiecePicSize(size) * (static_cast<size_t>(size) + 3);
    size_t csz = BoardSize2CmdPicSize(size).width * 2;
    return { bsz, bsz + csz };
}

const Picture chessboard_pic    (pic_path + "chessboard.png"    , 99, 99);  // resize
const Picture white_piece_pic   (pic_path + "white_piece.png"   , 10, 10);
const Picture black_piece_pic   (pic_path + "black_piece.png"   , 10, 10);
const Picture white_piece_sp_pic(pic_path + "white_piece_sp.png", 10, 10);
const Picture black_piece_sp_pic(pic_path + "black_piece_sp.png", 10, 10);
const Picture helper_mode_pic   (pic_path + "helper_mode.png"   , 20, 99);
const Picture helper_size_pic   (pic_path + "helper_size.png"   , 20, 99);

const Picture option_mode_pics[3] = {
    Picture(pic_path + "option_mode_1.png" , 20, 33),
    Picture(pic_path + "option_mode_2.png" , 20, 33),
    Picture(pic_path + "option_mode_3.png" , 20, 33)
};
const Picture option_size_pics[3] = {
    Picture(pic_path + "option_size_1.png" , 20, 33),
    Picture(pic_path + "option_size_2.png" , 20, 33),
    Picture(pic_path + "option_size_3.png" , 20, 33)
};

constexpr int num_of_vick_pic = 5;
const Picture vick_pics[num_of_vick_pic] = {
    Picture(pic_path + "option_size_1.png" , 20, 33),
    Picture(pic_path + "option_size_1.png" , 20, 33),
    Picture(pic_path + "option_size_1.png" , 20, 33),
    Picture(pic_path + "option_size_1.png" , 20, 33),
    Picture(pic_path + "option_size_1.png" , 20, 33)
};


struct Box {
public:
    using ptr = std::shared_ptr<Box>;
    using pic_type = mfwu::Picture;  // TODO
    Box(int x0_, int x1_, int y0_, int y1_, 
        CommandType cmd_type_, const pic_type& pic_)
        : x0(x0_), x1(x1_), y0(y0_), y1(y1_), 
          cmd_type(cmd_type_), pics(1, pic_) {
        assert(x0_ < x1_ && y0_ < y1_);
    }
    Box(int x0_, int x1_, int y0_, int y1_, 
        CommandType cmd_type_, const std::vector<pic_type>& pics_)
        : x0(x0_), x1(x1_), y0(y0_), y1(y1_), 
          cmd_type(cmd_type_), pics(pics_) {
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

    void show() const {
        // show pic
    }
    const std::vector<pic_type>& get_pics() const {
        return pics;
    }

private:
    int x0, x1, y0, y1;
    const CommandType cmd_type;
    std::vector<pic_type> pics;
};  // endof struct Box


class Page {
public:
    using ptr = std::shared_ptr<Page>;
    using pic_type = Box::pic_type;
    Page(PageType type, const std::string& str1="") 
        : type_(type), description_(str1) {}

    PageType get_type() const { return type_; }
    std::string get_description() const { return description_; }
    void append(typename Box::ptr b) {
        boxes_.push_back(b);
    }
    virtual void show() const {
        for (const typename Box::ptr& box : boxes_) {
            box->show();
        }
    }
    virtual Command get_command() {
        const PositionPix pos = wait_input();
        for (const typename Box::ptr& box : boxes_) {
            if (box->encircle(pos)) {
                return box->get_cmd(pos);
            }
        }
        return Command{CommandType::INVALID, {}};
    } 
    typename Box::ptr get_box(int idx) const {
        assert(0 <= idx && idx < boxes_.size());
        return boxes_[idx];
    }

private:
    PositionPix wait_input() {
        return {};
    }

    PageType type_;
    std::string description_;
    std::vector<typename Box::ptr> boxes_;
    // recommended: std::unordered_map<std::string, typename Box::ptr> boxes_;
};  // endof class Page



}  // endof namespace mfwu

#endif  // __GUI_COMMON_HPP__
