#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>


void build_sa(const std::string& s, std::vector<int>& p) {
    int n = s.length();
    const int alphabet = 256;
    p.assign(n, 0);
    std::vector<int> c(n, 0), cnt(std::max(alphabet, n), 0);

    for (int i = 0; i < n; i++) cnt[(unsigned char)s[i]]++;
    for (int i = 1; i < alphabet; i++) cnt[i] += cnt[i - 1];
    for (int i = 0; i < n; i++) p[--cnt[(unsigned char)s[i]]] = i;

    c[p[0]] = 0;
    int classes = 1;
    for (int i = 1; i < n; i++) {
        if (s[p[i]] != s[p[i - 1]]) classes++;
        c[p[i]] = classes - 1;
    }

    std::vector<int> pn(n), cn(n);
    for (int h = 0; (1 << h) < n; ++h) {
        for (int i = 0; i < n; i++) {
            pn[i] = p[i] - (1 << h);
            if (pn[i] < 0) pn[i] += n;
        }
        
        fill(cnt.begin(), cnt.begin() + classes, 0);
        for (int i = 0; i < n; i++) cnt[c[pn[i]]]++;
        for (int i = 1; i < classes; i++) cnt[i] += cnt[i - 1];
        for (int i = n - 1; i >= 0; i--) p[--cnt[c[pn[i]]]] = pn[i];

        cn[p[0]] = 0;
        classes = 1;

        for (int i = 1; i < n; i++) {
            int mid1 = p[i] + (1 << h);
            if (mid1 >= n) mid1 -= n;
            
            int mid2 = p[i - 1] + (1 << h);
            if (mid2 >= n) mid2 -= n;

            if (c[p[i]] != c[p[i - 1]] || c[mid1] != c[mid2]) {
                ++classes;
            }
            cn[p[i]] = classes - 1;
        }
        c = cn;

        if (classes == n) break;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    auto start_time = std::chrono::high_resolution_clock::now();

    std::string text;
    if (!(std::cin >> text)) return 0;
    
    text += '$';

    std::vector<int> p;
    build_sa(text, p);

    std::string pattern;
    int pattern_idx = 1;

    std::vector<int> occurrences;
    
    while (std::cin >> pattern) {
        int m = pattern.length();

        auto comp_low = [&](int suffix_idx, const std::string& pat) {
            return text.compare(suffix_idx, m, pat) < 0;
        };

        auto comp_up = [&](const std::string& pat, int suffix_idx) {
            return text.compare(suffix_idx, m, pat) > 0;
        };

        auto it_start = lower_bound(p.begin(), p.end(), pattern, comp_low);
        auto it_end = upper_bound(it_start, p.end(), pattern, comp_up);

        if (it_start != it_end) {
            occurrences.clear();
            
            for (auto it = it_start; it != it_end; ++it) {
                occurrences.push_back(*it + 1);
            }
            
            std::sort(occurrences.begin(), occurrences.end());

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