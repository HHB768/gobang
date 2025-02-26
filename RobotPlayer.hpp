#ifndef __ROBOTPLAYER_HPP__
#define __ROBOTPLAYER_HPP__

#include "Player.hpp"

namespace mfwu {

class RobotPlayer : Player {
public:
    virtual void play() override {
        Position pos = get_best_position();
        this->place(pos);
    }

    void place(const Position& pos) override {
        Player::place(pos);
    }

private:
    Position get_best_position();
};  // endof class RobotPlayer

class DebugRobot : RobotPlayer {

};  // endof class DebugRobot;

class DummyRobot : RobotPlayer {

};  // endof class DummyRobot

class HumanLikeRobot : RobotPlayer {

};  // endof class HumanLikeRobot

class SmartRobot : RobotPlayer {

};  // enof class SmartRobot


}  // endof namespace mfwu

#endif // __ROBOTPLAYER_HPP__