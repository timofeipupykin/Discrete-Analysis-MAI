#include <iostream>
#include <string>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <map>

using duration_t = std::chrono::microseconds;
const std::string DURATION_PREFIX = "us";

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}
void save_map(const std::map<std::string, uint64_t>& tree, const std::string& path) {
    std::ofstream out(path, std::ios::binary);
    size_t size = tree.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (const auto& [key, val] : tree) {
        size_t len = key.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(key.c_str(), len);
        out.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
}

void load_map(std::map<std::string, uint64_t>& tree, const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in || in.peek() == std::ifstream::traits_type::eof()) {
        tree.clear();
        return;
    }
    std::map<std::string, uint64_t> temp_tree;
    size_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    for (size_t i = 0; i < size; ++i) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        std::string key(len, ' ');
        in.read(&key[0], len);
        uint64_t val;
        in.read(reinterpret_cast<char*>(&val), sizeof(val));
        temp_tree[key] = val;
    }
    tree = std::move(temp_tree);
}

int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::map<std::string, uint64_t> tree;
    std::string cmd;

    auto start_ts = std::chrono::system_clock::now();

    while (std::cin >> cmd) {
        try {
            if (cmd == "+") {
                std::string key;
                uint64_t val;
                std::cin >> key >> val;
                std::string lower_key = to_lower(key);
                
                if (tree.find(lower_key) == tree.end()) {
                    tree[lower_key] = val;
                    std::cout << "OK\n";
                } else {
                    std::cout << "Exist\n";
                }
            } 
            else if (cmd == "-") {
                std::string key;
                std::cin >> key;
                if (tree.erase(to_lower(key))) {
                    std::cout << "OK\n";
                } else {
                    std::cout << "NoSuchWord\n";
                }
            } 
            else if (cmd == "!") {
                std::string action, path;
                std::cin >> action >> path;
                if (action == "Save") {
                    save_map(tree, path);
                    std::cout << "OK\n";
                } else {
                    load_map(tree, path);
                    std::cout << "OK\n";
                }
            } 
            else {
                auto it = tree.find(to_lower(cmd));
                if (it != tree.end()) {
                    std::cout << "OK: " << it->second << "\n";
                } else {
                    std::cout << "NoSuchWord\n";
                }
            }
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << "\n";
        }
    }

    auto end_ts = std::chrono::system_clock::now();
    uint64_t total_time = std::chrono::duration_cast<duration_t>(end_ts - start_ts).count();
    
    std::cerr << "Total execution std::map time: " << total_time << DURATION_PREFIX << std::endl;
    
    return 0;
}