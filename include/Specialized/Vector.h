#pragma once

#include "../Sequences/Sequence.h"
#include "../Sequences/ArraySequence.h"
#include <cmath>
#include <stdexcept>

template<typename T>
class Vector {
private:
    Sequence<T>* elements;

public:
    Vector()
        : elements(new MutableArraySequence<T>()) {}

    explicit Vector(int size)
        : elements(new MutableArraySequence<T>()) {
        for (int i = 0; i < size; ++i) {
            elements->Append(T());
        }
    }

    explicit Vector(Sequence<T>* sequence)
        : elements(sequence) {}

    Vector(const Vector& other) {
        this->elements = new MutableArraySequence<T>();
        int size = other.Size();
        for (int i = 0; i < size; ++i) {
            this->elements->Append(other.GetAt(i));
        }
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            Sequence<T>* newElements = new MutableArraySequence<T>();
            int size = other.Size();
            for (int i = 0; i < size; ++i) {
                newElements->Append(other.GetAt(i));
            }

            delete elements;
            elements = newElements;
        }
        return *this;
    }

    ~Vector() {
        delete elements;
    }

    int Size() const {
        return elements->GetLength();
    }

    T GetAt(int index) const {
        return elements->Get(index);
    }

    void SetAt(int index, const T& value) {
        int currentSize = Size();
        if (index < 0 || index >= currentSize) {
            throw std::out_of_range("Vector index out of range");
        }

        MutableArraySequence<T>* newSequence = new MutableArraySequence<T>();
        for (int i = 0; i < currentSize; ++i) {
            if (i == index) {
                newSequence->Append(value);
            } else {
                newSequence->Append(elements->Get(i));
            }
        }

        delete elements;
        elements = newSequence;
    }

    Vector<T> SumWith(const Vector<T>& other) const {
        int size = Size();
        if (size != other.Size()) {
            throw std::domain_error("Vector sizes must match for addition");
        }

        MutableArraySequence<T>* resultSequence = new MutableArraySequence<T>();
        for (int i = 0; i < size; ++i) {
            resultSequence->Append(this->GetAt(i) + other.GetAt(i));
        }

        return Vector<T>(resultSequence);
    }

    Vector<T> MultiplyByScalar(const T& scalar) const {
        int size = Size();
        MutableArraySequence<T>* resultSequence = new MutableArraySequence<T>();

        for (int i = 0; i < size; ++i) {
            resultSequence->Append(this->GetAt(i) * scalar);
        }

        return Vector<T>(resultSequence);
    }

    T DotProduct(const Vector<T>& other) const {
        int size = Size();
        if (size != other.Size()) {
            throw std::domain_error("Vector sizes must match for dot product");
        }

        T result = T();
        for (int i = 0; i < size; ++i) {
            result += this->GetAt(i) * other.GetAt(i);
        }

        return result;
    }

    double GetMagnitude() const {
        T dot = this->DotProduct(*this);
        return std::sqrt(static_cast<double>(dot));
    }
};