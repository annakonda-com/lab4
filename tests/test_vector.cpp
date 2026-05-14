#include <gtest/gtest.h>
#include "../include/Specialized/Vector.h"

TEST(VectorTest, SizeAndAccess) {
    Vector<int> v(3);
    EXPECT_EQ(v.Size(), 3);

    v.SetAt(0, 5);
    v.SetAt(1, 6);
    v.SetAt(2, 7);

    EXPECT_EQ(v.GetAt(0), 5);
    EXPECT_EQ(v.GetAt(1), 6);
    EXPECT_EQ(v.GetAt(2), 7);

    EXPECT_THROW(v.GetAt(3), std::out_of_range);
    EXPECT_THROW(v.SetAt(-1, 0), std::out_of_range);
}

TEST(VectorTest, Addition) {
    Vector<int> v1(2);
    v1.SetAt(0, 1);
    v1.SetAt(1, 2);

    Vector<int> v2(2);
    v2.SetAt(0, 3);
    v2.SetAt(1, 4);

    Vector<int> sum = v1.SumWith(v2);
    EXPECT_EQ(sum.GetAt(0), 4);
    EXPECT_EQ(sum.GetAt(1), 6);

    Vector<int> v3(3);
    EXPECT_THROW(v1.SumWith(v3), std::domain_error);
}

TEST(VectorTest, ScalarMultiplication) {
    Vector<int> v(2);
    v.SetAt(0, 1);
    v.SetAt(1, 2);

    Vector<int> scaled = v.MultiplyByScalar(5);
    EXPECT_EQ(scaled.GetAt(0), 5);
    EXPECT_EQ(scaled.GetAt(1), 10);
}

TEST(VectorTest, DotProduct) {
    Vector<int> v1(2);
    v1.SetAt(0, 1);
    v1.SetAt(1, 2);

    Vector<int> v2(2);
    v2.SetAt(0, 3);
    v2.SetAt(1, 4);

    EXPECT_EQ(v1.DotProduct(v2), 11);
}

TEST(VectorTest, Magnitude) {
    Vector<int> v(2);
    v.SetAt(0, 3);
    v.SetAt(1, 4);
    EXPECT_DOUBLE_EQ(v.GetMagnitude(), 5.0);
}

TEST(VectorTest, CopyConstructorAndAssignment) {
    Vector<int> v1(2);
    v1.SetAt(0, 10);
    v1.SetAt(1, 20);

    Vector<int> v2(v1);
    EXPECT_EQ(v2.GetAt(0), 10);
    EXPECT_EQ(v2.Size(), 2);

    Vector<int> v3;
    v3 = v1;
    EXPECT_EQ(v3.GetAt(1), 20);
    EXPECT_EQ(v3.Size(), 2);
}