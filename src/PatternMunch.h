#pragma once

#include <FastLED.h>
#include "Drawable.h"

class PatternMunch : public Drawable
{
  private:
    byte count = 0;
    byte dir = 1;
    byte flip = 0;
    byte generation = 0;

  public:
    PatternMunch()
    {
        name = (char *)"Munch";
    }

    unsigned int DrawFrame()
    {
        for (byte x = 0; x < MATRIX_WIDTH; x++)
        {
            for (byte y = 0; y < MATRIX_HEIGHT; y++)
            {

                RgbColor c = (x ^ y ^ flip) < count ? matrix.ColorFromCurrentPalette(((x ^ y) << 3) + generation) : RgbColor(CRGB::Black);
                matrix.SetPixelColor(x, y, c);
            }
        }

        count += dir;

        if (count <= 0 || count >= MATRIX_WIDTH)
        {
            dir = -dir;
        }

        if (count <= 0)
        {
            if (flip == 0)
                flip = 31;
            else
                flip = 0;
        }

        generation++;

        return 60;
    }
};