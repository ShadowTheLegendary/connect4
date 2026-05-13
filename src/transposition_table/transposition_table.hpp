#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include <random>

inline uint64_t zobrist_table[42][4];

inline void register_zobrist() {
    std::mt19937_64 rng(0xDEADBEEF12345678ULL);
    for (auto& cell : zobrist_table) {
        for (auto& val : cell) {
            val = rng();
        }
    }
}

inline uint64_t hash_board(const std::array<uint8_t, 11>& board) {
    uint64_t h = 0;
    for (int token = 0; token < 42; token++) {
        int index = token / 4;
        int shift = (token % 4) * 2;
        uint8_t val = (board[index] >> shift) & 0b11;
        h ^= zobrist_table[token][val];
    }
    return h;
}

inline uint64_t update_hash(uint64_t h, int token_index, uint8_t old_val, uint8_t new_val) {
    return h ^ zobrist_table[token_index][old_val] ^ zobrist_table[token_index][new_val];
}

enum class TTFlag : uint8_t { EXACT, LOWER_BOUND, UPPER_BOUND };

struct TTEntry {
    uint64_t hash;
    int32_t  score;
    uint8_t  depth;
    TTFlag   flag;
    uint8_t  best_move;
    uint8_t  pad;
};
static_assert(sizeof(TTEntry) == 16);

class TranspositionTable {
public:
    explicit TranspositionTable(size_t size_in_MB = 64);

    TTEntry* probe(uint64_t hash);
    void store(uint64_t hash, int32_t score, uint8_t depth, TTFlag flag, uint8_t best_move = 0xFF);
    void clear();

private:
    std::vector<TTEntry> table_;
    size_t size_;
};