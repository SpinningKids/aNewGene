
/************************************
  Code By: Pan/SpinningKids
  Revised on 3/15/2001 10:11:47 PM
  Seriously revised on 2022/02/12
  Comments: creation
 ************************************/

#include "noise.h"

#include <cstdlib>
#include <cmath>

namespace {

    int pan_random() {
        return (rand() << 16) + (rand() << 1) + (rand() & 1);
    }

    void pan_srandom(int seed) {
        srand(seed);
    }

    float catmullrom(float x, const float k0, float k1, float k2, float k3) {
        const float c3 = 1.5f * (k1 - k2) + 0.5f * (k3 - k0);
        const float c2 = k0 - 2.5f * k1 + 2.f * k2 - 0.5f * k3;
        const float c1 = 0.5f * (k2 - k0);
        const float c0 = k1;
        return ((c3 * x + c2) * x + c1) * x + c0;
    }

    bool valueTabInit(int seed) {
        pan_srandom(seed);
        for (size_t i = 0; i < details::tableSize; i++) {
            details::permutation_table[i] = i;
        }
        for (size_t i = 0; i < 16 * details::tableSize; i++) {
            const table_type i1 = pan_random() & details::tableMask;
            const table_type i2 = pan_random() & details::tableMask;
            std::swap(details::permutation_table[i1], details::permutation_table[i2]);
        }
        return true;
    }
}

namespace details {

    float valueFunc(table_type i)
    {
        return 1.f - 2.f * i / (tableSize - 1);
    }

    float vnoise2(size_t array_size, table_type index, const table_type ix[], const float fx[]) {
        if (!array_size)
        {
            return valueFunc(index);
        }
        const table_type base_index = index + ix[array_size - 1];
        return
            catmullrom(
                fx[array_size - 1],
                vnoise2(array_size - 1, permutation(base_index - 1), ix, fx),
                vnoise2(array_size - 1, permutation(base_index + 0), ix, fx),
                vnoise2(array_size - 1, permutation(base_index + 1), ix, fx),
                vnoise2(array_size - 1, permutation(base_index + 2), ix, fx));
    }

    bool init = valueTabInit(665); // risky business. This might be optimized away.
}
