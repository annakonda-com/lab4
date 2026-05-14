#pragma once

template <typename T>
class ReadOnlyStream {
protected:
    Sequence<T>* source;
    int cursor = 0;
public:
    ReadOnlyStream(Sequence<T>* seq) : source(seq) {}
    
    // КРИТИЧЕСКИ ВАЖНО для полиморфизма
    virtual ~ReadOnlyStream() = default;

    virtual T Read() {
        return source->Get(cursor++);
    }

    bool HasMore() const {
        int len = source->GetLength();
        return (len == -1) || (cursor < len); 
    }
};
template <typename T>
class WriteOnlyStream {
protected:
    Sequence<T>* target;
public:
    WriteOnlyStream(Sequence<T>* seq) : target(seq) {}

    virtual ~WriteOnlyStream() = default;

    virtual void Write(T item) {
        target->Append(item);
    }
};