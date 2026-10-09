#include <iostream>
#include <assert.h>
const size_t HEAP_SIZE = 1024 * 1024;
alignas(16) static unsigned char heap[HEAP_SIZE];
struct Block {
    // size = header & ~size_t(0xF);
    size_t header; // lower 4 bits are flags, upper 4 are size
};
Block* b = reinterpret_cast<Block*>(heap + 8);
bool initialized = false;

static void init() {
    b->header = HEAP_SIZE - 16;
    initialized = true;
}

size_t get_size(Block* b) {
    return (b->header & ~size_t(0xF));
}
bool get_alloc(Block* b) {
    return b->header & 0x1;
}

size_t alignValue16(size_t size) {
    return (size + 15) & size_t(~15);
}

void write_block(Block* b, size_t size, bool alloc) {
    b->header = size | alloc;
}

Block* next(Block* curr) {
    size_t size = get_size(curr);
    unsigned char* nextAddr = reinterpret_cast<unsigned char*>(curr) + size;
    return reinterpret_cast<Block*>(nextAddr);
}

Block* find_free(size_t bytesNeeded) {
    Block* curr = b;
    unsigned char* end = heap + HEAP_SIZE - 8;
    while (reinterpret_cast<unsigned char*>(curr) < end) {
        if (!get_alloc(curr) && get_size(curr) >= bytesNeeded) {
            return curr;
        }
        curr = next(curr);
    }
    return nullptr;
}

void split(Block* b, size_t sizeNeeded) {
    size_t remainder = get_size(b) - sizeNeeded;
    write_block(b, sizeNeeded, true);
    if (remainder >= 16) {
        unsigned char* nextPos = reinterpret_cast<unsigned char*>(b) + sizeNeeded;
        Block* remBlock = reinterpret_cast<Block*>(nextPos);
        write_block(remBlock, remainder, false);
    }
}

void* mmalloc(size_t bytes) {
    if (!initialized) {
        init();
    }
    // size max overflow?
    size_t totalBytes = alignValue16(bytes + sizeof(size_t));
    Block* block = find_free(totalBytes);
    if (block == nullptr) {
        return nullptr;
    }
    split(block, totalBytes);
    return reinterpret_cast<unsigned char*>(block) + 8;
}

void ffree(void* ptr) {
    if (ptr == nullptr) return;
    Block* b = reinterpret_cast<Block*>(static_cast<unsigned char*>(ptr) - 8);  
    write_block(b, get_size(b), false);
    Block* curr = next(b);
    unsigned char* end = heap + HEAP_SIZE - 8;
    size_t counter = get_size(b);
    while (reinterpret_cast<unsigned char*>(curr) < end and !get_alloc(curr)) {
        counter += get_size(curr);
        curr = next(curr);
    }
    write_block(b, counter, false);
}

int main() {
    void* a = mmalloc(10);
    void* b = mmalloc(100);

    ffree(a);
    void* c = mmalloc(10);
    assert(c == a);
    return 0;
}