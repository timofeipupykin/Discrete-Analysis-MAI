#include <iostream>
#include <string>
#include <vector>

struct Pair {
    std::string key;
    std::string value;
    std::string clear_key;

    friend std::ostream& operator<<(std::ostream& os, const Pair& pair) {
        os << pair.clear_key << "|" << pair.key << "\t" << pair.value;
        return os;
    }
};


std::vector<Pair> CountingSort(std::vector<Pair> pairs, int dig) {
    std::vector<int> extra(10, 0);
    for (int i = 0; i < pairs.size(); i++) {
        std::string& cur_key = pairs[i].clear_key;
        
        if (dig >= cur_key.size()) {
            cur_key.insert(0, dig - cur_key.size() + 1, '0');
        }
        
        char ch = cur_key[dig];
        int index = (ch - '0');
        extra[index]++;
    }

    for (int i = 1; i < extra.size(); i++) {
        extra[i] += extra[i - 1];
    }
    
    std::vector<Pair> output(pairs.size());
    for (int i = pairs.size() - 1; i >= 0; i--) {
        std::string& cur_key = pairs[i].clear_key;
        if (dig >= cur_key.size()) {
            cur_key.insert(0, dig - cur_key.size() + 1, '0');
        }
        
        char ch = cur_key[dig];
        int index = (ch - '0');
        extra[index]--;
        output[extra[index]] = pairs[i];
    }
    return output;
}


std::vector<Pair> RadixSort(const std::vector<Pair> pairs) {
    std::vector<Pair> output = pairs;
    int max_size = 0;
    for (int i = 0; i < output.size(); i++) {
        int cur_size = output[i].clear_key.size();
        if (cur_size >= max_size) {
            max_size = cur_size;
        }
    }

    for (int dig = max_size - 1; dig >= 0; dig--) {
        output = CountingSort(output, dig);
    }

    return output;
}


int main() {
    std::vector<Pair> pairs;
    std::string input;
    std::cout << "Enter a key-value pair (key value):\n";

    while (std::getline(std::cin, input)) {
        if (input.empty()) continue;

        size_t delimiterPos = input.find('\t');
        if (delimiterPos != std::string::npos) {
            Pair pair;
            pair.key = input.substr(0, delimiterPos);

            for (int i = 0; i < pair.key.size(); i++) {
                if (pair.key[i] < '0' || pair.key[i] > '9') {
                    continue;
                }
                pair.clear_key += pair.key[i];
            }
            
            size_t valueStart = input.find_first_not_of(' ', delimiterPos);
            if (valueStart != std::string::npos) {
                pair.value = input.substr(valueStart);
                pairs.push_back(pair);
            }
        } else {
            std::cout << "Invalid input. Please enter in the format key value." << std::endl;
        }
    }

    pairs = RadixSort(pairs);

    std::cout << "Sorted key-value pairs:\n";
    
    for (const auto& pair : pairs) {
        std::cout << pair << "\n";
    }
}
