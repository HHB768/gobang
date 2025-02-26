# ifndef  __GAMECONTROLLER_HPP__
# define  __GAMECONTROLLER_HPP__

#include "utils.hpp"
#include "Player.hpp"
#include "ChessBoard.hpp"
#include "HumanPlayer.hpp"
#include "RobotPlayer.hpp"

namespace mfwu {

template <typename Player1_type, typename Player2_type, typename ChessBoard_type,
          typename std::enable_if<std::is_base_of<mfwu::Player, Player1_type>::value 
                                  && std::is_base_of<mfwu::Player, Player2_type>::value, int
                                 >::type = 0>
class GameController {};

template <typename Player1_type, typename Player2_type, BoardSize Size>
class GameController<Player1_type, Player2_type, GUIBoard<Size>> {
public:
    GameController() 
        : board_(new GUIBoard()), player1_(&board_), player2_(&board_),
          logger_(), archive_() {
        
    }
    ~GameController() {}
    
    void start() {}  // fork a process to do this
    void game_play_task() {}  // for thread
    void advance() {

    }
    bool check() {
        const Piece& p = board_.get_last_piece();
        
    }
private:
    std::shared_ptr<ChessBoard_base> board_;
    Player1_type player1_;
    Player2_type player2_;

    Logger logger_;
    std::stack<ChessBoard_type> archive_; 
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