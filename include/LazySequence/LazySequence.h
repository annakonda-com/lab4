#pragma once
#include "Sequence.h"
#include <vector>
#include <memory>

#include "IGenerator.h"

template <typename T>
class LazySequence : public Sequence<T> {
private:
    mutable std::vector<T> cache;
    std::unique_ptr<IGenerator<T>> generator;
    mutable bool is_finished = false;

    std::vector<T> deferred_appends;

    void materialize_to(int index) const {
        while (!is_finished && (int)cache.size() <= index) {
            auto next = generator->GetNext();
            if (next.has_value()) {
                cache.push_back(next.value());
            } else {
                is_finished = true;
            }
        }
    }

public:
    LazySequence(IGenerator<T>* gen) : generator(gen) {}

    T Get(int index) const override {
        if (index < 0) throw std::out_of_range("Index out of range");

        materialize_to(index);

        if (index < (int)cache.size()) {
            return cache[index];
        }

        if (is_finished) {
            int deferred_idx = index - (int)cache.size();
            if (deferred_idx >= 0 && deferred_idx < (int)deferred_appends.size()) {
                return deferred_appends[deferred_idx];
            }
        }

        throw std::out_of_range("Index out of bounds (infinite or not yet reached)");
    }

    int GetLength() const override {
        if (!is_finished) return -1;
        return (int)(cache.size() + deferred_appends.size());
    }

    void Append(T item) override {
        deferred_appends.push_back(item);
    }

};
