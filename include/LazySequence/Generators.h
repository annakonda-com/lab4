#pragma once

#include <functional>
#include <stdexcept>
#include "../Sequences/Sequence.h"
#include "Common/Pair.h"

template <typename T>
class Generator {
public:
    virtual ~Generator() = default;
    virtual T GetNext() = 0;
    virtual bool HasNext() const = 0;
    virtual Generator<T>* Clone() const = 0;
};

template <typename T>
class SequenceGenerator : public Generator<T> {
    Sequence<T>* source;
    int current = 0;
public:
    SequenceGenerator(Sequence<T>* s) : source(s->Clone()) {}
    ~SequenceGenerator() override { delete source; }
    T GetNext() override { return source->Get(current++); }
    bool HasNext() const override { return current < source->GetLength(); }
    Generator<T>* Clone() const override { return new SequenceGenerator<T>(source); }
};

template <typename T>
class ConcatGenerator : public Generator<T> {
    Generator<T> *g1, *g2;
public:
    ConcatGenerator(Generator<T>* first, Generator<T>* second) {
        g1 = first ? first->Clone() : nullptr;
        g2 = second ? second->Clone() : nullptr;
    }
    ~ConcatGenerator() override {
        delete g1;
        delete g2;
    }

    T GetNext() override {
        if (g1 && g1->HasNext()) return g1->GetNext();
        if (g2 && g2->HasNext()) return g2->GetNext();
        throw std::out_of_range("No more elements");
    }
    bool HasNext() const override {
        return (g1 && g1->HasNext()) || (g2 && g2->HasNext());
    }
    Generator<T>* Clone() const override { return new ConcatGenerator<T>(g1, g2); }

    Generator<T>* GetFirst() const { return g1; }
    Generator<T>* GetSecond() const { return g2; }
};

template <typename T>
class RuleGenerator : public Generator<T> {
    std::function<T(Sequence<T>*)> rule;
    Sequence<T>* history;
public:
    RuleGenerator(std::function<T(Sequence<T>*)> r, Sequence<T>* h) : rule(r), history(h) {}

    void SetHistory(Sequence<T>* h) { history = h; }

    T GetNext() override {
        if (!history) throw std::runtime_error("History not linked");
        return rule(history);
    }
    bool HasNext() const override { return true; }

    Generator<T>* Clone() const override { return new RuleGenerator<T>(rule, history); }
};

template <typename T>
class SubGen : public Generator<T> {
    Sequence<T>* src; int start; int count; int current = 0;
public:
    SubGen(Sequence<T>* s, int st, int c) : src(s->Clone()), start(st), count(c) {}
    ~SubGen() override { delete src; }
    T GetNext() override { return src->Get(start + current++); }
    bool HasNext() const override { return current < count; }
    Generator<T>* Clone() const override { return new SubGen<T>(src, start, count); }
};

template <typename T, typename U>
class MapGenerator : public Generator<U> {
    Generator<T>* src;
    std::function<U(T)> func;
public:
    MapGenerator(Generator<T>* s, std::function<U(T)> f) : src(s->Clone()), func(f) {}
    ~MapGenerator() { delete src; }
    U GetNext() override { return func(src->GetNext()); }
    bool HasNext() const override { return src->HasNext(); }
    Generator<U>* Clone() const override { return new MapGenerator<T, U>(src, func); }
};

template <typename T>
class WhereGenerator : public Generator<T> {
    Generator<T>* src;
    std::function<bool(T)> pred;
    mutable T nextVal;
    mutable bool hasNextPrepared = false;
public:
    WhereGenerator(Generator<T>* s, std::function<bool(T)> p)
        : src(s->Clone()), pred(p) {}

    ~WhereGenerator() override { delete src; }

    T GetNext() override {
        // Если HasNext() еще не вызывался, вызываем его для подготовки значения
        if (!hasNextPrepared && !HasNext()) throw std::out_of_range("No more elements");
        hasNextPrepared = false;
        return nextVal;
    }

    bool HasNext() const override {
        if (hasNextPrepared) return true;

        while (src->HasNext()) {
            T val = src->GetNext();
            if (pred(val)) {
                nextVal = val;
                hasNextPrepared = true;
                return true;
            }
        }
        return false;
    }

    Generator<T>* Clone() const override {
        return new WhereGenerator<T>(src, pred);
    }
};

template <typename T, typename U>
class ZipGenerator : public Generator<Pair<T, U>> {
    mutable IEnumerator<T>* it1;
    mutable IEnumerator<U>* it2;
    Sequence<T>* s1_orig;
    Sequence<U>* s2_orig;
    mutable bool isPrepared = false;
    mutable bool hasNextVal = false;
public:
    ZipGenerator(Sequence<T>* s1, Sequence<U>* s2)
        : s1_orig(s1->Clone()), s2_orig(s2->Clone()) {
        it1 = s1_orig->GetEnumerator();
        it2 = s2_orig->GetEnumerator();
    }
    ~ZipGenerator() override {
        delete it1; delete it2;
        delete s1_orig; delete s2_orig;
    }
    Pair<T, U> GetNext() override {
        if (!isPrepared && !HasNext()) throw std::out_of_range("No more elements");
        isPrepared = false;
        return Pair<T, U>(it1->GetCurrent(), it2->GetCurrent());
    }
    bool HasNext() const override {
        if (isPrepared) return hasNextVal;
        hasNextVal = it1->MoveNext() && it2->MoveNext();
        isPrepared = true;
        return hasNextVal;
    }
    Generator<Pair<T, U>>* Clone() const override {
        return new ZipGenerator<T, U>(s1_orig, s2_orig);
    }
};

template <typename TPair, typename TOut>
class FirstProjectionGenerator : public Generator<TOut> {
    mutable IEnumerator<TPair>* it;
    Sequence<TPair>* src_orig;
    mutable bool isPrepared = false;
    mutable bool hasNextVal = false;
public:
    FirstProjectionGenerator(Sequence<TPair>* src) : src_orig(src->Clone()) {
        it = src_orig->GetEnumerator();
    }
    ~FirstProjectionGenerator() override { delete it; delete src_orig; }

    TOut GetNext() override {
        if (!isPrepared && !HasNext()) throw std::out_of_range("No more");
        isPrepared = false;
        return it->GetCurrent().first;
    }
    bool HasNext() const override {
        if (isPrepared) return hasNextVal;
        hasNextVal = it->MoveNext();
        isPrepared = true;
        return hasNextVal;
    }
    Generator<TOut>* Clone() const override { return new FirstProjectionGenerator<TPair, TOut>(src_orig); }
};

template <typename TPair, typename TOut>
class SecondProjectionGenerator : public Generator<TOut> {
    mutable IEnumerator<TPair>* it;
    Sequence<TPair>* src_orig;
    mutable bool isPrepared = false;
    mutable bool hasNextVal = false;
public:
    SecondProjectionGenerator(Sequence<TPair>* src) : src_orig(src->Clone()) {
        it = src_orig->GetEnumerator();
    }
    ~SecondProjectionGenerator() override { delete it; delete src_orig; }

    TOut GetNext() override {
        if (!isPrepared && !HasNext()) throw std::out_of_range("No more");
        isPrepared = false;
        return it->GetCurrent().second;
    }
    bool HasNext() const override {
        if (isPrepared) return hasNextVal;
        hasNextVal = it->MoveNext();
        isPrepared = true;
        return hasNextVal;
    }
    Generator<TOut>* Clone() const override { return new SecondProjectionGenerator<TPair, TOut>(src_orig); }
};