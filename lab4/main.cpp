#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>

struct Pos {
    size_t line;
    size_t pos;
};

class FArray {
    std::vector<size_t> F;

public:
    explicit FArray(const std::vector<std::string>& pattern) {
        size_t n = pattern.size();
        if (n == 0) return;

        std::vector<size_t> Z(n, 0);
        size_t L = 0, R = 0;
        for (size_t i = 1; i < n; ++i) {
            if (i <= R) {
                Z[i] = std::min(R - i + 1, Z[i - L]);
            }
            while (i + Z[i] < n && pattern[Z[i]] == pattern[i + Z[i]]) {
                ++Z[i];
            }
            if (i + Z[i] - 1 > R) {
                L = i;
                R = i + Z[i] - 1;
            }
        }

        std::vector<size_t> sp_prime(n, 0);
        for (int64_t j = static_cast<int64_t>(n) - 1; j >= 1; --j) {
            if (Z[j] > 0) {
                size_t i = j + Z[j] - 1;
                sp_prime[i] = Z[j];
            }
        }

        F.resize(n + 2, 0);
        F[1] = 1;
        for (size_t i = 2; i <= n + 1; ++i) {
            F[i] = sp_prime[i - 2] + 1;
        }
    }

    inline size_t operator[](size_t p) const {
        return F[p];
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::string pattern_line;
    if (!std::getline(std::cin, pattern_line) || pattern_line.empty()) {
        return 0;
    }

    std::vector<std::string> pattern;
    std::string temp;
    for (char ch : pattern_line) {
        if (std::isspace(ch)) {
            if (!temp.empty()) {
                for (char &c : temp) c = std::tolower(c);
                pattern.push_back(temp);
                temp.clear();
            }
        } else {
            temp += ch;
        }
    }
    if (!temp.empty()) {
        for (char &c : temp) c = std::tolower(c);
        pattern.push_back(temp);
    }

    size_t n = pattern.size();
    if (n == 0) return 0;

    FArray F(pattern);

    std::vector<Pos> ring_buffer(n);

    size_t ring_idx = 0;
    size_t line_num = 1;

    size_t p = 1;

    std::string current_word;
    current_word.reserve(20); 

    auto process_word = [&](size_t l, size_t w) {
        if (current_word.empty()) return;

        for (char &c : current_word) c = std::tolower(c);

        ring_buffer[ring_idx] = {l, w};

        while (true) {
            if (p <= n && current_word == pattern[p - 1]) {
                p++;
                break;
            }
            if (p == 1) break;
            p = F[p];
        }

        if (p == n + 1) {
            size_t match_start_idx = (ring_idx + 1) % n;
            std::cout << ring_buffer[match_start_idx].line << ", " << ring_buffer[match_start_idx].pos << "\n";
            p = F[p];
        }

        ring_idx = (ring_idx + 1) % n;
        current_word.clear();
    };

    std::string line;
    while (std::getline(std::cin, line)) {
        size_t word_in_line = 1;

        for (size_t i = 0; i < line.length(); ++i) {

            char ch = line[i];

            if (std::isspace(ch)) {
                if (!current_word.empty()) {
                    process_word(line_num, word_in_line);
                    word_in_line++;
                }
            } else {
                current_word += ch;
            }
        }
        
        if (!current_word.empty()) {
            process_word(line_num, word_in_line);
        }

        line_num++;
    }

    return 0;
}