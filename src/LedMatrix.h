#pragma once

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

const int MATRIX_WIDTH = 10;
const int MATRIX_HEIGHT = 10;

const int MATRIX_CENTER_X = MATRIX_WIDTH / 2;
const int MATRIX_CENTER_Y = MATRIX_HEIGHT / 2;

const uint16_t EXTERNAL_POWER_MIN = 500;

const byte MATRIX_CENTRE_X = MATRIX_CENTER_X - 1;
const byte MATRIX_CENTRE_Y = MATRIX_CENTER_Y - 1;

const int NUM_LEDS = MATRIX_WIDTH * MATRIX_HEIGHT;

extern RgbColor red;
extern RgbColor green;
extern RgbColor blue;
extern RgbColor white;
extern RgbColor black;

class LedMatrix
{
  private:
    bool status;

  public:
    LedMatrix();

    void ToggleStatus();
    void StatusOff();
    void ShowWifi();

    void Setup();

    void Show();
    void SetBrightness(uint8_t value);

    void SetPixelColor(int16_t x, int16_t y, RgbColor color);
    void SwapPixelColor(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void DimPixelColor(int16_t x, int16_t y, byte value);
    void CopyPixelColor(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void CopyPixelColor(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t scale);

    void FillScreen(RgbColor color);
    void DimAll(byte value);

    // All the caleidoscope functions work directly within the screenbuffer (leds array).
    // Draw whatever you like in the area x(0-15) and y (0-15) and then copy it arround.

    // rotates the first 16x16 quadrant 3 times onto a 32x32 (+90 degrees rotation for each one)
    void Caleidoscope1();

    // mirror the first 16x16 quadrant 3 times onto a 32x32
    void Caleidoscope2();
    // copy one diagonal triangle into the other one within a 16x16
    void Caleidoscope3();

    // copy one diagonal triangle into the other one within a 16x16 (90 degrees rotated compared to Caleidoscope3)
    void Caleidoscope4();

    // copy one diagonal triangle into the other one within a 8x8
    void Caleidoscope5();

    void Caleidoscope6();

    // create a square twister to the left or counter-clockwise
    // x and y for center, r for radius
    void SpiralStream(int x, int y, int r, byte dimm);

    // expand everything within a circle
    void Expand(int centerX, int centerY, int radius, byte dimm);

    void StreamRight(byte scale, int fromX = 0, int toX = MATRIX_WIDTH, int fromY = 0, int toY = MATRIX_HEIGHT);
    void StreamLeft(byte scale, int fromX = MATRIX_WIDTH, int toX = 0, int fromY = 0, int toY = MATRIX_HEIGHT);
    void StreamDown(byte scale);
    void StreamUp(byte scale);
    void StreamUpAndLeft(byte scale);
    void StreamUpAndRight(byte scale);
    void MoveDown();
    void VerticalMoveFrom(int start, int end);
    void Copy(byte x0, byte y0, byte x1, byte y1, byte x2, byte y2);
    void RotateTriangle();
    void MirrorTriangle();

    RgbColor ColorFromCurrentPalette(uint8_t index, uint8_t brightness = 255);
};

// there is only one led matrix, so make it global
extern LedMatrix matrix;