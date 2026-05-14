#include <gtest/gtest.h>
#include "./LazySequence/LazySequence.h"

TEST(LazySequenceTest, FromSequence) {
    auto* base = new MutableArraySequence<int>();
    for(int i = 0; i < 5; ++i) base->Append(i);

    LazySequence<int> lazy(base);
    EXPECT_EQ(lazy.GetLength(), 5);
    EXPECT_EQ(lazy.Get(0), 0);
    EXPECT_EQ(lazy.Get(4), 4);
}

TEST(LazySequenceTest, MemoizationCheck) {
    int callCount = 0;
    auto rule = [&](Sequence<int>* s) {
        callCount++;
        return (int)s->GetLength();
    };

    LazySequence<int> lazy(new RuleGenerator<int>(rule, nullptr), Cardinal::Infinity());

    int firstCall = lazy.Get(0);
    int secondCall = lazy.Get(0);

    EXPECT_EQ(firstCall, 0);
    EXPECT_EQ(secondCall, 0);
    EXPECT_EQ(callCount, 1);
}

TEST(LazySequenceTest, InfiniteSequence) {
    auto rule = [](Sequence<int>* s) { return 1; };
    LazySequence<int> lazy(new RuleGenerator<int>(rule, nullptr), Cardinal::Infinity());

    EXPECT_EQ(lazy.GetLength(), -1);
    EXPECT_ANY_THROW(lazy.GetLast());
}

TEST(LazySequenceTest, ConcatTest) {
    auto* s1 = new MutableArraySequence<int>(); s1->Append(1);
    auto* s2 = new MutableArraySequence<int>(); s2->Append(2);

    LazySequence<int> lazy1(s1);
    LazySequence<int> lazy2(s2);

    auto* combined = lazy1.Concat(&lazy2);
    EXPECT_EQ(combined->GetLength(), 2);
    EXPECT_EQ(combined->Get(0), 1);
    EXPECT_EQ(combined->Get(1), 2);
}