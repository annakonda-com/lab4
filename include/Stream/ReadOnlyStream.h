#pragma once

#include <stdexcept>


class EndOfStream : public std::runtime_error {
public:
    EndOfStream() : std::runtime_error("Достигнут конец потока") {}
};

template <class T>
class ReadOnlyStream {
public:
    virtual ~ReadOnlyStream() = default;

    virtual void Open() = 0;
    virtual void Close() = 0;
    virtual T Read() = 0;
    virtual bool IsEndOfStream() const = 0;
    virtual size_t GetPosition() const = 0;

    // Навигация (может иметь реализацию по умолчанию)
    virtual bool IsCanSeek() const { return false; }
    virtual size_t Seek(size_t index) { throw std::runtime_error("Seek not supported"); }
    virtual bool IsCanGoBack() const { return false; }

};