#pragma once

#include "Drawable.h"
#include "Effects.h"

class PendulumWave : public Drawable
{
  public:
    PendulumWave()
    {
        name = (char *)"Pendulum Wave";
    }

    unsigned int DrawFrame()
    {
        matrix.DimAll(3);

        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            uint8_t y = beatsin8(x + MATRIX_WIDTH, 0, MATRIX_HEIGHT);

            matrix.SetPixelColor(x, y, matrix.ColorFromCurrentPalette(x * 7));
        }

        return 15;
    }
};