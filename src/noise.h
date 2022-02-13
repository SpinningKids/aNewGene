
/************************************
  Code By: Pan/SpinningKids
  Revised on 3/15/2001 10:12:30 PM
  Seriously revised on 2022/02/12
  Comments: creation
 ************************************/

#ifndef NOISE_H_
#define NOISE_H_

#include <cstdint>
#include <limits>

using table_type = uint16_t;

namespace details {

    extern bool init;

    constexpr size_t tableBits = 16;
    constexpr size_t tableSize = 1 << tableBits;
    constexpr table_type tableMask = tableSize - 1;

    static_assert(tableMask + 1 == tableSize, "table_type isn't big enough for tableBits bits");

    float valueFunc(table_type i);
    inline unsigned short permutation_table[tableSize];

    constexpr table_type permutation(table_type index) {
        if constexpr (tableMask == std::numeric_limits<table_type>::max())
        {
            return permutation_table[index];
        }
        else
        {
            return permutation_table[index & tableMask];
        }
    }

    template<size_t N>
    uint16_t index(const table_type(&idx)[N]) {
        int new_index = 0;
        for (int i = 0; i < N; i++) {
            new_index = permutation(new_index + idx[i]);
        }
        return new_index & tableMask;
    }

    float vnoise2(size_t array_size, table_type index, const table_type ix[], const float fx[]);
}

template<size_t N>
float vlattice(const table_type(&idx)[N]) {
    return details::valueFunc(details::index(idx));
}

template<size_t N>
float vnoise(const float(&x)[N]) {
    table_type ix[N];
    float fx[N];
    for (size_t i = 0; i < N; i++) {
        float fl = floorf(x[i]);
        ix[i] = (table_type)fl;
        fx[i] = x[i] - fl;
    }
    return details::vnoise2(N, 0, ix, fx);
}

inline float vnoise(float x) {
    const float a[]{ x };
    return vnoise(a);
}

inline float vnoise(float x, float y) {
    const float a[]{ x, y };
    return vnoise(a);
}

inline float vnoise(float x, float y, float z) {
    const float a[]{ x, y, z };
    return vnoise(a);
}

inline float vnoise(float x, float y, float z, float w) {
    const float a[]{ x, y, z, w };
    return vnoise(a);
}

inline float vlattice(int x) {
    const table_type a[]{ (table_type)x };
    return vlattice(a);
}

inline float vlattice(int x, int y) {
    const table_type a[]{ (table_type)x, (table_type)y };
    return vlattice(a);
}

inline float vlattice(int x, int y, int z) {
    const table_type a[]{ (table_type)x, (table_type)y, (table_type)z };
    return vlattice(a);
}

inline float vlattice(int x, int y, int z, int w) {
    const table_type a[]{ (table_type)x, (table_type)y, (table_type)z, (table_type)w };
    return vlattice(a);
}

#endif // NOISE_H_
