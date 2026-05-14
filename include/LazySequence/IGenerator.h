#pragma once
#include <optional>

template <typename T>
class IGenerator {
public:
    virtual ~IGenerator() = default;
    virtual std::optional<T> GetNext() = 0;
};