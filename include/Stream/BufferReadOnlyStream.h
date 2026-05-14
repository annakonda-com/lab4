#pragma once
#include "RWStreams.h"

template <typename T>
class BufferedReadOnlyStream : public ReadOnlyStream<T> {
private:
    std::vector<T> buffer{};
    size_t buffer_ptr = 0;
    size_t capacity{};

    void FillBuffer() {
        buffer.clear();
        // Используем this->, так как source и cursor — члены зависимого базового класса (шаблона)
        for (size_t i = 0; i < capacity && this->HasMore(); ++i) {
            buffer.push_back(this->source->Get(this->cursor++));
        }
        buffer_ptr = 0;
    }

public:
    BufferedReadOnlyStream(Sequence<T>* seq, size_t cap)
        : ReadOnlyStream<T>(seq), capacity(cap) {}

    // Теперь деструктор вызовется корректно
    ~BufferedReadOnlyStream() override = default;

    T Read() override {
        if (buffer_ptr >= buffer.size()) {
            FillBuffer();
        }
        if (buffer.empty()) throw std::out_of_range("Stream empty");
        return buffer[buffer_ptr++];
    }
};