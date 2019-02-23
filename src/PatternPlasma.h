#pragma once

#include <FastLED.h>

#include "Drawable.h"
#include "Effects.h"

class PatternPlasma : public Drawable {
private:
    int time = 0;
    int cycles = 0;

public:
    PatternPlasma() {
        name = (char *)"Plasma";
    }

    unsigned int DrawFrame() {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
            for (int y = 0; y < MATRIX_HEIGHT; y++) {
                int16_t v = 0;
                uint8_t wibble = sin8(time);
                v += sin16(x * wibble * 2 + time);
                v += cos16(y * (128 - wibble) * 2 + time);
                v += sin16(y * x * cos8(-time) / 2);

                RgbColor c = matrix.ColorFromCurrentPalette((v >> 8) + 127);
                matrix.SetPixelColor(x,y,c);
            }
        }

        time += 1;
        cycles++;

        if (cycles >= 2048) {
            time = 0;
            cycles = 0;
        }

        return 30;
    }
};