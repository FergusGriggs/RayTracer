#pragma once

#include <stdlib.h> 

namespace static_functions
{
    static float randomNegativeOneToOne()
    {
        return static_cast<float>(rand() % 20000) * 0.0001f - 1.0f;
    }
}
