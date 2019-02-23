#pragma once

#include "Drawable.h"
#include "Effects.h"

class Test : public Drawable
{
  private:
    int y = 0;

  public:
    Test()
    {
        name = (char *)"Test";
    }

    unsigned int DrawFrame()
    {
        matrix.DimAll(20);
        RgbColor c = red;
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            matrix.SetPixelColor(x, y, c);
            c.Darken(10);
        }

        y = (y + 1) % MATRIX_HEIGHT;

        return 1000;
    }
};