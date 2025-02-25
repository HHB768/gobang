#ifndef __ROBOTPLAYER_HPP__
#define __ROBOTPLAYER_HPP__

#include "Player.hpp"

namespace mfwu {

class RobotPlayer : Player {
public:
    void play() override {
        Position pos = get_best_position();
        this->place(pos);
    }

    void place(Position pos) override {
        return Player::place(pos);
    }

private:
    Position get_best_position();
};  // endof class RobotPlayer

}  // endof namespace mfwu

#endif // __ROBOTPLAYER_HPP__