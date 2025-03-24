#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

static std::unordered_map<int, char> mapping = {{0, '_'}, {1, 'O'}, {2, 'X'}};
static std::unordered_set<std::string> recs;

bool check(const std::vector<int>& a) {
    for (int i = 0; i < 4; i++) {
        if (a[i] == 2) {
            for (int j = 0; j < i; j++) {
                if (a[j] != 0) return false;
            }
        }
    }
    for (int i = 5; i < 9; i++) {
        if (a[i] == 2) {
            for (int j = 8; j > i; j--) {
                if (a[j] != 0) return false;
            }
        }
    }
    std::string s;
    for (int i = 0; i < 9; i++) {
        s += std::string(1, a[i]);
    }
    if (recs.count(s)) return false;
    std::reverse(s.begin(), s.end());
    recs.emplace(s);
    return true;
}
int main() {
    std::vector<int> a(9);
    for (a[0] = 0; a[0] < 3; a[0]++) {
    for (a[1] = 0; a[1] < 3; a[1]++) {
    for (a[2] = 0; a[2] < 3; a[2]++) {
    for (a[3] = 0; a[3] < 3; a[3]++) {
    // for (a[4] = 0; a[4] < 3; a[4]++) {
    for (a[5] = 0; a[5] < 3; a[5]++) {
    for (a[6] = 0; a[6] < 3; a[6]++) {
    for (a[7] = 0; a[7] < 3; a[7]++) {
    for (a[8] = 0; a[8] < 3; a[8]++) {
        if (!check(a)) {
            continue;
        }
        std::cout << mapping[a[0]] << " ";
        std::cout << mapping[a[1]] << " ";
        std::cout << mapping[a[2]] << " ";
        std::cout << mapping[a[3]] << " ";
        std::cout << '*' << " ";
        std::cout << mapping[a[5]] << " ";
        std::cout << mapping[a[6]] << " ";
        std::cout << mapping[a[7]] << " ";
        std::cout << mapping[a[8]] << "\n";
    }
    }
    }
    }
    // }
    }
    }
    }
    }


    return 0;
}