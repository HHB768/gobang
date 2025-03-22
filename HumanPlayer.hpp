#ifndef __HUMANPLAYER_HPP_
#define __HUMANPLAYER_HPP_

#include "Player.hpp"

namespace mfwu {

class HumanPlayer : public Player {
public:
    HumanPlayer() : Player() {}
    HumanPlayer(std::shared_ptr<ChessBoard_base> board, Piece::Color color) 
        : Player(board, color) {}

    virtual CommandType play() override {
        Command cmd = this->board_->get_command();
        log_debug("Human player gets cmd: %s", 
                  CommandTypeDescription.at(static_cast<size_t>(cmd.type)));
        switch (cmd.type) {
        case CommandType::PIECE : {
            this->place(cmd.pos);
            log_debug("Human's pos: [%d, %d]", cmd.pos.row, cmd.pos.col);
        } break;
        case CommandType::RESTART : {
        } break;
        case CommandType::MENU : {
        } break;
        case CommandType::QUIT : {
        } break;
        case CommandType::XQ4GB: {
            log_info(                 "XQ41-GB cheater begins...");
            log_info(XQ4GB_TIMESTAMP, ">>>>>>>>>>>>>>>>>>>>>>>>>");
            log_info(XQ4GB_TIMESTAMP, ":D :) XD TT >.< -^- (: zz");
            sleep(2);
        } break;
        default :
            logerr_unknown_cmdtype();
        }
        return cmd.type;
    }

    void place(const Position& pos) override {
        return Player::place(pos);
    }
protected:
    // virtual Command get_command() = 0;
    // 不应该由你来实现吧，不然真要写GUIplayer和CMDplayer你又不乐意
    // XQ4 25.03.01
};  // endof class HumanPlayer

// deprecated
// class GUIPlayer : public HumanPlayer {
// private:
    
// };  // endof class GUIPlayer

// class CMDPlayer : public HumanPlayer {

// };  // endof class CMDPlayer

// TODO: do we really need this?
//       or we can wait_movement() and call chessboard(GUI/CMD) to monitor the human input

}  // endof namespace mfwu

#endif  // __HUMANPLAYER_HPP__