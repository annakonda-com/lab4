#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <limits>
#include "../LazySequence/LazySequence.h"

template<typename F>
double benchmark_milli(F func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

template<typename T>
void print_lazy_sequence(LazySequence<T>& seq, int count_to_show = 10) {
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

void lazySequenceMenu() {
    Sequence<long long>* current = nullptr;
    int choice;

    while (true) {
        std::cout << "\n=== ACTIVE LAZY SEQUENCE SYSTEM ===\n";
        if (current) {
            print_lazy_sequence(*static_cast<LazySequence<long long>*>(current));
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
            std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (choice == 0) {
            delete current;
            current = nullptr;
            break;
        }

        Sequence<long long>* next = nullptr;

        switch (choice) {
            case 1: {
                std::cout << "Count: "; int n; std::cin >> n;
                auto* base = new MutableArraySequence<long long>();
                for (int i = 0; i < n; ++i) {
                    long long v; std::cin >> v;
                    base->Append(v);
                }
                delete current;
                current = new LazySequence<long long>(base);
                delete base;
                break;
            }
            case 2: {
                auto fib = [](Sequence<long long>* s) -> long long {
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
                long long val; std::cout << "Value to append: "; std::cin >> val;
                next = current->Append(val);
                break;
            }
            case 4: {
                if (!current) break;
                std::cout << "Enter 2 elements for a new sequence to concat: ";
                auto* s2 = new MutableArraySequence<long long>();
                for(int i=0; i<2; ++i) { long long v; std::cin >> v; s2->Append(v); }
                auto* tempLazy = new LazySequence<long long>(s2);
                next = current->Concat(tempLazy); //
                delete s2; delete tempLazy;
                break;
            }
            case 5: {
                if (!current) break;
                int start, end; std::cout << "Start and End indices: "; std::cin >> start >> end;
                try { next = current->GetSubsequence(start, end); } //
                catch (...) { std::cout << "Error: Index out of range!\n"; }
                break;
            }
            case 6: {
                if (!current) break;
                int idx; std::cout << "Index to remove: "; std::cin >> idx;
                try { next = current->RemoveAt(idx); } //
                catch (...) { std::cout << "Error!\n"; }
                break;
            }
            case 7: {
                if (!current) break;
                int idx; std::cout << "Target index: "; std::cin >> idx;
                double t = benchmark_milli([&]() {
                    try { std::cout << "Value: " << current->Get(idx) << "\n"; }
                    catch (...) { std::cout << "Range error\n"; }
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

void run_interface() {
    int type = 0;
    while (true) {
        std::cout << "\n=== MAIN INTERFACE ===\n";
        std::cout << "1. LazySequence (Lab 4)\n";
        std::cout << "0. Exit\n>> ";

        if (!(std::cin >> type)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (type == 0) break;
        if (type == 1) {
            lazySequenceMenu();
        } else {
            std::cout << "Option not implemented yet.\n";
        }
    }
}