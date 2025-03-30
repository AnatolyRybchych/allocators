#include <allocators/static_arena.hpp>

#include <vector>
#include <unordered_map>

int main() {
    uint8_t buffer[1024] = {0};
    allocators::StaticArena<std::pair<int, int>> alloc(sizeof buffer, buffer);
    std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, decltype(alloc)> test(alloc);

    for(int i = 0; i < 25; i++) {
        test[i] = i * 10;
    }

    for(auto [k, v]:test) {
        std::cout<<k << ": " << v <<std::endl;
    }

    for(unsigned i = 0; i < sizeof buffer; i++) {
        std::cout<<std::hex<<(int)buffer[i]<<"\t";

        if(i % 8 == 0) {
            std::cout<<std::endl;
        }
    }
    std::cout<<std::endl;
}