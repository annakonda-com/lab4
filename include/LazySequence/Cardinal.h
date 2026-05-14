#pragma once

struct Cardinal {
    size_t value;
    bool isInfinite;

    static Cardinal Finite(size_t v) { return {v, false}; }
    static Cardinal Infinity() { return {0, true}; }

    bool operator==(const Cardinal& other) const {
        return isInfinite == other.isInfinite && (isInfinite || value == other.value);
    }
};