#pragma once

#include <functional>
#include <stdexcept>
#include "../Sequences/Sequence.h"

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