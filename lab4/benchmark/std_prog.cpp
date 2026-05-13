#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    // 1. Читаем паттерн (первая строка)
    std::string pattern;
    if (!std::getline(std::cin, pattern) || pattern.empty()) return 0;
    for (char &c : pattern) c = std::tolower(c);

    // 2. Читаем весь оставшийся текст в одну большую строку
    // Для find() это самый быстрый способ
    std::string text;
    std::string line;
    
    // Замеряем время только на подготовку и сам поиск
    auto start = std::chrono::high_resolution_clock::now();

    // Считываем и сразу приводим к нижнему регистру
    while (std::getline(std::cin, line)) {
        for (char &c : line) c = std::tolower(c);
        text += line + " "; // Добавляем пробел вместо перевода строки
    }

    size_t count = 0;
    size_t pos = text.find(pattern, 0);

    while (pos != std::string::npos) {
        count++;
        // Ищем следующее вхождение
        pos = text.find(pattern, pos + 1);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cerr << "std::string::find() Time: " << elapsed.count() << " ms\n";
    std::cerr << "Found occurrences: " << count << "\n";

    return 0;
}