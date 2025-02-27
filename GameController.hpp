# ifndef  __GAMECONTROLLER_HPP__
# define  __GAMECONTROLLER_HPP__

#include "utils.hpp"
#include "Player.hpp"
#include "ChessBoard.hpp"
#include "HumanPlayer.hpp"
#include "RobotPlayer.hpp"

namespace mfwu {

class GameController_base_base {};

template <typename Player1_type, typename Player2_type, typename ChessBoard_type,
          typename std::enable_if<std::is_base_of<mfwu::Player, Player1_type>::value 
                                  && std::is_base_of<mfwu::Player, Player2_type>::value, int
                                 >::type = 0>
class GameController_base : GameController_base_base {
public:
    GameController_base() 
        : board_(new ChessBoard_type()), 
          player1_(&board_, Piece::Color::White), 
          player2_(&board_, Piece::Color::Black),
          current_player_(player1_),
          current_player_(player2_),
          logger_(), archive_() {
        // ?
    }
    ~GameController_base() {}
    
    virtual void start() {
        auto pid = fork();
        if (pid == 0) {
            game_play_task();
            exit(0);
        } else {
            int status;
            wait(&status);
            if (status) {
                std::cerr << "game end with status: " << status << "\n"; 
            }
        }
        reinit(); start();
    }
    
    virtual void game_play_task() {
        while (!stop_flag_ && !check_end()) {
            advance();
        }
    }
    
    virtual void advance() {
        current_player_->play();
        board_->fresh();
        std::swap(current_player_, idle_player_);
    }

    virtual bool check_end() const {
        const Piece& p = board_.get_last_piece();
        ChessBoard_type::count_res_4 res;
        board_->count_dir(p, &res);
        if (is_end(res)) { return true; }
        else return false;
    }
private:
    static bool is_end(const count_res_4& res) {
        if (res.left_right >= 5
            || res.up_down >= 5
            || res.up_left_down_right >= 5
            || res.up_right_down_left >= 5) {
            return true;
        } else return false;
    }

    std::shared_ptr<ChessBoard_base> board_;
    Player1_type player1_;
    Player2_type player2_;
    Player& current_player_;
    Player& idle_player_;

    Logger logger_;
    std::vector<typename ChessBoard_type::Archive_type> archive_; 

};  // endof class GameController_base

template <typename Player1_type, typename Player2_type, typename ChessBoard_type>
class GameController : public GameController_base {};

template <typename Player1_type, typename Player2_type, BoardSize Size>
class GameController<Player1_type, Player2_type, GUIBoard<Size>> {

};


template <typename Player1_type, typename Player2_type, BoardSize Size>
class GameController<Player1_type, Player2_type, CMDBoard<Size>> {
public:
    GameController() : board_(new CMDBoard()), player1_(&board_), player2_(&board_) {
        
    }
    ~GameController() {}
    
    void start();  // fork a process to do this
    void game_play_task();  // for thread
    void advance();
    bool check();
private:
    std::shared_ptr<ChessBoard_base> board_;
    Player1_type player1_;
    Player2_type player2_;

    Logger logger_;
    std::stack<ChessBoard_type> archive_; 
};
}  // endof namespace mfwu

#endif  // __GAMECONTROLLER_HPP__