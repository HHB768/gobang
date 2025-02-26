#ifndef __ROBOTPLAYER_HPP__
#define __ROBOTPLAYER_HPP__

#include "Player.hpp"

namespace mfwu {

class RobotPlayer : public Player {
public:
    virtual void play() override {
        Position pos = get_best_position();
        this->place(pos);
    }

    void place(const Position& pos) override {
        Player::place(pos);
    }

protected:
    virtual Position get_best_position() = 0;
};  // endof class RobotPlayer

class DebugRobot : public RobotPlayer {
private:
    Position get_best_position() const override {
        size_t sz = this->board_->size();
    }
};  // endof class DebugRobot;

class DummyRobot : public RobotPlayer {

};  // endof class DummyRobot

class HumanLikeRobot : public RobotPlayer {

};  // endof class HumanLikeRobot

class SmartRobot : public RobotPlayer {

};  // enof class SmartRobot


}  // endof namespace mfwu

#endif // __ROBOTPLAYER_HPP__