#pragma once

#include "../include/Sequences/ArraySequence.h"
#include "../include/Sequences/ListSequence.h"
#include "../include/Sequences/SequenceFunctions.h"
#include "../include/Specialized/BitSequence.h"
#include "../include/Specialized/Queue.h"
#include "../include/Specialized/Stack.h"
#include "../include/Specialized/Deque.h"
#include "../include/Specialized/Vector.h"
#include "../include/Common/Option.h"
#include <iostream>
#include <string>
#include <chrono>
#include <limits>

template<typename T>
void update_ptr(T*& ptr, T* newPtr) {
    if (ptr == newPtr) return;
    delete ptr;
    ptr = newPtr;
}

template<typename F>
double benchmark(F func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

Sequence<int>* create_seq(int type, int* data, int count) {
    if (type == 1) return new MutableArraySequence<int>(data, count);
    if (type == 2) return new ImmutableArraySequence<int>(data, count);
    if (type == 3) return new MutableListSequence<int>(data, count);
    if (type == 4) return new ImmutableListSequence<int>(data, count);
    return nullptr;
}

int* input_int_sequence(const std::string& prompt, int& out_length) {
    std::cout << prompt << " (count): ";
    int n = 0;
    if (!(std::cin >> n) || n < 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        out_length = 0;
        return nullptr;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (n == 0) {
        out_length = 0;
        return new int[0];
    }
    int* buf = new int[n];
    std::cout << "Enter " << n << " elements: ";
    for (int i = 0; i < n; ++i) {
        if (!(std::cin >> buf[i])) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            delete[] buf;
            out_length = 0;
            return nullptr;
        }
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    out_length = n;
    return buf;
}

void stackMenu() {
    Stack<int> stack;
    int choice, value;
    while (true) {
        std::cout << "\n--- STACK MENU ---\n";
        std::cout << "1. Push  2. Pop  3. Peek  4. Size  5. Map (x2)  6. Where (>0)  7. Reduce (sum)  8. GetSubsequence  9. Concat with another stack  0. Back\n>> ";
        std::cin >> choice;
        if (choice == 0) break;
        switch (choice) {
            case 1: std::cout << "Value: "; std::cin >> value; stack.Push(value); break;
            case 2: try { std::cout << "Popped: " << stack.Pop() << std::endl; } catch(std::exception& e) { std::cerr << e.what() << std::endl; } break;
            case 3: try { std::cout << "Top: " << stack.Peek() << std::endl; } catch(std::exception& e) { std::cerr << e.what() << std::endl; } break;
            case 4: std::cout << "Size: " << stack.GetLength() << std::endl; break;
            case 5: {
                Stack<int>* mapped = stack.Map([](const int& x) -> int { return x * 2; });
                std::cout << "Mapped: ";
                for (int i = 0; i < mapped->GetLength(); ++i) std::cout << mapped->GetSequence()->Get(i) << " ";
                std::cout << std::endl;
                delete mapped;
                break;
            }
            case 6: {
                Stack<int>* filtered = stack.Where([](const int& x) -> bool { return x > 0; });
                std::cout << "Filtered: ";
                for (int i = 0; i < filtered->GetLength(); ++i) std::cout << filtered->GetSequence()->Get(i) << " ";
                std::cout << std::endl;
                delete filtered;
                break;
            }
            case 7: {
                int sum = stack.Reduce([](const int& a, const int& b) -> int { return a + b; }, 0);
                std::cout << "Sum: " << sum << std::endl;
                break;
            }
            case 8: {
                int start, end;
                std::cout << "Start and end indices: "; std::cin >> start >> end;
                Stack<int>* sub = stack.GetSubsequence(start, end);
                std::cout << "Substack: ";
                for (int i = 0; i < sub->GetLength(); ++i) std::cout << sub->GetSequence()->Get(i) << " ";
                std::cout << std::endl;
                delete sub;
                break;
            }
            case 9: {
                Stack<int> other;
                std::cout << "Enter elements for second stack (-1 to stop): ";
                while (std::cin >> value && value != -1) other.Push(value);
                Stack<int>* concat = stack.Concat(&other);
                std::cout << "Concatenated: ";
                for (int i = 0; i < concat->GetLength(); ++i) std::cout << concat->GetSequence()->Get(i) << " ";
                std::cout << std::endl;
                delete concat;
                break;
            }
            default: std::cout << "Invalid option\n";
        }
    }
}

void dequeMenu() {
    Deque<int> deq;
    int choice, value;
    while (true) {
        std::cout << "\n--- DEQUE MENU ---\n";
        std::cout << "1. PushFront  2. PushBack  3. PopFront  4. PopBack  5. PeekFront  6. PeekBack\n";
        std::cout << "7. Size  8. Map (x2)  9. Where (>0)  10. Reduce (sum)  11. Sort  12. Merge with another deque\n";
        std::cout << "13. GetSubsequence  14. Concat  0. Back\n>> ";
        std::cin >> choice;
        if (choice == 0) break;
        switch (choice) {
            case 1: std::cout << "Value: "; std::cin >> value; deq.PushFront(value); break;
            case 2: std::cout << "Value: "; std::cin >> value; deq.PushBack(value); break;
            case 3: try { std::cout << "Popped front: " << deq.PopFront() << std::endl; } catch(std::exception& e) { std::cerr << e.what() << std::endl; } break;
            case 4: try { std::cout << "Popped back: " << deq.PopBack() << std::endl; } catch(std::exception& e) { std::cerr << e.what() << std::endl; } break;
            case 5: try { std::cout << "Front: " << deq.PeekFront() << std::endl; } catch(std::exception& e) { std::cerr << e.what() << std::endl; } break;
            case 6: try { std::cout << "Back: " << deq.PeekBack() << std::endl; } catch(std::exception& e) { std::cerr << e.what() << std::endl; } break;
            case 7: std::cout << "Size: " << deq.GetLength() << std::endl; break;
            case 8: {
                Deque<int>* mapped = deq.Map([](const int& x) -> int { return x * 2; });
                std::cout << "Mapped: ";
                for (int i = 0; i < mapped->GetLength(); ++i) std::cout << mapped->GetSequence()->Get(i) << " ";
                std::cout << std::endl;
                delete mapped;
                break;
            }
            case 9: {
                Deque<int>* filtered = deq.Where([](const int& x) -> bool { return x > 0; });
                std::cout << "Filtered: ";
                for (int i = 0; i < filtered->GetLength(); ++i) std::cout << filtered->GetSequence()->Get(i) << " ";
                std::cout << std::endl;
                delete filtered;
                break;
            }
            case 10: {
                int sum = deq.Reduce([](const int& a, const int& b) -> int { return a + b; }, 0);
                std::cout << "Sum: " << sum << std::endl;
                break;
            }
            case 11: {
                deq.Sort();
                std::cout << "Deque sorted.\n";
                break;
            }
            case 12: {
                Deque<int> other;
                std::cout << "Enter elements for second deque (sorted, -1 to stop): ";
                while (std::cin >> value && value != -1) other.PushBack(value);
                other.Sort();
                Deque<int>* merged = Deque<int>::Merge(&deq, &other);
                std::cout << "Merged: ";
                for (int i = 0; i < merged->GetLength(); ++i) std::cout << merged->GetSequence()->Get(i) << " ";
                std::cout << std::endl;
                delete merged;
                break;
            }
            case 13: {
                int start, end;
                std::cout << "Start and end indices: "; std::cin >> start >> end;
                Deque<int>* sub = deq.GetSubsequence(start, end);
                std::cout << "Subsequence: ";
                for (int i = 0; i < sub->GetLength(); ++i) std::cout << sub->GetSequence()->Get(i) << " ";
                std::cout << std::endl;
                delete sub;
                break;
            }
            case 14: {
                Deque<int> other;
                std::cout << "Enter elements for second deque (-1 to stop): ";
                while (std::cin >> value && value != -1) other.PushBack(value);
                Deque<int>* concat = deq.Concat(&other);
                std::cout << "Concatenated: ";
                for (int i = 0; i < concat->GetLength(); ++i) std::cout << concat->GetSequence()->Get(i) << " ";
                std::cout << std::endl;
                delete concat;
                break;
            }
            default: std::cout << "Invalid option\n";
        }
    }
}
void vectorMenu() {
    Vector<int> vec;
    int choice, value, index, size;

    while (true) {
        std::cout << "\n--- VECTOR MENU ---\n";
        std::cout << "1. Set size (current: " << vec.Size() << ")\n";
        std::cout << "2. Set element  3. Get element  4. Show all elements\n";
        std::cout << "5. Sum with vector  6. Multiply by scalar  7. Dot product  8. Magnitude  0. Back\n>> ";
        std::cin >> choice;

        if (choice == 0) break;

        switch (choice) {
            case 1: {
                std::cout << "New size: ";
                std::cin >> size;
                vec = Vector<int>(size);
                break;
            }
            case 2: {
                std::cout << "Index and value: ";
                std::cin >> index >> value;
                try {
                    vec.SetAt(index, value);
                } catch(const std::exception& e) {
                    std::cerr << e.what() << std::endl;
                }
                break;
            }
            case 3: {
                std::cout << "Index: ";
                std::cin >> index;
                try {
                    std::cout << "Element: " << vec.GetAt(index) << std::endl;
                } catch(const std::exception& e) {
                    std::cerr << e.what() << std::endl;
                }
                break;
            }
            case 4: {
                std::cout << "Elements: ";
                for (int i = 0; i < vec.Size(); ++i) {
                    std::cout << vec.GetAt(i) << " ";
                }
                std::cout << std::endl;
                break;
            }
            case 5: {
                int currentSize = vec.Size();
                Vector<int> other(currentSize);
                std::cout << "Enter elements of second vector (size " << currentSize << "): ";
                for (int i = 0; i < currentSize; ++i) {
                    int x; std::cin >> x;
                    other.SetAt(i, x);
                }
                Vector<int> sum = vec.SumWith(other);
                std::cout << "Resulting sum: ";
                for (int i = 0; i < sum.Size(); ++i) {
                    std::cout << sum.GetAt(i) << " ";
                }
                std::cout << std::endl;
                break;
            }
            case 6: {
                std::cout << "Scalar: ";
                std::cin >> value;
                Vector<int> scaled = vec.MultiplyByScalar(value);
                std::cout << "Scaled vector: ";
                for (int i = 0; i < scaled.Size(); ++i) {
                    std::cout << scaled.GetAt(i) << " ";
                }
                std::cout << std::endl;
                break;
            }
            case 7: {
                int currentSize = vec.Size();
                Vector<int> other(currentSize);
                std::cout << "Enter elements of second vector (size " << currentSize << "): ";
                for (int i = 0; i < currentSize; ++i) {
                    int x; std::cin >> x;
                    other.SetAt(i, x);
                }
                try {
                    std::cout << "Dot product: " << vec.DotProduct(other) << std::endl;
                } catch(const std::exception& e) {
                    std::cerr << e.what() << std::endl;
                }
                break;
            }
            case 8: {
                std::cout << "Magnitude (Norm): " << vec.GetMagnitude() << std::endl;
                break;
            }
            default:
                std::cout << "Invalid option\n";
        }
    }
}

void run_interface() {
    Sequence<int>* seq = nullptr;
    BitSequence* bit_seq = nullptr;
    Queue<int>* queue = nullptr;
    int type = 0;

    std::cout << "Select type: "
              << "1-MutableArray, 2-ImmutableArray, 3-MutableList, 4-ImmutableList, 5-Bit, 6-Queue, 7-Stack, 8-Deque, 9-Vector\n>> ";
    if (!(std::cin >> type)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cerr << "Invalid input. Exiting.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try {
        if (type == 5) {
            bit_seq = new BitSequence();
            std::string input;
            std::cout << "Enter bit string (0/1): ";
            std::cin >> input;
            for (char c : input) bit_seq->Append(c == '1');
        } else if (type == 6) {
            queue = new Queue<int>();
            int n;
            std::cout << "Enter number of elements: ";
            std::cin >> n;
            std::cout << "Enter elements: ";
            for (int i = 0; i < n; ++i) {
                int x; std::cin >> x;
                queue->Enqueue(x);
            }
            std::cout << "Queue created (" << queue->GetSize() << " items)\n";
        } else if (type >= 7 && type <= 9) {
            if (type == 7) stackMenu();
            else if (type == 8) dequeMenu();
            else if (type == 9) vectorMenu();
            return;
        } else {
            int n = 0;
            std::cout << "Enter count: ";
            if (!(std::cin >> n) || n < 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Invalid count.\n";
                return;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            int* buffer = new int[n];
            std::cout << "Enter " << n << " elements: ";
            for (int i = 0; i < n; ++i) {
                if (!(std::cin >> buffer[i])) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cerr << "\nInvalid number detected. Skipping remaining inputs.\n";
                    delete[] buffer;
                    return;
                }
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            seq = create_seq(type, buffer, n);
            delete[] buffer;
            std::cout << "Sequence created (" << seq->GetLength() << " items)." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Init error: " << e.what() << std::endl;
        return;
    }

    if (type == 6) {
        int qchoice = -1;
        while (true) {
            std::cout << "\n--- QUEUE MENU ---\n"
                      << "1.Enqueue  2.Dequeue  3.Peek  4.Size  5.Map  6.Where  7.Reduce\n"
                      << "8.Concat   9.Subsequence  10.ContainsSubsequence  11.SplitBy\n"
                      << "12.Show result  0.Exit\n>> ";

            if (!(std::cin >> qchoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (qchoice == 0) break;

            try {
                if (qchoice == 1) {
                    int val;
                    std::cout << "Value: ";
                    std::cin >> val;
                    queue->Enqueue(val);
                } else if (qchoice == 2) {
                    std::cout << "Dequeued: " << queue->Dequeue() << std::endl;
                } else if (qchoice == 3) {
                    std::cout << "Front: " << queue->Peek() << std::endl;
                } else if (qchoice == 4) {
                    std::cout << "Size: " << queue->GetSize() << std::endl;
                } else if (qchoice == 5) {
                    auto* mapped = queue->Map([](int x) { return x * 2; });
                    delete queue;
                    queue = mapped;
                    std::cout << "Mapped (x2) completed.\n";
                } else if (qchoice == 6) {
                    auto* filtered = queue->Where([](int x) { return x > 0; });
                    delete queue;
                    queue = filtered;
                    std::cout << "Filtered (>0) completed.\n";
                } else if (qchoice == 7) {
                    int sum = queue->Reduce(0, [](int a, int b) { return a + b; });
                    std::cout << "Sum: " << sum << std::endl;
                } else if (qchoice == 8) {
                    int n2;
                    int* data2 = input_int_sequence("Enter second queue for Concat", n2);
                    if (data2) {
                        Queue<int>* other = new Queue<int>();
                        for (int i = 0; i < n2; ++i) other->Enqueue(data2[i]);
                        auto* concatenated = queue->Concat(other);
                        delete queue;
                        queue = concatenated;
                        delete other;
                        delete[] data2;
                        std::cout << "Queues concatenated.\n";
                    }
                } else if (qchoice == 9) {
                    int start, end;
                    std::cout << "Enter start and end indices: ";
                    std::cin >> start >> end;
                    auto* sub = queue->GetSubsequence(start, end);
                    std::cout << "Subsequence (size " << sub->GetSize() << "): ";
                    int subSize = sub->GetSize();
                    int* temp = new int[subSize];
                    for (int i = 0; i < subSize; ++i) {
                        temp[i] = sub->Dequeue();
                        std::cout << temp[i] << " ";
                    }
                    std::cout << std::endl;
                    delete[] temp;
                    delete sub;
                } else if (qchoice == 10) {
                    int n2;
                    int* data2 = input_int_sequence("Enter sub-queue to find", n2);
                    if (data2) {
                        Queue<int>* sub = new Queue<int>();
                        for (int i = 0; i < n2; ++i) sub->Enqueue(data2[i]);
                        bool found = queue->ContainsSubsequence(sub);
                        std::cout << (found ? "Subsequence FOUND." : "Subsequence NOT FOUND.") << std::endl;
                        delete sub;
                        delete[] data2;
                    }
                } else if (qchoice == 11) {
                    std::cout << "Splitting by parity (Even / Odd)...\n";
                    auto parts = queue->SplitBy([](const int& x) { return x % 2 == 0; });
                    std::cout << "Part 1 (Evens, size " << parts.first->GetSize() << ")\n";
                    std::cout << "Part 2 (Odds, size " << parts.second->GetSize() << ")\n";
                    delete parts.first;
                    delete parts.second;
                } else if (qchoice == 12) {
                    int size = queue->GetSize();
                    if (size == 0) {
                        std::cout << "Queue is empty.\n";
                    } else {
                        std::cout << "Queue (front -> back): ";
                        int* temp = new int[size];
                        for (int i = 0; i < size; ++i) {
                            int val = queue->Dequeue();
                            temp[i] = val;
                            std::cout << val << " ";
                        }
                        std::cout << std::endl;
                        for (int i = 0; i < size; ++i) queue->Enqueue(temp[i]);
                        delete[] temp;
                    }
                } else {
                    std::cout << "Invalid option.\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
        delete queue;
        std::cout << "Queue session ended.\n";
        return;
    }

    int choice = -1;
    while (true) {
        std::cout << "\n--- MENU ---\n"
                  << "1.Append  2.Prepend  3.InsertAt  4.GetSubsequence  5.Concat\n"
                  << "6.GetEnumerator  7.Map  8.Where  9.Reduce  10.Zip  11.Stats  12.BitOps\n"
                  << "13.Split  14.Slice  15.Unzip  16.FlatMap  17.Skip  18.TryFind  0.Exit\n>> ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        if (choice == 0) break;

        try {
            double time = 0;

            if (choice == 1) {
                int val; std::cout << "Value: "; std::cin >> val;
                time = benchmark([&] { update_ptr(seq, seq->Append(val)); });
            } else if (choice == 2) {
                int val; std::cout << "Value: "; std::cin >> val;
                time = benchmark([&] { update_ptr(seq, seq->Prepend(val)); });
            } else if (choice == 3) {
                int val, idx; std::cout << "Value Index: "; std::cin >> val >> idx;
                time = benchmark([&] { update_ptr(seq, seq->InsertAt(val, idx)); });
            } else if (choice == 4) {
                int s, e; std::cout << "Start End: "; std::cin >> s >> e;
                Sequence<int>* sub = nullptr;
                time = benchmark([&] { sub = seq->GetSubsequence(s, e); });
                std::cout << "Sub-result: ";
                for (int i = 0; i < sub->GetLength(); ++i) std::cout << sub->Get(i) << " ";
                delete sub;
            } else if (choice == 5) {
                int m; std::cout << "Second seq count: "; std::cin >> m;
                int* buf2 = new int[m];
                for (int i = 0; i < m; ++i) std::cin >> buf2[i];
                auto s2 = create_seq(type, buf2, m);
                time = benchmark([&] { update_ptr(seq, seq->Concat(s2)); });
                delete[] buf2; delete s2;
            } else if (choice == 7) {
                time = benchmark([&] { update_ptr(seq, SequenceFunctions::Map(seq, [](int x) { return x * 2; })); });
            } else if (choice == 8) {
                time = benchmark([&] { update_ptr(seq, SequenceFunctions::Where(seq, [](int x) { return x > 0; })); });
            } else if (choice == 9) {
                int res = 0;
                time = benchmark([&] { res = SequenceFunctions::Reduce(seq, 0, [](int a, int b) { return a + b; }); });
                std::cout << "Reduce Result (Sum): " << res << "\n";
            } else if (choice == 13) {
                int delim; std::cout << "Delimiter: "; std::cin >> delim;
                Sequence<Sequence<int>*>* res = nullptr;
                time = benchmark([&] { res = SequenceFunctions::Split(seq, [delim](int x) { return x == delim; }); });
                std::cout << "Split into " << res->GetLength() << " parts.\n";
                for (int i = 0; i < res->GetLength(); ++i) delete res->Get(i);
                delete res;
            } else if (choice == 14) {
                int start, count; std::cout << "Start and Count: "; std::cin >> start >> count;
                Sequence<int>* res = nullptr;
                time = benchmark([&] { res = SequenceFunctions::Slice(seq, start, count); });
                std::cout << "Slice result length: " << res->GetLength() << "\n";
                delete res;
            } else if (choice == 16) {
                time = benchmark([&] {
                    update_ptr(seq, SequenceFunctions::FlatMap(seq, [](int x) {
                        auto inner = new MutableArraySequence<int>();
                        inner->Append(x); inner->Append(x * 10);
                        return (Sequence<int>*)inner;
                    }));
                });
            } else if (choice == 17) {
                int cnt; std::cout << "Skip count: "; std::cin >> cnt;
                time = benchmark([&] { update_ptr(seq, SequenceFunctions::Skip(seq, cnt)); });
            } else if (choice == 18) {
                int val; std::cout << "Find value: "; std::cin >> val;
                Option<int> res = Option<int>::None();
                time = benchmark([&] { res = SequenceFunctions::TryFind(seq, [val](int x) { return x == val; }); });
                if (res.IsSome()) std::cout << "Found: " << res.GetValue() << "\n";
                else std::cout << "Not found.\n";
            }

            std::cout << "--------------------------------\n";
            std::cout << "Execution Time: " << time << " ms\n";
            std::cout << "Current Sequence: ";
            for (int i = 0; i < seq->GetLength(); ++i) {
                std::cout << seq->Get(i) << " ";
            }
            std::cout << "\n--------------------------------\n";

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    delete seq;
    delete bit_seq;
    std::cout << "Session ended." << std::endl;
}