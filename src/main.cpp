#include <iostream>
#include <string>
#include <vector>

struct Pair {
    std::string key;
    std::string value;
    std::string clear_key;

    Pair() = default;

    Pair(std::string str) {
        size_t delimiterPos = str.find('\t');

        if (delimiterPos == std::string::npos) {
            throw std::invalid_argument("Invalid input format. Expected 'key\tvalue'");
        }

        key = str.substr(0, delimiterPos);

        for (int i = 0; i < key.size(); i++) {
            if (key[i] < '0' || key[i] > '9') {
                continue;
            }
            clear_key += key[i];
        }
        
        size_t valueStart = str.find_first_not_of('\t', delimiterPos);
        if (valueStart != std::string::npos) {
            value = str.substr(valueStart);
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Pair& pair) {
        os << pair.key << "\t" << pair.value;
        return os;
    }
};


std::vector<Pair> CountingSort(const std::vector<Pair>& pairs, int dig, int max_size) {
    std::vector<int> extra(10, 0);
    std::vector<Pair> output(pairs.size());

    auto getDigit = [&](const std::string& key) {
            int shift = max_size - key.size();
            if (dig < shift) {
                return 0;
            }
            return key[dig - shift] - '0';
    };

    for (int i = 0; i < pairs.size(); i++) {
        int index = getDigit(pairs[i].clear_key);
        extra[index]++;
    }

    for (int i = 1; i < extra.size(); i++) {
        extra[i] += extra[i - 1];
    }
    
    for (int i = pairs.size() - 1; i >= 0; i--) {
        int index = getDigit(pairs[i].clear_key);
        extra[index]--;
        output[extra[index]] = pairs[i];
    }
    return output;
}


std::vector<Pair> RadixSort(const std::vector<Pair>& pairs) {
    std::vector<Pair> output = pairs;

    int max_size = 0;
    for (int i = 0; i < output.size(); i++) {
        int cur_size = output[i].clear_key.size();
        if (cur_size >= max_size) {
            max_size = cur_size;
        }
    }

    for (int dig = max_size - 1; dig >= 0; dig--) {
        output = CountingSort(output, dig, max_size);
    }

    return output;
}


int main() {
    std::vector<Pair> pairs;
    std::string input;
    std::cout << "Enter a key-value pairs (key value):\n";

    while (std::getline(std::cin, input)) {
        if (input.empty()) continue;

        Pair pair = Pair(input);
        pairs.push_back(pair);
    }

    pairs = RadixSort(pairs);

    std::cout << "Sorted key-value pairs:\n";
    
    for (const auto& pair : pairs) {
        std::cout << pair << "\n";
    }
}
