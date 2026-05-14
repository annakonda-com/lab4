#pragma once
#include "Sequences/Sequence.h"
#include "Sequences/ListSequence.h"
#include "Sequences/SequenceFunctions.h"
#include "Common/Pair.h"
#include <stdexcept>

template<typename T>
class Queue {
private:
    Sequence<T> *data;

public:
    Queue() {
        data = new MutableListSequence<T>();
    }

    Queue(Sequence<T>* seq) {
        if (seq == nullptr) {
            throw std::invalid_argument("Sequence pointer cannot be null");
        }
        data = seq;
    }

    Queue(const Queue<T>&) = delete;
    Queue<T>& operator=(const Queue<T>&) = delete;

    ~Queue() {
        delete data;
    }


    bool IsEmpty() const {
        return data->GetLength() == 0;
    }

    int GetSize() const {
        return data->GetLength();
    }

    void Enqueue(const T &item) {
        data->Append(item);
    }

    T Dequeue() {
        if (IsEmpty()) {
            throw std::out_of_range("Queue underflow");
        }
        T item = data->GetFirst();
        data->RemoveAt(0);
        return item;
    }

    T Peek() const {
        if (IsEmpty()) {
            throw std::out_of_range("Queue is empty");
        }
        return data->GetFirst();
    }

    IEnumerator<T> *GetEnumerator() {
        return data->GetEnumerator();
    }

    template<typename Func>
    auto Map(Func func) -> Queue<decltype(func(std::declval<T>()))>* {
        auto* mappedSeq = SequenceFunctions::Map(this->data, func);
        return new Queue<decltype(func(std::declval<T>()))>(mappedSeq);
    }

    template<typename Func>
    Queue<T>* Where(Func pred) {
        auto* filteredSeq = SequenceFunctions::Where(this->data, pred);
        return new Queue<T>(filteredSeq);
    }

    template<typename U, typename Func>
    U Reduce(U initial, Func func) {
        return SequenceFunctions::Reduce(this->data, initial, func);
    }

    Queue<T>* Concat(Queue<T>* other) {
        Sequence<T>* clonedSeq = this->data->Clone();
        Sequence<T>* resultSeq = clonedSeq->Concat(other->data);
        return new Queue<T>(resultSeq);
    }

    Queue<T>* GetSubsequence(int start, int end) {
        Sequence<T>* sub = this->data->GetSubsequence(start, end);
        return new Queue<T>(sub);
    }

    bool ContainsSubsequence(Queue<T>* sub) {
        return SequenceFunctions::ContainsSubsequence(this->data, sub->data);
    }

    Pair<Queue<T>*, Queue<T>*> SplitBy(std::function<bool(const T&)> predicate) {
        auto seqPair = SequenceFunctions::SplitBy(this->data, predicate);
        return Pair<Queue<T>*, Queue<T>*>(new Queue<T>(seqPair.first), new Queue<T>(seqPair.second));
    }
};