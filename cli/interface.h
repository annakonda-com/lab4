#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <limits>
#include <vector>
#include <optional>
#include <thread>
#include "../LazySequence/LazySequence.h"
#include "../Stream/Specialized/IndexBuilder.h"
#include "../LazySequence/TuringMachine.h"
#include "../LazySequence/Generators.h"


void printDivider() {
    std::cout << "================================================================\n";
}

void printHeader(const std::string &title) {
    printDivider();
    std::cout << ">>> " << title << " <<<\n";
    printDivider();
}

template<typename T>
void print_lazy_sequence(LazySequence<T> &seq, int count_to_show = 10) {
    int len = seq.GetLength();
    std::cout << "Sequence info: ";
    if (len == -1) std::cout << "INFINITE";
    else std::cout << "Length = " << len;

    std::cout << "\nElements: ";
    int limit = (len == -1 || len > count_to_show) ? count_to_show : len;
    for (int i = 0; i < limit; ++i) {
        std::cout << seq.Get(i) << " ";
    }
    if (len == -1 || len > count_to_show) std::cout << "...";
    std::cout << std::endl;
}

void showLoading(const std::string &message, int duration_ms = 500) {
    std::cout << message << " [";
    for (int i = 0; i < 10; ++i) {
        std::cout << "#" << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms / 10));
    }
    std::cout << "] Done!\n";
}

template<typename F>
double benchmark_milli(F func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// --- 1. Твоё оригинальное lazySequenceMenu ---

void lazySequenceMenu() {
    Sequence<long long> *current = nullptr;
    int choice;

    while (true) {
        std::cout << "\n=== ACTIVE LAZY SEQUENCE SYSTEM ===\n";
        if (current) {
            print_lazy_sequence(*static_cast<LazySequence<long long> *>(current));
        } else {
            std::cout << "[ No sequence created yet ]\n";
        }

        std::cout << "1. Create: Manual Input (Finite)\n";
        std::cout << "2. Create: Fibonacci Rule (Infinite)\n";
        std::cout << "3. Action: Append element\n";
        std::cout << "4. Action: Concat with another sequence\n";
        std::cout << "5. Action: Get Subsequence\n";
        std::cout << "6. Action: Remove element at index\n";
        std::cout << "7. Performance: Access large index (Stress)\n";
        std::cout << "0. Back / Reset\n>> ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (choice == 0) {
            delete current;
            current = nullptr;
            break;
        }

        Sequence<long long> *next = nullptr;

        switch (choice) {
            case 1: {
                std::cout << "Count: ";
                int n;
                std::cin >> n;
                auto *base = new MutableArraySequence<long long>();
                for (int i = 0; i < n; ++i) {
                    long long v;
                    std::cin >> v;
                    base->Append(v);
                }
                delete current;
                current = new LazySequence<long long>(base);
                delete base;
                break;
            }
            case 2: {
                auto fib = [](Sequence<long long> *s) -> long long {
                    int n = s->GetLength();
                    if (n == 0) return 0;
                    if (n == 1) return 1;
                    return s->Get(n - 1) + s->Get(n - 2);
                };
                delete current;
                current = new LazySequence<long long>(new RuleGenerator<long long>(fib, nullptr), Cardinal::Infinity());
                break;
            }
            case 3: {
                if (!current) break;
                long long val;
                std::cout << "Value to append: ";
                std::cin >> val;
                next = current->Append(val);
                break;
            }
            case 4: {
                if (!current) break;
                std::cout << "Enter 2 elements for a new sequence to concat: ";
                auto *s2 = new MutableArraySequence<long long>();
                for (int i = 0; i < 2; ++i) {
                    long long v;
                    std::cin >> v;
                    s2->Append(v);
                }
                auto *tempLazy = new LazySequence<long long>(s2);
                next = current->Concat(tempLazy); //
                delete s2;
                delete tempLazy;
                break;
            }
            case 5: {
                if (!current) break;
                int start, end;
                std::cout << "Start and End indices: ";
                std::cin >> start >> end;
                try { next = current->GetSubsequence(start, end); } //
                catch (...) { std::cout << "Error: Index out of range!\n"; }
                break;
            }
            case 6: {
                if (!current) break;
                int idx;
                std::cout << "Index to remove: ";
                std::cin >> idx;
                try { next = current->RemoveAt(idx); } //
                catch (...) { std::cout << "Error!\n"; }
                break;
            }
            case 7: {
                if (!current) break;
                int idx;
                std::cout << "Target index: ";
                std::cin >> idx;
                double t = benchmark_milli([&]() {
                    try { std::cout << "Value: " << current->Get(idx) << "\n"; } catch (...) {
                        std::cout << "Range error\n";
                    }
                });
                std::cout << "Time: " << t << " ms\n";
                break;
            }
        }

        if (next) {
            delete current;
            current = next;
        }
    }
}

// --- 2. Интерактивный Алфавитный Указатель ---

class StringStream : public ReadOnlyStream<std::string> {
    std::vector<std::string> words;
    size_t pos = 0;

public:
    StringStream(std::string text) {
        std::string word;
        for (char c: text) {
            if (isspace(c)) {
                if (!word.empty()) words.push_back(word);
                word = "";
            } else word += c;
        }
        if (!word.empty()) words.push_back(word);
    }

    void Open() override { pos = 0; }

    void Close() override {
    }

    std::string Read() override { return words[pos++]; }
    bool IsEndOfStream() const override { return pos >= words.size(); }
    size_t GetPosition() const override { return pos; }
};

void indexMenu() {
    printHeader("ALPHABETICAL INDEX BUILDER");
    std::cout << "Enter text to index (end with empty line):\n";
    std::string text, line;
    std::cin.ignore();
    while (std::getline(std::cin, line) && !line.empty()) text += line + " ";

    StringStream ss(text);
    AlphabeticalIndex index;

    showLoading("Parsing stream and balancing BST");
    index.BuildFromStream(ss);

    std::cout << "\nGenerated Index (Word -> Pages):\n";
    printDivider();
    index.Print();
    printDivider();
}

// --- 3. Визуализация Машины Тьюринга ---

void turingMenu() {
    printHeader("TURING MACHINE SIMULATOR");
    std::string startState;
    std::cout << "Enter initial state (e.g. q0): ";
    std::cin >> startState;
    TuringMachine tm(startState, '_');

    std::cout << "Enter initial tape string: ";
    std::string input;
    std::cin >> input;
    tm.SetInput(input);

    std::cout << "Configuration: Enter rules (state read write dir nextState)\n";
    std::cout << "Directions: L (Left), R (Right), N (None). Type 'run' to start.\n";

    std::string s;
    while (std::cin >> s && s != "run") {
        char r, w, d_char;
        std::string nextS;
        std::cin >> r >> w >> d_char >> nextS;
        Direction d = (d_char == 'L') ? Direction::LEFT : (d_char == 'R') ? Direction::RIGHT : Direction::NONE;
        tm.AddTransition(s, r, w, d, nextS);
    }

    showLoading("Initializing Tape");
    std::cout << "Execution steps:\n";
    int steps = 0;
    while (tm.Step() && steps < 50) {
        // Здесь можно добавить вывод текущего состояния ленты, если в TM есть метод GetTape
        steps++;
    }
    std::cout << "Machine halted after " << steps << " steps.\n";
}

// --- 4. Синхронизация по времени (Events) ---

void syncMenu() {
    printHeader("EVENT SYNCHRONIZATION UNIT");
    double tau;
    std::cout << "Enter tolerance interval (tau): ";
    std::cin >> tau;

    // Создаем последовательности (Sequences)
    auto *s1 = new MutableArraySequence<Pair<double, int> >();
    auto *s2 = new MutableArraySequence<Pair<double, int> >();

    std::cout << "Simulating Source 1 and Source 2...\n";
    s1->Append({1.0, 101});
    s1->Append({2.0, 102});
    s1->Append({5.0, 105});
    s2->Append({1.2, 201});
    s2->Append({2.1, 202});
    s2->Append({4.5, 204});

    // Создаем вектор указателей на Sequence, как того требует конструктор
    // Используем базовый тип Sequence, так как конструктор ожидает именно его
    std::vector<Sequence<Pair<double, int> > *> sources = {s1, s2};

    // Теперь типы совпадают
    SyncEventGenerator<int> sync(sources, tau);

    std::cout << "\nSynchronized Result Table:\n";
    std::cout << "Time Window\t| Src 1\t| Src 2\t\n";
    printDivider();

    while (sync.HasNext()) {
        auto ev = sync.GetNext();
        std::cout << ev.calculatedTime << "\t| ";
        for (auto &v: ev.values) {
            if (v.has_value()) std::cout << v.value() << "\t| ";
            else std::cout << "---\t| ";
        }
        std::cout << "\n";
    }

    // Очистка памяти
    delete s1;
    delete s2;
}

// --- Main Interface Entry Point ---

void run_interface() {
    int type = 0;
    while (true) {
        printHeader("LAB 4: POLYMORPHIC DATA TYPES");
        std::cout << "1. [Core] LazySequence Explorer (Your Menu)\n";
        std::cout << "2. [Task] Alphabetical Index (Streams & BST)\n";
        std::cout << "3. [Task] Turing Machine (Mutable Tape)\n";
        std::cout << "4. [Task] Event Synchronization (Generators)\n";
        std::cout << "0. Exit Program\n";
        printDivider();
        std::cout << ">> ";

        if (!(std::cin >> type)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (type == 0) break;

        switch (type) {
            case 1: lazySequenceMenu();
                break;
            case 2: indexMenu();
                break;
            case 3: turingMenu();
                break;
            case 4: syncMenu();
                break;
            default: std::cout << "Option not recognized.\n";
                break;
        }

        std::cout << "\nPress Enter to return to main menu...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }
}
