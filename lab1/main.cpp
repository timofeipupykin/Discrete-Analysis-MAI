#include "main.hpp"


void CountingSortStep(Vector<Pair>& input, Vector<Pair>& output, int shift) {
    const size_t BASE = 256;
    static uint64_t extra[BASE];

    for (size_t i = 0; i < input.Size(); i++) {
        uint8_t index = (input[i].numeric_key >> shift) & 0xFF;
        extra[index]++;
    }

    for (size_t i = 1; i < BASE; i++) {
        extra[i] += extra[i - 1];
    }
    
    for (int i = (int)input.Size() - 1; i >= 0; i--) {
        uint8_t index = (input[i].numeric_key >> shift) & 0xFF;
        extra[index]--;
        output[extra[index]] = std::move(input[i]);
    }

    for (size_t i = 0; i < BASE; i++) {
        extra[i] = 0;
    }
}

void RadixSort(Vector<Pair>& pairs) {
    if (pairs.Size() < 2) return;

    Vector<Pair> buffer(pairs.Size());

    Vector<Pair>* src = &pairs;
    Vector<Pair>* dest = &buffer;

    for (int byte = 0; byte < 8; byte++) {
        CountingSortStep(*src, *dest, byte * 8);

        std::swap(src, dest);
    }

    if (src != &pairs) {
        pairs = std::move(*src);
    }
}
