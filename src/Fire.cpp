#include <Arduino.h>

#include "LedMatrix.h"
#include "Fire.h"

//these values are substracetd from the generated values to give a shape to the animation
const unsigned char valueMask[M_WIDTH][M_HEIGHT] = {
    // { 8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
    // {16, 0, 0, 0, 0, 0, 0, 0, 0, 16},
    {32, 0, 0, 0, 0, 0, 0, 0, 0, 32},
    {64, 0, 0, 0, 0, 0, 0, 0, 0, 64},
    {96, 32, 0, 0, 0, 0, 0, 0, 32, 96},
    {128, 64, 32, 0, 0, 0, 0, 32, 64, 128},
    {160, 96, 64, 16, 16, 32, 32, 64, 96, 160},
    {192, 128, 96, 64, 32, 32, 64, 96, 128, 192},
    {255, 160, 128, 96, 64, 64, 96, 128, 160, 255},
    {255, 192, 160, 128, 96, 96, 128, 160, 192, 255},
    {255, 255, 192, 160, 128, 128, 160, 192, 255, 255},
    {255, 255, 255, 255, 160, 160, 192, 255, 255, 255}};

//these are the hues for the fire,
//should be between 0 (red) to about 13 (yellow)
const unsigned char hueMask[M_WIDTH][M_HEIGHT] = {
    {0, 2, 5, 10, 12, 12, 11, 8, 4, 1},
    {1, 4, 7, 9, 10, 10, 9, 8, 4, 1},
    {1, 4, 7, 9, 10, 10, 9, 8, 4, 1},
    {1, 3, 5, 7, 8, 10, 9, 7, 3, 1},
    {1, 3, 5, 6, 7, 8, 7, 6, 3, 1},
    {1, 2, 4, 5, 6, 6, 5, 5, 2, 1},
    {1, 2, 4, 4, 5, 5, 4, 4, 2, 1},
    {0, 1, 2, 3, 4, 4, 3, 2, 1, 0},
    {0, 0, 1, 2, 3, 3, 2, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0}};

unsigned char fireMatrix[M_WIDTH][M_HEIGHT];
unsigned char line[M_WIDTH];
int pcnt = 0;

/**
 * Randomly generate the next line (fireMatrix row)
 */
void generateLine()
{
    for (unsigned char x = 0; x < M_HEIGHT; x++)
    {
        line[x] = random(64, 255);
    }
}

/**
 * shift all values in the fireMatrix up one row
 */
void shiftUp()
{

    for (unsigned char y = M_WIDTH - 1; y > 0; y--)
    {
        for (unsigned char x = 0; x < M_HEIGHT; x++)
        {
            fireMatrix[x][y] = fireMatrix[x][y - 1];
        }
    }

    for (unsigned char x = 0; x < M_HEIGHT; x++)
    {
        fireMatrix[x][0] = line[x];
    }
}

/**
 * draw a frame, interpolating between 2 "key frames"
 * @param pcnt percentage of interpolation
 */
void drawFrame(LedMatrix *_matrix, int pcnt)
{
    int nextv;

    //each row interpolates with the one before it
    for (unsigned char y = M_WIDTH - 1; y > 0; y--)
    {
        for (unsigned char x = 0; x < M_HEIGHT; x++)
        {

            int h = hueMask[y][x];
            int s = 255;
            nextv =
                (((100.0 - pcnt) * fireMatrix[x][y] + pcnt * fireMatrix[x][y - 1]) / 100.0) - valueMask[y][x];
            int v = (unsigned char)max(0, nextv);

            HsbColor colorHsb = HsbColor(
                (float)(h / 256.0),
                (float)(s / 256.0),
                (float)(v / 256.0));

            _matrix->SetPixelColor(x, y, colorHsb);
        }
    }

    //first row interpolates with the "next" line
    for (unsigned char x = 0; x < M_HEIGHT; x++)
    {
        int h = hueMask[0][x];
        int s = 255;
        int v = (unsigned char)(((100.0 - pcnt) * fireMatrix[x][0] + pcnt * line[x]) / 100.0);

        HsbColor colorHsb = HsbColor(
            (float)(h / 256.0),
            (float)(s / 256.0),
            (float)(v / 256.0));

        _matrix->SetPixelColor(x, 0, colorHsb);
    }
}

void SetRandomSeed()
{
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(0);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3)
    {
        seed ^= analogRead(0) << shifts;
        delay(1);
    }

    // Serial.println(seed);
    randomSeed(seed);
}

Fire::Fire(LedMatrix &fireMatrix)
{
    _matrix = &fireMatrix;
}

void Fire::Setup()
{
    SetRandomSeed();

    generateLine();

    //init all pixels to zero
    for (unsigned char y = 0; y < M_WIDTH; y++)
    {
        for (unsigned char x = 0; x < M_HEIGHT; x++)
        {
            fireMatrix[x][y] = 0;
        }
    }
}

void Fire::Loop()
{
    if (pcnt >= 100)
    {
        shiftUp();
        generateLine();
        pcnt = 0;
    }
    drawFrame(_matrix, pcnt);
    pcnt += 30;
}
