#pragma once
#include "LazySequence/IGenerator.h"

template <typename T>
class FilterGenerator : public IGenerator<T> {
private:
    Sequence<T>* source;
    int source_idx = 0;
    bool (*predicate)(const T&);

public:
    FilterGenerator(Sequence<T>* src, bool (*pred)(const T&))
        : source(src), predicate(pred) {}

    std::optional<T> GetNext() override {
        while (true) {
            // Если исходная последовательность конечна и закончилась
            int len = source->GetLength();
            if (len != -1 && source_idx >= len) return std::nullopt;

            T item = source->Get(source_idx++);
            if (predicate(item)) return item;

            // Если последовательность была бесконечной, цикл просто уйдет в ожидание Get
        }
    }
};
