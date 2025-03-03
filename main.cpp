#include "GameController.hpp"
using namespace mfwu;

#define __CMD_MODE__

int main() {

    while (true) {

#ifdef __CMD_MODE__
        GameMode  mode = print_mode_choice_help_cmd();
        BoardSize size = print_size_choice_help_cmd();
#else  // GUI_HELP
        GameMode mode = print_mode_choice_help_gui();
        BoardSize size = print_size_choice_help_gui();
#endif  // __CMD_MODE__

        GameController_base_base* game = nullptr;
    // use std::unique_ptr, but nonsense

#ifdef __CMD_MODE__
#define __CHESSBOARD_TYPE__ CMDBoard
        using Human_type = HumanPlayer;
#else 
#define __CHESSBOARD_TYPE__ GUIBoard
        using Human_type = HumanPlayer;
#endif

        using Robot_type = DebugRobot;
        

        switch (mode) {
        case GameMode::PVE : {
            switch (size) {
            case BoardSize::Small : {
                game = new GameController<Human_type, Robot_type, __CHESSBOARD_TYPE__<BoardSize::Small>>();
            }; break;
            case BoardSize::Middle : {
                game = new GameController<Human_type, Robot_type, __CHESSBOARD_TYPE__<BoardSize::Middle>>();
            }; break;
            case BoardSize::Large : {
                game = new GameController<Human_type, Robot_type, __CHESSBOARD_TYPE__<BoardSize::Large>>();
            }; break;
            default:
                std::cerr << NEW_GC_ERROR << "\n";
            }; break;
        }; break;
        case GameMode::PVP : {
            switch (size) {
            case BoardSize::Small : {
                game = new GameController<Human_type, Human_type, __CHESSBOARD_TYPE__<BoardSize::Small>>();
            }; break;
            case BoardSize::Middle : {
                game = new GameController<Human_type, Human_type, __CHESSBOARD_TYPE__<BoardSize::Middle>>();
            }; break;
            case BoardSize::Large : {
                game = new GameController<Human_type, Human_type, __CHESSBOARD_TYPE__<BoardSize::Large>>();
            }; break;
            default:
            std::cerr << NEW_GC_ERROR << "\n";
            }; break;
        }; break;
        case GameMode::EVE : {
            switch (size) {
            case BoardSize::Small : {
                game = new GameController<Robot_type, Robot_type, __CHESSBOARD_TYPE__<BoardSize::Small>>();
            }; break;
            case BoardSize::Middle : {
                game = new GameController<Robot_type, Robot_type, __CHESSBOARD_TYPE__<BoardSize::Middle>>();
            }; break;
            case BoardSize::Large : {
                game = new GameController<Robot_type, Robot_type, __CHESSBOARD_TYPE__<BoardSize::Large>>();
            }; break;
            default:
            std::cerr << NEW_GC_ERROR << "\n";
            }; break;
        }; break;
        default:
        std::cerr << NEW_GC_ERROR << "\n";
        }
        
        auto pid = fork();
        if (pid == 0) {
            bool ret_memu_flag = false;
            while (!ret_memu_flag) {
                GameStatus status = game->start();
                switch (status) {
                case GameStatus::RESTART : {
                    while (status == GameStatus::RESTART) {
                        GameStatus status = game->start();
                    }
                } /*break;*/
                case GameStatus::NORMAL : {
                    game->reset_game();
                } break;
                case GameStatus::MENU : {
                    ret_memu_flag = true;
                    break;
                } break;
                case GameStatus::QUIT : {
                    exit(1);
                } break;
                default:
                    std::cerr << "...\n";
                    exit(-1);
                }
            }
            exit(0);
        }
        int status;
        wait(&status);
        // return to help menu
        if (status) { std::cerr << "last game ends with " << status << "\n"; }
    }

    return 0;
}