#include <gtest/gtest.h>
#include <string>
#include "LazySequence/TuringMachine.h"

TEST(TuringMachineTest, HeavyLoadTest) {
    TuringMachine tm("q0");
    std::string largeInput(100000, '1');
    tm.SetInput(largeInput);
    tm.AddTransition("q0", '1', '1', Direction::RIGHT, "q0");
    tm.AddTransition("q0", '_', '_', Direction::LEFT, "q1");
    tm.AddTransition("q1", '1', '0', Direction::LEFT, "q1");
    tm.AddTransition("q1", '_', '1', Direction::NONE, "halt");
    auto start = std::chrono::high_resolution_clock::now();
    tm.Run();
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start;
    std::cout << "МТ обработала ленту в 100к символов за: " << diff.count() << " сек." << std::endl;
    EXPECT_EQ(tm.GetTape()[0], '1');
    EXPECT_EQ(tm.GetTape()[1], '0');
}