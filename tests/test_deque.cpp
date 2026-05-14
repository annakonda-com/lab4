#include <gtest/gtest.h>
#include "../include/Specialized/Deque.h"

TEST(DequeTest, BasicOperations) {
    Deque<int> d;
    d.PushBack(10);
    d.PushFront(5);
    d.PushBack(20);
    EXPECT_EQ(d.GetLength(), 3);
    EXPECT_EQ(d.PeekFront(), 5);
    EXPECT_EQ(d.PeekBack(), 20);
    EXPECT_EQ(d.PopFront(), 5);
    EXPECT_EQ(d.PopBack(), 20);
    EXPECT_EQ(d.PopFront(), 10);
    EXPECT_TRUE(d.IsEmpty());
}

TEST(DequeTest, Underflow) {
    Deque<int> d;
    EXPECT_THROW(d.PopFront(), std::underflow_error);
    EXPECT_THROW(d.PopBack(), std::underflow_error);
}

TEST(DequeTest, MapWhereReduce) {
    Deque<int> d;
    d.PushBack(1); d.PushBack(2); d.PushBack(3);
    Deque<int>* mapped = d.Map([](const int& x) -> int { return x * 2; });
    ASSERT_EQ(mapped->GetLength(), 3);
    EXPECT_EQ(mapped->PopFront(), 2);
    EXPECT_EQ(mapped->PopFront(), 4);
    EXPECT_EQ(mapped->PopFront(), 6);
    delete mapped;

    Deque<int>* filtered = d.Where([](const int& x) -> bool { return x % 2 == 0; });
    ASSERT_EQ(filtered->GetLength(), 1);
    EXPECT_EQ(filtered->PopFront(), 2);
    delete filtered;
}

TEST(DequeTest, SortAndMerge) {
    Deque<int> d;
    d.PushBack(3); d.PushBack(1); d.PushBack(2);
    d.Sort();
    EXPECT_EQ(d.PopFront(), 1);
    EXPECT_EQ(d.PopFront(), 2);
    EXPECT_EQ(d.PopFront(), 3);

    Deque<int> a, b;
    a.PushBack(1); a.PushBack(3); a.PushBack(5);
    b.PushBack(2); b.PushBack(4); b.PushBack(6);
    Deque<int>* merged = Deque<int>::Merge(&a, &b);
    ASSERT_EQ(merged->GetLength(), 6);
    for (int expected = 1; expected <= 6; ++expected)
        EXPECT_EQ(merged->PopFront(), expected);
    delete merged;
}

TEST(DequeTest, ConcatAndSubsequence) {
    Deque<int> d1, d2;
    d1.PushBack(1); d1.PushBack(2);
    d2.PushBack(3); d2.PushBack(4);
    Deque<int>* concat = d1.Concat(&d2);
    ASSERT_EQ(concat->GetLength(), 4);
    EXPECT_EQ(concat->PopFront(), 1);
    EXPECT_EQ(concat->PopFront(), 2);
    EXPECT_EQ(concat->PopFront(), 3);
    EXPECT_EQ(concat->PopFront(), 4);
    delete concat;

    Deque<int>* sub = d1.GetSubsequence(0, 0);
    ASSERT_EQ(sub->GetLength(), 1);
    EXPECT_EQ(sub->PopFront(), 1);
    delete sub;
}