#pragma once

#include <FastLED.h>

#include "Drawable.h"
#include "Effects.h"

class ElectricMandala : public Drawable
{
  private:
    // The coordinates for 16-bit noise spaces.
#define NUM_LAYERS 1

    // used for the random based animations
    int16_t dx;
    int16_t dy;
    int16_t dz;
    int16_t dsx;
    int16_t dsy;

  public:
    ElectricMandala()
    {
        name = (char *)"ElectricMandala";
    }

    void Start()
    {
        // set to reasonable values to avoid a black out
        noisesmoothing = 200;

        // just any free input pin
        //random16_add_entropy(analogRead(18));

        // fill coordinates with random values
        // set zoom levels
        noise_x = random16();
        noise_y = random16();
        noise_z = random16();
        noise_scale_x = 6000;
        noise_scale_y = 6000;

        // for the random movement
        dx = random8();
        dy = random8();
        dz = random8();
        dsx = random8();
        dsy = random8();
    }

    unsigned int DrawFrame()
    {
#if FASTLED_VERSION >= 3001000
        // a new parameter set every 15 seconds
        EVERY_N_SECONDS(15)
        {
            //SetupRandomPalette3();
            dy = random16(500) - 250; // random16(2000) - 1000 is pretty fast but works fine, too
            dx = random16(500) - 250;
            dz = random16(500) - 250;
            noise_scale_x = random16(10000) + 2000;
            noise_scale_y = random16(10000) + 2000;
        }
#endif

        noise_y += dy;
        noise_x += dx;
        noise_z += dz;

        FillNoise();
        ShowNoiseLayer(0, 1, 0);

        matrix.Caleidoscope3();
        matrix.Caleidoscope1();

        return 0;
    }

    // show just one layer
    void ShowNoiseLayer(byte layer, byte colorrepeat, byte colorshift)
    {
        for (uint8_t i = 0; i < MATRIX_WIDTH; i++)
        {
            for (uint8_t j = 0; j < MATRIX_HEIGHT; j++)
            {

                uint8_t color = noise[i][j];

                uint8_t bri = color;

                // assign a color depending on the actual palette
                RgbColor pixel = matrix.ColorFromCurrentPalette(colorrepeat * (color + colorshift), bri);
                matrix.SetPixelColor(i, j, pixel);
            }
        }
    }
};
