#define __GUI_MODE__  // compilation marco
#define __CMD_MODE__
#ifdef __GUI_MODE__
#undef __CMD_MODE__
#endif // __GUI_MODE__

#define __LOG_INFERENCE_ELSEWHERE__

#include "GameController.hpp"
#include <cstdlib>
using namespace mfwu;

int main() {
    while (true) {

#ifdef __CMD_MODE__
        const GameMode  mode = print_mode_choice_help_cmd();
        const BoardSize size = print_size_choice_help_cmd();
#else  // __GUI_MODE__
        const GameMode mode = print_mode_choice_help_gui();
        const BoardSize size = print_size_choice_help_gui();
#endif  // __CMD_MODE__

        // ONLY ONE GAMECONTROLLER
        std::unique_ptr<GameController_base_base> game = nullptr;

#ifdef __CMD_MODE__
#define __CHESSBOARD_TYPE__ CmdBoard
#else  // __GUI_BOARD__
#define __CHESSBOARD_TYPE__ GuiBoard
#endif
        using Human_type = HumanPlayer;
        // using Robot_type = DebugRobot;
        // using Robot_type = DummyRobot;
        using Robot_type = HumanLikeRobot;
        
        switch (mode) {
        case GameMode::PVE : {
            switch (size) {
            case BoardSize::Small : {
                game = std::make_unique<GameController<Human_type, Robot_type, 
                                        __CHESSBOARD_TYPE__<BoardSize::Small>>>();
            }; break;
            case BoardSize::Middle : {
                game = std::make_unique<GameController<Human_type, Robot_type,
                                        __CHESSBOARD_TYPE__<BoardSize::Middle>>>();
            }; break;
            case BoardSize::Large : {
                game = std::make_unique<GameController<Human_type, Robot_type, 
                                        __CHESSBOARD_TYPE__<BoardSize::Large>>>();
            }; break;
            default:
                gc_error_exit(mode, size);
            }; break;
        }; break;
        case GameMode::PVP : {
            switch (size) {
            case BoardSize::Small : {
                game = std::make_unique<GameController<Human_type, Human_type, 
                                        __CHESSBOARD_TYPE__<BoardSize::Small>>>();
            }; break;
            case BoardSize::Middle : {
                game = std::make_unique<GameController<Human_type, Human_type, 
                                        __CHESSBOARD_TYPE__<BoardSize::Middle>>>();
            }; break;
            case BoardSize::Large : {
                game = std::make_unique<GameController<Human_type, Human_type, 
                                        __CHESSBOARD_TYPE__<BoardSize::Large>>>();
            }; break;
            default:
                gc_error_exit(mode, size);
            }; break;
        }; break;
        case GameMode::EVE : {
            switch (size) {
            case BoardSize::Small : {
                game = std::make_unique<GameController<Robot_type, Robot_type, 
                                        __CHESSBOARD_TYPE__<BoardSize::Small>>>();
            }; break;
            case BoardSize::Middle : {
                game = std::make_unique<GameController<Robot_type, Robot_type, 
                                        __CHESSBOARD_TYPE__<BoardSize::Middle>>>();
            }; break;
            case BoardSize::Large : {
                game = std::make_unique<GameController<Robot_type, Robot_type, 
                                        __CHESSBOARD_TYPE__<BoardSize::Large>>>();
            }; break;
            default:
                gc_error_exit(mode, size);
            }; break;
        }; break;
        default:
            gc_error_exit(mode, size);
        }
        
        // TODO: 
        // auto pid = fork();
        // if (pid == 0) {
            bool ret2memu_flag = false;
            while (!ret2memu_flag) {
                GameStatus status = game->start();
                switch (status) {
                case GameStatus::RESTART : {
                    game->restart_game_init();
                } break;
                case GameStatus::NORMAL : {
                    std::cout << HELPER_PRESS_ANY_KEY << "\n";
                    game->abrupt_flush(status);
                    sleep(1);  // TODO: del this after transfering to win platf
                    fgetc(stdin);
                    game->reset_game_init();
                } break;
                case GameStatus::MENU : {
                    ret2memu_flag = true;
                    game->abrupt_flush(status);
                    break;
                } break;
                case GameStatus::QUIT : {
                    game->abrupt_flush(status);
                    exit(0);
                } break;
                default:
                    game->abrupt_flush(status);
                    logerr_unknown_game_status();
                    exit(-1);
                }
            }
        //     exit(0);
        // }
        // int status = 42;
        // wait(&status);

        // if (WIFEXITED(status)){
        //     printf("fork exit success status code is %d\n",WEXITSTATUS(status));
        // } else if (WIFSIGNALED(status)){
        //     printf("fork was killed by %d\n",WTERMSIG(status));
        // } else if (WIFSTOPPED(status)){
        //     printf("fork was stop by %d\n",WSTOPSIG(status));
        // }
        // // return to help menu
        // if (status) { std::cerr << "last game ends with " << status << "\n"; }
        // // TODO: quit func
    }

    return 0;
}

// TODO: check this->