#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono> // Подключаем библиотеку для замера времени

int main() {
    // Оптимизация ввода-вывода
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    // Стартуем таймер в самом начале выполнения программы
    auto start_time = std::chrono::high_resolution_clock::now();

    std::string text;
    if (!(std::cin >> text)) return 0;

    std::string pattern;
    int pattern_idx = 1;
    std::vector<int> occurrences;

    // Читаем паттерны до конца файла
    while (std::cin >> pattern) {
        occurrences.clear();

        // Ищем первое вхождение паттерна в тексте
        size_t pos = text.find(pattern, 0);
        
        // Пока паттерн находится, сохраняем позицию и ищем дальше
        while (pos != std::string::npos) {
            // В ТЗ нумерация позиций с 1, а find возвращает с 0, поэтому +1
            occurrences.push_back(pos + 1); 
            
            // Ищем следующее вхождение, сместившись на 1 символ вперед
            pos = text.find(pattern, pos + 1);
        }

        // Если что-то нашли, выводим в нужном формате
        if (!occurrences.empty()) {
            std::cout << pattern_idx << ": ";
            for (size_t i = 0; i < occurrences.size(); ++i) {
                std::cout << occurrences[i] << (i + 1 == occurrences.size() ? "" : ", ");
            }
            std::cout << "\n";
        }
        
        pattern_idx++;
    }

    // Фиксируем время окончания работы алгоритма
    auto end_time = std::chrono::high_resolution_clock::now();
    
    // Считаем разницу в миллисекундах
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    // Выводим время в поток ошибок (std::cerr), чтобы не портить выходной файл
    std::cerr << "\n=========================================\n";
    std::cerr << "Время выполнения программы: " << duration << " ms\n";
    std::cerr << "=========================================\n";

    return 0;
}