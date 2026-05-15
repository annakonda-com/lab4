#pragma once
#include <string>
#include "./Base/BinarySearchTree.h"
#include "./Stream/ReadOnlyStream.h"
#include "./Sequences/ArraySequence.h"

#include <locale>


class AlphabeticalIndex {
private:
    BinarySearchTree<std::string, MutableArraySequence<int>*> tree;
    const int wordsPerPage = 50;

    std::string CleanWord(std::string word) {
        std::string result = "";
        for (unsigned char c : word) {
            if (c > 127 || std::isalnum(c)) {
                result += static_cast<char>(std::tolower(c));
            }
        }
        return result;
    }

public:
    void BuildFromStream(ReadOnlyStream<std::string>& stream) {
        stream.Open();
        
        while (!stream.IsEndOfStream()) {
            std::string rawWord = stream.Read();
            std::string word = CleanWord(rawWord);

            if (word.empty()) continue;
            int currentPage = static_cast<int>(stream.GetPosition() / wordsPerPage) + 1;
            MutableArraySequence<int>*& pages = tree.GetOrInsert(word, nullptr);
            if (pages == nullptr) {
                pages = new MutableArraySequence<int>();
            }
            bool exists = false;
            for (int i = 0; i < pages->GetLength(); ++i) {
                if (pages->Get(i) == currentPage) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                pages->Append(currentPage);
            }
        }
        stream.Close();
    }

    void Print() {
        tree.Traverse([](const std::string& word, MutableArraySequence<int>*& pages) {
            std::cout << word << ": ";
            for (int i = 0; i < pages->GetLength(); ++i) {
                std::cout << pages->Get(i) << (i == pages->GetLength() - 1 ? "" : ", ");
            }
            std::cout << std::endl;
        });
    }

    ~AlphabeticalIndex() {
        tree.Traverse([](const std::string&, MutableArraySequence<int>*& pages) {
            delete pages;
        });
    }

    bool Contains(const std::string& word) {
        return tree.Find(word) != nullptr;
    }
};