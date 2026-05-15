#pragma once

#include "./Sequences/Sequence.h"

template <class T>
class SequenceReadOnlyStream : public ReadOnlyStream<T> {
private:
    Sequence<T>* data;
    size_t position;
    bool isOpen;

public:
    SequenceReadOnlyStream(Sequence<T>* seq) : data(seq), position(0), isOpen(false) {}

    void Open() override { isOpen = true; }
    void Close() override { isOpen = false; }

    bool IsEndOfStream() const override {
        return position >= data->GetLength();
    }

    T Read() override {
        if (!isOpen) throw std::runtime_error("Поток не открыт");
        if (IsEndOfStream()) throw EndOfStream();
        return data->Get(position++);
    }

    size_t GetPosition() const override { return position; }

    bool IsCanSeek() const override { return true; }
    size_t Seek(size_t index) override {
        position = (index < data->GetLength()) ? index : data->GetLength();
        return position;
    }

    bool IsCanGoBack() const override { return true; }
};