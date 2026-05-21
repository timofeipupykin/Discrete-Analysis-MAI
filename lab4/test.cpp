#include <vector>
#include <string>
#include <iostream>

std::vector<int> is_anagram(std::string s, std::string p) {
    int n = s.size();
    int m = p.size();
    std::vector<int> pattern(26, 0);
    std::vector<int> count_pattern(26, 0);
    std::vector<int> result;

    for (auto& el : p) {
        ++pattern[el - 'a'];
    }

    for (int i = 0; i < m; ++i) {
        ++count_pattern[s[i] - 'a'];
    }

    if (count_pattern == pattern) {
        result.push_back(0);
    }

    for (int i = 1; i <= n - m; ++i) {
        --count_pattern[s[i - 1] - 'a'];
        ++count_pattern[s[i + m - 1] - 'a'];
        if (count_pattern == pattern) {
            result.push_back(i);
        }
    }

    return result;
}


int main() {
    std::string s = "cbacasdhjbca";
    std::string p = "abc";

    for (auto& el : is_anagram(s, p)) {
        std::cout << el << "\n";
    }
}