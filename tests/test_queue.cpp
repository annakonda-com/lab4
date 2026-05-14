#include <gtest/gtest.h>
#include "./Specialized/Queue.h"
#include <string>

// Тест конструктора и базового состояния
TEST(QueueTest, ConstructorAndEmptyState) {
    Queue<int> q;
    EXPECT_TRUE(q.IsEmpty());
    EXPECT_EQ(q.GetSize(), 0);
}

// Тест добавления и извлечения элементов
TEST(QueueTest, EnqueueDequeue) {
    Queue<int> q;
    q.Enqueue(10);
    q.Enqueue(20);
    q.Enqueue(30);

    EXPECT_EQ(q.GetSize(), 3);
    EXPECT_FALSE(q.IsEmpty());

    EXPECT_EQ(q.Peek(), 10);    // Первый зашел — первый на выход
    EXPECT_EQ(q.Dequeue(), 10);
    EXPECT_EQ(q.Dequeue(), 20);
    EXPECT_EQ(q.GetSize(), 1);
    EXPECT_EQ(q.Dequeue(), 30);
    EXPECT_TRUE(q.IsEmpty());
}

// Тест обработки исключений
TEST(QueueTest, Exceptions) {
    Queue<int> q;
    EXPECT_THROW(q.Dequeue(), std::out_of_range);
    EXPECT_THROW(q.Peek(), std::out_of_range);
}

// Тест метода Map
TEST(QueueTest, MapFunction) {
    Queue<int> q;
    q.Enqueue(1);
    q.Enqueue(2);
    q.Enqueue(3);

    // Умножаем каждый элемент на 2
    auto* mappedQueue = q.Map([](int x) { return x * 2; });

    EXPECT_EQ(mappedQueue->GetSize(), 3);
    EXPECT_EQ(mappedQueue->Dequeue(), 2);
    EXPECT_EQ(mappedQueue->Dequeue(), 4);
    EXPECT_EQ(mappedQueue->Dequeue(), 6);

    delete mappedQueue;
}

// Тест метода Where (фильтрация)
TEST(QueueTest, WhereFunction) {
    Queue<int> q;
    q.Enqueue(1);
    q.Enqueue(10);
    q.Enqueue(3);
    q.Enqueue(20);

    // Оставляем только числа больше 5
    auto* filteredQueue = q.Where([](int x) { return x > 5; });

    EXPECT_EQ(filteredQueue->GetSize(), 2);
    EXPECT_EQ(filteredQueue->Dequeue(), 10);
    EXPECT_EQ(filteredQueue->Dequeue(), 20);

    delete filteredQueue;
}

// Тест метода Reduce (агрегация)
TEST(QueueTest, ReduceFunction) {
    Queue<int> q;
    q.Enqueue(1);
    q.Enqueue(2);
    q.Enqueue(3);
    q.Enqueue(4);

    // Суммируем элементы: 0 + 1 + 2 + 3 + 4 = 10
    int sum = q.Reduce(0, [](int acc, int x) { return acc + x; });

    EXPECT_EQ(sum, 10);
}

// Тест метода Concat
TEST(QueueTest, ConcatQueues) {
    Queue<int> q1;
    q1.Enqueue(1);
    Queue<int> q2;
    q2.Enqueue(2);

    auto* result = q1.Concat(&q2);

    EXPECT_EQ(result->GetSize(), 2);
    EXPECT_EQ(result->Dequeue(), 1);
    EXPECT_EQ(result->Dequeue(), 2);

    delete result;
}

TEST(QueueTest, SubsequenceOperations) {
    Queue<int> q;
    for(int i = 0; i < 5; ++i) q.Enqueue(i);

    auto* sub = q.GetSubsequence(1, 3);
    EXPECT_EQ(sub->GetSize(), 3);
    EXPECT_EQ(sub->Dequeue(), 1);

    EXPECT_TRUE(q.ContainsSubsequence(sub));

    delete sub;
}