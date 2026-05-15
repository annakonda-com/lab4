#pragma once

#include <fstream>
#include <string>
#include "../ReadOnlyStream.h"


class FileReadOnlyStream : public ReadOnlyStream<std::string> {
private:
    std::string fileName;
    mutable std::ifstream file;
    size_t count;
    bool isOpen;

public:
    FileReadOnlyStream(const std::string& path) : fileName(path), count(0), isOpen(false) {}

    void Open() override {
        file.open(fileName);
        if (!file.is_open()) throw std::runtime_error("Не удалось открыть файл");
        isOpen = true;
        count = 0;
    }

    void Close() override {
        if (file.is_open()) file.close();
        isOpen = false;
    }

    std::string Read() override {
        if (!isOpen) throw std::runtime_error("Поток не открыт");
        std::string word;
        if (file >> word) {
            count++;
            return word;
        }
        throw EndOfStream();
    }

    bool IsEndOfStream() const override {
        if (!isOpen) return true;
        int c = file.peek();
        return c == EOF;
    }

    size_t GetPosition() const override {
        return count;
    }
};