#include <iostream>
#include <map>
#include <string>
#include <cstdint>

int main() {
    using MyMap = std::map<std::string, uint64_t>;
    
    // Получаем тип аллокатора для узлов дерева
    using NodeAlloc = std::allocator_traits<MyMap::allocator_type>::rebind_alloc<MyMap::value_type>;
    
    // В большинстве реализаций размер структуры узла можно вычислить через 
    // внутренние типы (специфично для компилятора, например в GCC):
    // Но универсальный хак — это замерить разницу в памяти или использовать sizeof внутренних структур.
    
    std::cout << "Размер std::pair (данные): " << sizeof(MyMap::value_type) << " байт" << std::endl;
    
    // Для GCC (libstdc++) размер узла можно узнать так:
    #ifdef __GLIBCXX__
    struct NodeGen : public std::_Rb_tree_node_base {
        MyMap::value_type val;
    };
    std::cout << "Приблизительный размер узла (GCC): " << sizeof(NodeGen) << " байт" << std::endl;
    #endif

    return 0;
}