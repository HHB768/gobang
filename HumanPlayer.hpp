#ifndef __HUMANPLAYER_HPP_
#define __HUMANPLAYER_HPP_

#include "Player.hpp"

namespace mfwu {

class HumanPlayer : Player {
    void play() override {
        Position pos = get_pos();
        this->place(pos);
    }

    void place(Position pos) override {
        return Player::place(pos);
    }
private:
    Position get_pos();
};  // endof class HumanPlayer

}  // endof namespace mfwu

#endif  // __HUMANPLAYER_HPP__