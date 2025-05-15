#include <iostream>
#include "Xq4Printer.hpp"
#include <unistd.h>
#include <sstream>

void clear_and_print_once(const mfwu::Xq4Printer::M_type& res) {
    std::cout << "\033[2J\033[1;1H";
    std::stringstream ss;
    for (const auto& line : res) {
        for (const auto& ch : line) {
            if (ch == 1) {
                ss << "♣";
            } else {
                ss << "~";
            }
        }
        ss << "\n";
    } 
    std::cout << ss.str();
}

int main() {
    auto res = mfwu::Xq4Printer::get_xq4_matrix();
    for (int i = 0; i < 551; i++) {
        clear_and_print_once(res);
        mfwu::Xq4Printer::roll(res, 1);
        usleep(50000);
    }
    return 0;
}