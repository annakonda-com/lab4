#pragma once

#include <gtest/gtest.h>
#include "./Stream/WriteOnlyStream.h"
#include "./Stream/ReadOnlyStream.h"
#include "./Sequences/ArraySequence.h"
#include "Stream/Specialized/IndexBuilder.h"
#include <chrono>

#include "../include/Stream/Specialized/FileReadOnlyStream.h"
#include "Stream/Specialized/SequenceReadOnlyStream.h"

TEST(WriteOnlyStreamFunc, BasicWriteProcess) {
    MutableArraySequence<int> seq;
    WriteOnlyStream<int> stream(&seq);

    EXPECT_THROW(stream.Write(10), std::runtime_error);

    stream.Open();

    size_t pos = stream.Write(100);
    EXPECT_EQ(pos, 1);
    EXPECT_EQ(stream.GetPosition(), 1);

    stream.Write(200);
    EXPECT_EQ(seq.Get(0), 100);
    EXPECT_EQ(seq.GetLength(), 2);

    stream.Close();
    EXPECT_THROW(stream.Write(300), std::runtime_error);
}

TEST(SequenceReadOnlyStreamFunc, ReadAndSeek) {
    MutableArraySequence<int> seq;
    seq.Append(10);
    seq.Append(20);

    SequenceReadOnlyStream<int> stream(&seq);

    EXPECT_THROW(stream.Read(), std::runtime_error);

    stream.Open();

    EXPECT_EQ(stream.Read(), 10);
    EXPECT_FALSE(stream.IsEndOfStream());

    EXPECT_EQ(stream.Read(), 20);
    EXPECT_TRUE(stream.IsEndOfStream());

    EXPECT_THROW(stream.Read(), EndOfStream);

    stream.Seek(0);
    EXPECT_EQ(stream.GetPosition(), 0);
    EXPECT_EQ(stream.Read(), 10);
}

TEST(SequenceReadOnlyStreamFunc, CapabilitiesCheck) {
    MutableArraySequence<int> seq;
    SequenceReadOnlyStream<int> stream(&seq);

    EXPECT_TRUE(stream.IsCanSeek());
    EXPECT_TRUE(stream.IsCanGoBack());
}

TEST(AlphabeticalIndexTest, BasicIndexing) {
    auto* seq = new MutableArraySequence<std::string>();
    for(int i = 0; i < 50; ++i) seq->Append("apple");
    seq->Append("Banana!");

    SequenceReadOnlyStream<std::string> stream(seq);
    AlphabeticalIndex index;
    index.BuildFromStream(stream);

    testing::internal::CaptureStdout();
    index.Print();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("apple: 1"), std::string::npos);
    EXPECT_NE(output.find("banana: 2"), std::string::npos);

    delete seq;
}

TEST(AlphabeticalIndexTest, NormalizationTest) {
    auto* seq = new MutableArraySequence<std::string>();
    seq->Append("Hello,");
    seq->Append("hello...");
    seq->Append("HELLO");

    SequenceReadOnlyStream<std::string> stream(seq);
    AlphabeticalIndex index;
    index.BuildFromStream(stream);

    testing::internal::CaptureStdout();
    index.Print();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("hello: 1"), std::string::npos);

    delete seq;
}


TEST(AlphabeticalIndexTest, RealBookPerformanceTest) {
    std::string filePath = "C:\\Anna\\informatik\\lab4\\tests\\data\\warandpeace.txt";

    std::ifstream checkFile(filePath);
    if (!checkFile.is_open()) {
        GTEST_SKIP() << "Файл с книгой не найден. Пропуск теста.";
    }
    checkFile.close();

    FileReadOnlyStream fileStream(filePath);
    AlphabeticalIndex index;

    auto start = std::chrono::high_resolution_clock::now();

    index.BuildFromStream(fileStream);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << "Книга обработана за: " << diff.count() << " секунд." << std::endl;
    std::cout << "Всего слов в потоке: " << fileStream.GetPosition() << std::endl;


    // Вместо поиска в огромной строке вывода, спрашиваем у дерева напрямую
    // Важно: если в файле "Пьер", в дереве он уже "пьер" (после CleanWord)
    EXPECT_TRUE(index.Contains("Пьер")) << "Слово 'пьер' не найдено в индексе!";
    EXPECT_TRUE(index.Contains("Андрей")) << "Слово 'андрей' не найдено в индексе!";
}