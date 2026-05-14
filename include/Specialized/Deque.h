#pragma once
#include "../Sequences/Sequence.h"
#include "../Sequences/ListSequence.h"
#include "../Sequences/SequenceFunctions.h"
#include "../Sequences/Algorithms.h"
#include <stdexcept>

template<typename T>
class Deque {
private:
    Sequence<T> *data;

    Deque(const Deque &) = delete;
    Deque &operator=(const Deque &) = delete;

public:
    Deque() : data(new MutableListSequence<T>()) {
    }

    explicit Deque(Sequence<T> *seq) : data(seq) {
    }

    ~Deque() { delete data; }

    void PushFront(const T &item) { data->Prepend(item); }
    void PushBack(const T &item) { data->Append(item); }

    T PopFront() {
        if (data->GetLength() == 0) throw std::underflow_error("Deque underflow");
        T val = data->GetFirst();
        data->RemoveAt(0);
        return val;
    }

    T PopBack() {
        if (data->GetLength() == 0) throw std::underflow_error("Deque underflow");
        T val = data->GetLast();
        data->RemoveAt(data->GetLength() - 1);
        return val;
    }

    T PeekFront() const {
        if (data->GetLength() == 0) throw std::underflow_error("Deque empty");
        return data->GetFirst();
    }

    T PeekBack() const {
        if (data->GetLength() == 0) throw std::underflow_error("Deque empty");
        return data->GetLast();
    }

    bool IsEmpty() const { return data->GetLength() == 0; }
    int GetLength() const { return data->GetLength(); }

    template<typename Func>
    Deque<T> *Map(Func func) const {
        Sequence<T> *newSeq = SequenceFunctions::Map(data, func);
        return new Deque<T>(newSeq);
    }

    template<typename Func>
    Deque<T> *Where(Func pred) const {
        Sequence<T> *newSeq = SequenceFunctions::Where(data, pred);
        return new Deque<T>(newSeq);
    }

    template<typename Func>
    T Reduce(Func func, T init) const {
        return SequenceFunctions::Reduce(data, init, func);
    }

    Deque<T> *Concat(const Deque<T> *other) const {
        Sequence<T> *concatSeq = data->Concat(other->data);
        return new Deque<T>(concatSeq);
    }

    Deque<T> *GetSubsequence(int start, int end) const {
        Sequence<T> *sub = data->GetSubsequence(start, end);
        return new Deque<T>(sub);
    }

    bool FindSubsequence(const Deque<T> *sub, int &start) const {
        return SequenceFunctions::ContainsSubsequence(data, sub->data);
    }

    void Sort() {
        Sequence<T> *sorted = SortSequence(data);
        delete data;
        data = sorted;
    }

    static Deque<T> *Merge(const Deque<T> *a, const Deque<T> *b) {
        Sequence<T> *merged = MergeSequences(a->data, b->data);
        return new Deque<T>(merged);
    }

    const Sequence<T> *GetSequence() const { return data; }
};