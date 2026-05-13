#include "transposition_table.hpp"

TranspositionTable::TranspositionTable(size_t size_in_MB) {
    size_t number_of_entries = (size_in_MB * 1024 * 1024) / sizeof(TTEntry);
    size_ = 1;
    while (size_ * 2 <= number_of_entries) {
        size_ *= 2;
    }
    table_.assign(size_, TTEntry{});
}

TTEntry* TranspositionTable::probe(uint64_t hash) {
    TTEntry* e = &table_[hash & (size_ - 1)];
    return (e->hash == hash) ? e : nullptr; // null = miss
}

void TranspositionTable::store(uint64_t hash, int32_t score, uint8_t depth, TTFlag flag, uint8_t bestMove) {
    TTEntry* e = &table_[hash & (size_ - 1)];

    if (e->hash != hash && e->depth > depth) {
        return;
    }

    *e = TTEntry{ hash, score, depth, flag, bestMove, 0 };
}

void TranspositionTable::clear() { 
    std::fill(table_.begin(), table_.end(), TTEntry{}); 
}