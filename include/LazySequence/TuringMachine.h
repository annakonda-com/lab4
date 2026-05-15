#pragma once

#include <map>
#include <string>
#include "LazySequence.h"

enum class Direction { LEFT, RIGHT, NONE };

struct Transition {
    char writeSymbol;
    Direction moveDir;
    std::string nextState;
};

using TransitionTable = std::map<std::string, std::map<char, Transition>>;

class TuringMachine {
private:
    TransitionTable transitions;
    std::string currentState;
    int headPosition;
    MutableArraySequence<char>* tape;
    char blankSymbol;

public:
    TuringMachine(const std::string& initialState, char blank = '_')
        : currentState(initialState), headPosition(0), blankSymbol(blank) {
        tape = new MutableArraySequence<char>();
    }

    ~TuringMachine() { delete tape; }

    void AddTransition(std::string state, char read, char write, Direction dir, std::string next) {
        transitions[state][read] = {write, dir, next};
    }

    void SetInput(const std::string& input) {
        for (char c : input) tape->Append(c);
    }

    bool Step() {
        if (headPosition < 0) {
            tape->Prepend(blankSymbol);
            headPosition = 0;
        }
        if (headPosition >= tape->GetLength()) {
            tape->Append(blankSymbol);
        }

        char currentSymbol = tape->Get(headPosition);

        if (transitions.count(currentState) && transitions[currentState].count(currentSymbol)) {
            Transition t = transitions[currentState][currentSymbol];
            (*tape)[headPosition] = t.writeSymbol;
            if (t.moveDir == Direction::LEFT) headPosition--;
            else if (t.moveDir == Direction::RIGHT) headPosition++;
            currentState = t.nextState;
            return true;
        }

        return false;
    }

    void Run(int maxSteps = 1000000) {
        int steps = 0;
        while (Step() && steps < maxSteps) {
            steps++;
        }
    }

    std::string GetTape() {
        std::string res = "";
        for (int i = 0; i < tape->GetLength(); ++i) res += tape->Get(i);
        return res;
    }
};