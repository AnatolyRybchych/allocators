#include <allocators/static_arena.hpp>
#include <allocators/arena.hpp>

#include <vector>
#include <unordered_map>

int main() {
    allocators::Arena<std::pair<int, int>> alloc;
    std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, decltype(alloc)> test(alloc);

    for(int i = 0; i < 25; i++) {
        test[i] = i * 10;
    }

    for(auto [k, v]:test) {
        std::cout<<k << ": " << v <<std::endl;
    }
    std::cout<<std::endl;
}