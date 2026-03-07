#include "main.hpp"
#include <chrono>
#include <algorithm>


using duration_t = std::chrono::microseconds;
const std::string DURATION_PREFIX = "us";

/*
using duration_t = std::chrono::milliseconds;
const std::string DURATION_PREFIX = "ms";
*/

int main() {
    Vector<Pair> pairs;
    Vector<Pair> pairsSTL;
    std::string input;

    while (std::getline(std::cin, input)) {
        if (input.empty()) continue;

        pairs.PushBack(Pair(input));
    }

    pairsSTL = pairs;

    std::chrono::time_point<std::chrono::system_clock> start_ts = std::chrono::system_clock::now();

    RadixSort(pairs);

    auto end_ts = std::chrono::system_clock::now();
    uint64_t radix_sort_ts = std::chrono::duration_cast<duration_t>( end_ts - start_ts ).count();


    start_ts = std::chrono::system_clock::now();

    std::stable_sort(&pairsSTL[0], &pairsSTL[0] + pairsSTL.Size(),
    [](const Pair& a, const Pair& b) {
        return a.numeric_key < b.numeric_key;
    });

    end_ts = std::chrono::system_clock::now();
    uint64_t stl_sort_ts = std::chrono::duration_cast<duration_t>( end_ts - start_ts ).count();
    
    std::cout << "Radix sort time: " << radix_sort_ts << DURATION_PREFIX << std::endl;
    std::cout << "STL stable sort time: " << stl_sort_ts << DURATION_PREFIX << std::endl;
}
