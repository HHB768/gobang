# ifndef  __GAMECONTROLLER_HPP__
# define  __GAMECONTROLLER_HPP__

#include "common.hpp"
#include "Player.hpp"
#include "ChessBoard.hpp"
#include "HumanPlayer.hpp"
#include "RobotPlayer.hpp"
#include "Logger.hpp"
#include "Archive.hpp"
#include <sys/wait.h>

namespace mfwu {

// interfaces exposed to main func
class GameController_base_base {
public:
    virtual GameStatus start() = 0;
    virtual void restart_game_init() = 0;
    virtual void reset_game_init() = 0;
    virtual void abrupt_flush(GameStatus status) = 0;
};  // endof class GameController_base_base

template <typename Player1_type, typename Player2_type, typename ChessBoard_type>  // TODO: type check
class GameController_base : public GameController_base_base {
public:
    GameController_base() 
        : board_(std::make_shared<ChessBoard_type>()), 
          player1_(board_, Piece::Color::Black), 
          player2_(board_, Piece::Color::White),
          current_player_(&player1_),
          idle_player_(&player2_),
          player1_first_(true),
          /*logger_(),*/ archive_() {
        _gc_init_();
    }
    virtual ~GameController_base() {}
    
    virtual GameStatus start() {
        log_new_game(board_->size(), board_->size());
        this->board_->show();
        CommandType cmd_type;
        // std::thread t(&GameController_base::game_play_task, this, std::ref(cmd_type));  // we really need this?
        this->game_play_task(cmd_type);
        // t.join();
        switch (cmd_type) {
        case CommandType::PIECE : {
            return GameStatus::NORMAL;
        } break;
        case CommandType::RESTART : {
            return GameStatus::RESTART;
        } break;
        case CommandType::MENU : {
            return GameStatus::MENU;
        } break;
        case CommandType::QUIT: {
            return GameStatus::QUIT;
        } break;
        case CommandType::INVALID: {
            if (this->check_draw()) {
                log_info("--- Draw ---");
                return GameStatus::NORMAL;
            }
        } break;
        default:
            log_error("Game end with cmd type: %lu", 
                      static_cast<size_t>(cmd_type));
        }
        return GameStatus::INVALID;
    }
    
    virtual void game_play_task(CommandType& cmd_type) {
        do {
            cmd_type = this->advance();
            if (cmd_type != CommandType::PIECE) {
                if (cmd_type == CommandType::XQ4GB) {
                    // log_new_game();
                    archive_.flush(GameStatus::XQ4GB);
                    execl("./xq4gb", "xq4gb", NULL);
                    exit(0x3F3F3F3F);
                }
                return ;
            }
        } while (!this->check_end());
        // only is_end() == true comes here
        winner_display(idle_player_->get_color());
    }
    
    virtual CommandType advance() {
        CommandType cmd_type = current_player_->play();
        if (cmd_type == CommandType::PIECE) {
            board_->refresh();
            Player* temp = current_player_;
            current_player_ = idle_player_;
            idle_player_ = temp;
            // log_info("step");
            archive_.record(board_->serialize());
        }
        return cmd_type;
    }

    virtual bool check_end() const {
        const Piece& p = this->board_->get_last_piece();
        count_res_4 res;
        this->board_->count_dir(p, &res);
        if (is_end(res)) { return true; }
        else return false;
    }
    virtual bool check_draw() const {
        return this->board_->is_full();
    }
    virtual void restart_game_init() override {
        log_end_game(GameStatus::RESTART);
        archive_.flush(GameStatus::RESTART);
        
        log_new_game(board_->size(), board_->size());
        board_->reset();
        // board_->show();
        // doesnt swap
        if (player1_first_) {
            current_player_ = &player1_;
            idle_player_ = &player2_;
        } else {
            current_player_ = &player2_;
            idle_player_ = &player1_;
        }
    }
    virtual void reset_game_init(GameStatus status) {
        abrupt_flush(status);
        reset_game_init();
    }
    virtual void reset_game_init() override {
        log_new_game(board_->size(), board_->size());
        board_->reset();
        // board_->show();
        std::swap(player1_.get_color(), player2_.get_color());
        player1_first_ = !player1_first_;
        if (player1_first_) {
            current_player_ = &player1_;
            idle_player_ = &player2_;
        } else {
            current_player_ = &player2_;
            idle_player_ = &player1_;
        }
    }
    virtual void abrupt_flush(GameStatus status) override {
        log_end_game(status);
        archive_.flush(status);
    }
protected:
    virtual void winner_display(const Piece::Color& color) const {
        board_->winner_display(color);
    }
private:
    virtual void _gc_init_() {
        // in init list now
        // player1_first_ = true;
        // current_player_ = &player1_;
        // idle_player_ = &player2_;
        // log_new_game();
        // archive_.flush();

        // log
        log_info("game controller inits...");
        log_debug("------------------------------");
        log_debug("player1 color: %s", 
                  (Piece::get_real_status(player1_.get_color_const()) 
                   == Piece::get_real_status(Piece::Color::White) ? "White" : "Black"));
        log_debug("player2 color: %s", 
                  (Piece::get_real_status(player2_.get_color_const()) 
                   == Piece::get_real_status(Piece::Color::White) ? "White" : "Black"));
        log_debug("player1 plays first: %s", (player1_first_ ? "True" : "False"));
        log_debug("current player: %s", (current_player_ == &player1_ ? "Player1" : "Player2"));
        log_debug("idle_:D player: %s", (idle_player_ == &player1_ ? "Player1" : "Player2"));  // :D
        if (archive_.get_status() == true) {
            log_debug("archive status: online");
        } else {
            log_error("archive status: offline");
            log_error(XQ4GB_TIMESTAMP, "your archive may be lost");
        }
        log_debug("------------------------------");
        // show CHECK: we really need this?
        // board_->show();
    }
    static bool is_end(const count_res_4& res) {
        if (res.left_right >= NoPtW - 1
            || res.up_down >= NoPtW - 1
            || res.up_left_down_right >= NoPtW - 1
            || res.up_right_down_left >= NoPtW - 1) {
            return true;
        } else return false;
    }
    
    std::shared_ptr<ChessBoard_base> board_;
    Player1_type player1_;
    Player2_type player2_;
    Player* current_player_;  // doesnt alloc any mem
    Player* idle_player_;
    bool player1_first_;

    Archive<ChessBoard_type> archive_;
    // std::vector<typename ChessBoard_type::Archive_type> archive_; 

};  // endof class GameController_base

// 250418 xq42: actually we dont need this...
// and maybe we should remove 'virtual' qualifiers
// in gc_base class def
template <typename Player1_type, typename Player2_type, typename ChessBoard_type>
class GameController : public GameController_base<Player1_type, Player2_type, ChessBoard_type> {
// private, not used
    GameController() : GameController_base<Player1_type, Player2_type, ChessBoard_type>() {}
    virtual ~GameController() {}
};

template <typename Player1_type, typename Player2_type, BoardSize Size>
class GameController<Player1_type, Player2_type, GuiBoard<Size>> 
    : public GameController_base<Player1_type, Player2_type, GuiBoard<Size>> {
public:
    GameController<Player1_type, Player2_type, GuiBoard<Size>>()
        : GameController_base<Player1_type, Player2_type, GuiBoard<Size>>() {}
    virtual ~GameController<Player1_type, Player2_type, GuiBoard<Size>>() {}
};


template <typename Player1_type, typename Player2_type, BoardSize Size>
class GameController<Player1_type, Player2_type, CmdBoard<Size>> 
    : public GameController_base<Player1_type, Player2_type, CmdBoard<Size>> {
public:
    GameController<Player1_type, Player2_type, CmdBoard<Size>>() 
        : GameController_base<Player1_type, Player2_type, CmdBoard<Size>>() {}
    virtual ~GameController<Player1_type, Player2_type, CmdBoard<Size>>() {}
    
private:
    
};
}  // endof namespace mfwu

#endif  // __GAMECONTROLLER_HPP__