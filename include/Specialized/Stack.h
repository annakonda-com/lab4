#pragma once
#include "../Sequences/Sequence.h"
#include "../Sequences/ListSequence.h"
#include "../Sequences/SequenceFunctions.h"
#include <stdexcept>

template<typename T>
class Stack {
private:
    Sequence<T> *data;

    Stack(const Stack &) = delete;
    Stack &operator=(const Stack &) = delete;

public:
    Stack() : data(new MutableListSequence<T>()) {
    }

    explicit Stack(Sequence<T> *seq) : data(seq) {
    }

    ~Stack() { delete data; }

    void Push(const T &item) { data->Append(item); }

    T Pop() {
        if (data->GetLength() == 0) throw std::underflow_error("Stack underflow");
        T val = data->GetLast();
        data->RemoveAt(data->GetLength() - 1);
        return val;
    }

    T Peek() const {
        if (data->GetLength() == 0) throw std::underflow_error("Stack empty");
        return data->GetLast();
    }

    bool IsEmpty() const { return data->GetLength() == 0; }
    int GetLength() const { return data->GetLength(); }

    template<typename Func>
    Stack<T> *Map(Func func) const {
        Sequence<T> *newSeq = SequenceFunctions::Map(data, func);
        return new Stack<T>(newSeq);
    }

    template<typename Func>
    Stack<T> *Where(Func pred) const {
        Sequence<T> *newSeq = SequenceFunctions::Where(data, pred);
        return new Stack<T>(newSeq);
    }

    template<typename Func>
    T Reduce(Func func, T init) const {
        return SequenceFunctions::Reduce(data, init, func);
    }

    Stack<T> *Concat(const Stack<T> *other) const {
        Sequence<T> *concatSeq = data->Concat(other->data);
        return new Stack<T>(concatSeq);
    }

    Stack<T> *GetSubsequence(int start, int end) const {
        Sequence<T> *sub = data->GetSubsequence(start, end);
        return new Stack<T>(sub);
    }

    bool FindSubsequence(const Stack<T> *sub, int &startIndex) const {
        return SequenceFunctions::ContainsSubsequence(data, sub->data);
    }

    const Sequence<T> *GetSequence() const { return data; }
};