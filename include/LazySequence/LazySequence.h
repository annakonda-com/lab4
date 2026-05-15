#pragma once

#include "../Sequences/Sequence.h"
#include "../Sequences/ArraySequence.h"
#include "Cardinal.h"
#include "Generators.h"
#include <stdexcept>

#include "Common/Pair.h"

template<typename T>
class LazySequence : public Sequence<T> {
private:
    mutable Generator<T> *generator;
    mutable Sequence<T> *memoized;
    Cardinal cardinalLength;

    void LinkGenerators(Generator<T> *gen) {
        if (!gen) return;
        if (auto *ruleGen = dynamic_cast<RuleGenerator<T> *>(gen)) {
            ruleGen->SetHistory(this->memoized);
        } else if (auto *concatGen = dynamic_cast<ConcatGenerator<T> *>(gen)) {
            LinkGenerators(concatGen->GetFirst());
            LinkGenerators(concatGen->GetSecond());
        }
    }

    void MaterializeUntil(int index) const {
        if (index < 0) return;
        while (memoized->GetLength() <= index && generator && generator->HasNext()) {
            memoized->Append(generator->GetNext());
        }
    }

public:
    LazySequence(Generator<T> *gen, Cardinal len) : cardinalLength(len) {
        memoized = new MutableArraySequence<T>();
        generator = gen ? gen->Clone() : nullptr;
        LinkGenerators(generator);
    }

    LazySequence(Sequence<T> *other) {
        memoized = other->Clone();
        generator = nullptr;
        int len = other->GetLength();
        cardinalLength = (len == -1) ? Cardinal::Infinity() : Cardinal::Finite(len);
    }

    LazySequence(T *items, int count) : cardinalLength(Cardinal::Finite(count)) {
        memoized = new MutableArraySequence<T>();
        for (int i = 0; i < count; ++i) {
            memoized->Append(items[i]);
        }
        generator = nullptr;
    }

    LazySequence(std::function<T(Sequence<T> *)> rule, Sequence<T> *initial)
        : cardinalLength(Cardinal::Infinity()) {
        memoized = initial->Clone();
        generator = new RuleGenerator<T>(rule, memoized);
        LinkGenerators(generator);
    }

    virtual ~LazySequence() {
        delete generator;
        delete memoized;
    }

    T Get(int index) const override {
        if (index < 0 || (!cardinalLength.isInfinite && index >= (int) cardinalLength.value))
            throw std::out_of_range("IndexOutOfRange");
        MaterializeUntil(index);
        return memoized->Get(index);
    }

    int GetLength() const override {
        return cardinalLength.isInfinite ? -1 : (int) cardinalLength.value;
    }

    T GetFirst() const override { return Get(0); }

    T GetLast() const override {
        if (cardinalLength.isInfinite) throw std::logic_error("Sequence is infinite");
        return Get((int) cardinalLength.value - 1);
    }

    Sequence<T> *Append(const T &item) override {
        MutableArraySequence<T> *itemSeq = new MutableArraySequence<T>();
        itemSeq->Append(item);
        LazySequence<T> *lazyItem = new LazySequence<T>(itemSeq);
        delete itemSeq;
        Sequence<T> *result = this->Concat(lazyItem);
        delete lazyItem;
        return result;
    }

    Sequence<T> *Prepend(const T &item) override {
        MutableArraySequence<T> *itemSeq = new MutableArraySequence<T>();
        itemSeq->Append(item);
        LazySequence<T> *lazyItem = new LazySequence<T>(itemSeq);
        delete itemSeq;
        Sequence<T> *result = lazyItem->Concat(this);
        delete lazyItem;
        return result;
    }

    Sequence<T> *Concat(Sequence<T> *other) override {
        class SimpleGen : public Generator<T> {
            Sequence<T> *s;
            int cur = 0;

        public:
            SimpleGen(Sequence<T> *seq) : s(seq->Clone()) {
            }

            ~SimpleGen() { delete s; }
            T GetNext() override { return s->Get(cur++); }

            bool HasNext() const override {
                int len = s->GetLength();
                return cur < len || len == -1;
            }

            Generator<T> * Clone() const override { return new SimpleGen(s); }
        };

        Cardinal newLen = (cardinalLength.isInfinite || other->GetLength() == -1)
                              ? Cardinal::Infinity()
                              : Cardinal::Finite(
                                  cardinalLength.value + (other->GetLength() == -1 ? 0 : other->GetLength()));

        Generator<T> *thisGen = (generator && generator->HasNext()) ? generator->Clone() : new SimpleGen(memoized);
        Generator<T> *otherGen = new SimpleGen(other);
        auto *combinedGen = new ConcatGenerator<T>(thisGen, otherGen);

        delete thisGen;
        delete otherGen;

        return new LazySequence<T>(combinedGen, newLen);
    }

    Sequence<T> *GetSubsequence(int start, int end) const override {
        int count = end - start + 1;
        return new LazySequence<T>(new SubGen<T>(const_cast<LazySequence *>(this), start, count),
                                   Cardinal::Finite(count));
    }

    Sequence<T> *InsertAt(const T &item, int index) override {
        auto *left = this->GetSubsequence(0, index - 1);
        MutableArraySequence<T> *midSeq = new MutableArraySequence<T>();
        midSeq->Append(item);
        auto *mid = new LazySequence<T>(midSeq);
        delete midSeq;
        auto *right = this->GetSubsequence(index, this->GetLength() - 1);
        auto *resLeftMid = left->Concat(mid);
        auto *result = resLeftMid->Concat(right);
        delete left;
        delete mid;
        delete right;
        delete resLeftMid;
        return result;
    }

    Sequence<T> *RemoveAt(int index) override {
        auto *left = this->GetSubsequence(0, index - 1);

        Sequence<T> *right;
        if (this->cardinalLength.isInfinite) {
            right = new LazySequence<T>(new SubGen<T>(const_cast<LazySequence *>(this), index + 1, -1),
                                        Cardinal::Infinity());
        } else {
            right = this->GetSubsequence(index + 1, this->GetLength() - 1);
        }

        auto *result = left->Concat(right);
        delete left;
        delete right;
        return result;
    }

    IEnumerator<T> *GetEnumerator() override {
        class LazyEnum : public IEnumerator<T> {
            const LazySequence<T> *seq;
            int currentIdx = -1;

        public:
            LazyEnum(const LazySequence<T> *s) : seq(s) {
            }

            bool MoveNext() override {
                int len = seq->GetLength();
                if (len != -1 && currentIdx + 1 >= len) return false;
                currentIdx++;
                try {
                    seq->Get(currentIdx);
                    return true;
                } catch (...) { return false; }
            }

            T GetCurrent() const override { return seq->Get(currentIdx); }
            void Reset() override { currentIdx = -1; }
        };
        return new LazyEnum(this);
    }

    Sequence<T> *Clone() const override {
        if (memoized->GetLength() > 0) {
            MaterializeUntil(memoized->GetLength() - 1);
        }
        LazySequence<T> *copy = new LazySequence<T>(nullptr, cardinalLength);
        delete copy->memoized;
        copy->memoized = memoized->Clone();
        copy->generator = generator ? generator->Clone() : nullptr;
        copy->LinkGenerators(copy->generator);
        return copy;
    }

    LazySequence() : cardinalLength(Cardinal::Finite(0)) {
        memoized = new MutableArraySequence<T>();
        generator = nullptr;
    }

    size_t GetMaterializedCount() const {
        return (size_t) memoized->GetLength();
    }

    template<typename U>
    LazySequence<U> *Map(std::function<U(T)> func) {
        Generator<T> *currentGen = (generator && generator->HasNext())
                                       ? generator->Clone()
                                       : new SequenceGenerator<T>(memoized);
        auto *mapGen = new MapGenerator<T, U>(currentGen, func);
        delete currentGen;
        return new LazySequence<U>(mapGen, this->cardinalLength);
    }

    LazySequence<T> *Where(std::function<bool(T)> pred) {
        Generator<T> *currentGen = (generator && generator->HasNext())
                                       ? generator->Clone()
                                       : new SequenceGenerator<T>(memoized);
        auto *whereGen = new WhereGenerator<T>(currentGen, pred);
        delete currentGen;
        return new LazySequence<T>(whereGen, Cardinal::Infinity());
    }

    template<typename U>
    U Reduce(U initial, std::function<U(U, T)> func) {
        U acc = initial;
        auto *it = this->GetEnumerator();
        while (it->MoveNext()) {
            acc = func(acc, it->GetCurrent());
        }
        delete it;
        return acc;
    }

    template<typename U>
    LazySequence<Pair<T, U> > *Zip(Sequence<U> *other) {
        auto *gen = new ZipGenerator<T, U>(this, other);
        int len2 = other->GetLength();
        Cardinal newLen;
        if (this->cardinalLength.isInfinite || len2 == -1) {
            newLen = Cardinal::Infinity();
        } else {
            newLen = Cardinal::Finite(std::min((int) this->cardinalLength.value, len2));
        }

        return new LazySequence<Pair<T, U> >(gen, newLen);
    }

    template<typename T1, typename T2>
    static Pair<Sequence<T1> *, Sequence<T2> *> Unzip(Sequence<Pair<T1, T2> > *seq) {
        auto *gen1 = new FirstProjectionGenerator<Pair<T1, T2>, T1>(seq);
        auto *gen2 = new SecondProjectionGenerator<Pair<T1, T2>, T2>(seq);

        Cardinal len = (seq->GetLength() == -1)
                           ? Cardinal::Infinity()
                           : Cardinal::Finite(seq->GetLength());

        return Pair<Sequence<T1> *, Sequence<T2> *>(
            new LazySequence<T1>(gen1, len),
            new LazySequence<T2>(gen2, len)
        );
    }
};
