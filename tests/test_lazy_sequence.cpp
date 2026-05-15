#include <gtest/gtest.h>
#include "./LazySequence/LazySequence.h"
#include "./LazySequence/Generators.h"
#include <string>

class CounterGenerator : public Generator<int> {
    mutable int count = 0;
    int limit;
public:
    CounterGenerator(int l) : limit(l) {}
    int GetNext() override { return count++; }
    bool HasNext() const override { return count < limit; }
    Generator<int>* Clone() const override { return new CounterGenerator(limit); }
    int GetCurrentCount() const { return count; }
};

TEST(LazySequenceTest, Basics) {
    int arr[] = {1, 2, 3};
    LazySequence<int> seq(arr, 3);

    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.GetFirst(), 1);
    EXPECT_EQ(seq.GetLast(), 3);
    EXPECT_EQ(seq.Get(1), 2);
    EXPECT_THROW(seq.Get(5), std::out_of_range);
}

TEST(LazySequenceTest, IsTrulyLazy) {
    auto* counter = new CounterGenerator(10);
    LazySequence<int> seq(counter, Cardinal::Finite(10));

    EXPECT_EQ(seq.GetMaterializedCount(), 0);

    seq.Get(0);
    EXPECT_EQ(seq.GetMaterializedCount(), 1);

    seq.Get(5);
    EXPECT_EQ(seq.GetMaterializedCount(), 6);

    delete counter;
}

TEST(LazySequenceTest, InfiniteFibonacci) {
    auto fibRule = [](Sequence<int>* history) {
        int n = history->GetLength();
        if (n == 0) return 0;
        if (n == 1) return 1;
        return history->Get(n - 1) + history->Get(n - 2);
    };

    auto* init = new MutableArraySequence<int>();
    LazySequence<int> fib(fibRule, init);

    EXPECT_EQ(fib.GetLength(), -1);
    EXPECT_EQ(fib.Get(0), 0);
    EXPECT_EQ(fib.Get(1), 1);
    EXPECT_EQ(fib.Get(2), 1);
    EXPECT_EQ(fib.Get(3), 2);
    EXPECT_EQ(fib.Get(4), 3);
    EXPECT_EQ(fib.Get(5), 5);
    EXPECT_EQ(fib.Get(6), 8);

    EXPECT_THROW(fib.GetLast(), std::logic_error);
    delete init;
}

TEST(LazySequenceTest, Transformations) {
    int arr[] = {1, 2, 3, 4, 5};
    LazySequence<int> base(arr, 5);
    auto* filtered = base.Where([](int x) { return x % 2 != 0; });
    auto* mapped = filtered->Map<int>([](int x) { return x * x; });
    int sum = mapped->Reduce<int>(0, [](int acc, int x) { return acc + x; });
    EXPECT_EQ(sum, 35);
    delete filtered;
    delete mapped;
}

TEST(LazySequenceTest, StructureModifications) {
    int arr[] = {10, 20};
    LazySequence<int> s(arr, 2);

    auto* s1 = s.Append(30);
    auto* s2 = s1->Prepend(0);

    EXPECT_EQ(s2->GetLength(), 4);
    EXPECT_EQ(s2->Get(0), 0);
    EXPECT_EQ(s2->Get(3), 30);

    auto* sub = s2->GetSubsequence(1, 2);
    EXPECT_EQ(sub->GetLength(), 2);
    EXPECT_EQ(sub->Get(0), 10);

    delete s1; delete s2; delete sub;
}

TEST(LazySequenceTest, ZipUnzip) {
    int a[] = {1, 2};
    std::string b[] = {"A", "B"};

    LazySequence<int> s1(a, 2);
    MutableArraySequence<std::string> s2(b, 2);

    auto* zipped = s1.Zip(&s2);
    ASSERT_EQ(zipped->GetLength(), 2);
    EXPECT_EQ(zipped->Get(0).first, 1);
    EXPECT_EQ(zipped->Get(0).second, "A");
    EXPECT_EQ(zipped->Get(1).first, 2);
    EXPECT_EQ(zipped->Get(1).second, "B");

    auto pair = LazySequence<int>::Unzip(zipped);

    ASSERT_EQ(pair.first->GetLength(), 2);
    EXPECT_EQ(pair.first->Get(0), 1);
    EXPECT_EQ(pair.first->Get(1), 2);

    ASSERT_EQ(pair.second->GetLength(), 2);
    EXPECT_EQ(pair.second->Get(0), "A");
    EXPECT_EQ(pair.second->Get(1), "B");

    delete zipped;
    delete pair.first;
    delete pair.second;
}

TEST(GeneratorTest, WhereGeneratorRobustness) {
    int d[] = {1, 2, 3, 4, 5};
    auto* s = new MutableArraySequence<int>(d, 5);
    auto* src = new SequenceGenerator<int>(s);
    WhereGenerator<int> filter(src, [](int x) { return x % 2 == 0; });
    EXPECT_TRUE(filter.HasNext());
    EXPECT_TRUE(filter.HasNext());
    EXPECT_EQ(filter.GetNext(), 2);

    EXPECT_TRUE(filter.HasNext());
    EXPECT_EQ(filter.GetNext(), 4);

    EXPECT_FALSE(filter.HasNext());

    delete src; delete s;
}

TEST(LazySequenceTest, IteratorIteration) {
    int arr[] = {100, 200, 300};
    LazySequence<int> seq(arr, 3);
    auto* it = seq.GetEnumerator();

    EXPECT_TRUE(it->MoveNext());
    EXPECT_EQ(it->GetCurrent(), 100);
    EXPECT_TRUE(it->MoveNext());
    EXPECT_EQ(it->GetCurrent(), 200);
    EXPECT_TRUE(it->MoveNext());
    EXPECT_EQ(it->GetCurrent(), 300);
    EXPECT_FALSE(it->MoveNext());

    delete it;
}

TEST(LazySequenceTest, EdgeCases) {
    LazySequence<int> empty;
    EXPECT_EQ(empty.GetLength(), 0);
    EXPECT_THROW(empty.GetFirst(), std::out_of_range);
    int arr[] = {1};
    LazySequence<int> s(arr, 1);
    auto* res = s.Concat(&empty);
    EXPECT_EQ(res->GetLength(), 1);
    EXPECT_EQ(res->Get(0), 1);
    delete res;
}

TEST(LazySequenceTest, CloneState) {
    int arr[] = {7, 8, 9};
    LazySequence<int> original(arr, 3);
    original.Get(0);

    auto* copy = original.Clone();
    EXPECT_EQ(copy->GetLength(), 3);
    EXPECT_EQ(copy->Get(2), 9);

    delete copy;
}


TEST(SyncGeneratorTest, AccuracyCheck) {
    double tau = 10.0;
    auto* s1 = new MutableArraySequence<Pair<double, int>>();
    auto* s2 = new MutableArraySequence<Pair<double, int>>();

    s1->Append({102.0, 10});
    s2->Append({108.0, 20});

    std::vector<Sequence<Pair<double, int>>*> sources = {s1, s2};
    SyncEventGenerator<int> gen(sources, tau);

    ASSERT_TRUE(gen.HasNext());
    Event<int> e = gen.GetNext();
    EXPECT_DOUBLE_EQ(e.calculatedTime, 100.0);
    EXPECT_EQ(e.values[0].value(), 10);
    EXPECT_EQ(e.values[1].value(), 20);

    delete s1; delete s2;
}

TEST(SyncGeneratorTest, MissingDataHandle) {
    double tau = 5.0;
    auto* s1 = new MutableArraySequence<Pair<double, int>>();
    auto* s2 = new MutableArraySequence<Pair<double, int>>();

    s1->Append({10.0, 1});
    s2->Append({20.0, 2});

    std::vector<Sequence<Pair<double, int>>*> sources = {s1, s2};
    SyncEventGenerator<int> gen(sources, tau);

    Event<int> e1 = gen.GetNext();
    EXPECT_EQ(e1.values[1], std::nullopt);
}