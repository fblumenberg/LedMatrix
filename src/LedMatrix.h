#pragma once

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

#define M_WIDTH 10  // matrix width
#define M_HEIGHT 10 // matrix height
#define NUM_LEDS 100

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

    void SetPixelColor(int16_t x, int16_t y, RgbColor color);
};