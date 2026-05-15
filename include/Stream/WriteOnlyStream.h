#pragma once

#include <cstddef>
#include <stdexcept>
#include "../Sequences/Sequence.h"

template <class T>
class WriteOnlyStream {
private:
    Sequence<T>* target;
    // Переименуем поле в m_position или аналогичное,
    // чтобы избежать конфликтов с именами методов.
    std::size_t m_currentPos;
    bool isOpen;

public:
    // Различные варианты создать поток для записи [cite: 199]
    WriteOnlyStream(Sequence<T>* seq)
        : target(seq), m_currentPos(0), isOpen(false) {}

    // Операции управления [cite: 203]
    void Open() {
        // После создания могут потребоваться дополнительные действия [cite: 208]
        isOpen = true;
    }

    void Close() {
        // Финализирующие действия, например освобождение дескрипторов [cite: 212, 218]
        isOpen = false;
    }

    // Записывает элемент в конец потока [cite: 206]
    size_t Write(T item) {
        if (!isOpen) throw std::runtime_error("Поток не открыт");

        target->Append(item);
        m_currentPos++;
        // Возвращает позицию, в которую будет осуществляться следующая запись [cite: 206]
        return m_currentPos;
    }

    // Декомпозиция [cite: 200]
    // Возвращает текущую позицию (сколько элементов было записано) [cite: 201, 202]
    size_t GetPosition() const {
        return m_currentPos;
    }
};