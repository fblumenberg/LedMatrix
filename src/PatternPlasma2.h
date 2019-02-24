#pragma once

#include <FastLED.h>

#include "Drawable.h"
#include "Effects.h"

class PatternPlasma2 : public Drawable
{
  private:
    int time = 0;
    int cycles = 0;

  public:
    PatternPlasma2()
    {
        name = (char *)"Plasma2";
    }

    unsigned int DrawFrame()
    {
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            for (int y = 0; y < MATRIX_HEIGHT; y++)
            {

                uint8_t temp1 = (x << 4) + time;
                uint8_t temp2 = (y << 5) + time;
                uint8_t temp3 = ((x << 4) + (y << 4)) + (time >> 1);
                uint8_t temp4 = ((x * x) << 3 + (y * y) << 3) / (x + y + 1);

                uint16_t color = (((+sin16(temp1) + sin16(temp2) + sin16(temp3) + sin16(temp4)) >> 4) + time) % 16;

                RgbColor c = matrix.ColorFromCurrentPalette(color);
                matrix.SetPixelColor(x, y, c);
            }
        }

        time += 1;
        cycles++;

        if (cycles >= 2048)
        {
            time = 0;
            cycles = 0;
        }

        return 30;
    }
};