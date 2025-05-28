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

struct Box {
public:
    using ptr = std::shared_ptr<Box>;
    using pic_type = int;  // TODO

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
