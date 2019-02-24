#include <Arduino.h>
#include <NeoPixelBrightnessBus.h>
#include <NeoPixelAnimator.h>
#include "FS.h"

#include <FastLED.h>

#include "LedMatrix.h"
#include "Colors.h"

LedMatrix matrix;

const int PixelCount = NUM_LEDS + 1; //First LED is for levelshift

#define colorSaturation 128

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> strip(PixelCount, 3);
NeoPixelAnimator animations(PixelCount, NEO_CENTISECONDS);
NeoTopology<ColumnMajorLayout> topo(10, 10);

// our NeoBitmapFile will use the same color feature as NeoPixelBus and
// we want it to use the SD File object
NeoBitmapFile<NeoGrbFeature, File> image;

static inline uint16_t XY(uint16_t x, uint16_t y)
{
    return 1 + topo.Map(x, y);
}

LedMatrix::LedMatrix()
{
}

void LedMatrix::Setup()
{
    // this resets all the neopixels to an off state
    strip.Begin();
    strip.Show();

    strip.SetPixelColor(0, red);
    strip.Show();

    String str = "";
    Dir dir = SPIFFS.openDir("/");
    while (dir.next())
    {
        str += dir.fileName();
        str += " / ";
        str += dir.fileSize();
        str += "\r\n";
    }
    Serial.print(str);

    // open the file
    File bitmapFile = SPIFFS.open("/wifi.bmp", "r");
    if (!bitmapFile)
    {
        Serial.println("File open fail, or not present");
        // don't do anything more:
        return;
    }

    // initialize the image with the file
    if (!image.Begin(bitmapFile))
    {
        Serial.println("File format fail, not a supported bitmap");
        // don't do anything more:
        return;
    }

    Serial.printf("Loaded bmp %d,%d - %d\n", image.Width(), image.Height(), image.PixelCount());
}

void LedMatrix::Show()
{
    strip.Show();
}

void LedMatrix::SetBrightness(uint8_t value)
{
    strip.SetBrightness(value);
}

void LedMatrix::ToggleStatus()
{
    status = !status;
    strip.SetPixelColor(0, status ? red : black);
    strip.Show();
}

void LedMatrix::StatusOff()
{
    status = false;
    strip.SetPixelColor(0, black);
    strip.Show();
}

void LedMatrix::SetPixelColor(int16_t x, int16_t y, RgbColor color)
{
    strip.SetPixelColor(XY(x, y), color);
}

void LedMatrix::DimAll(byte value)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        RgbColor c = strip.GetPixelColor(i + 1);
        nscale8x3(c.R, c.G, c.B, value);
        //        c.Darken(value);
        strip.SetPixelColor(i + 1, c);
    }
}

void LedMatrix::SwapPixelColor(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    strip.SwapPixelColor(XY(x1, y1), XY(x2, y2));
}

void LedMatrix::DimPixelColor(int16_t x, int16_t y, byte value)
{
    RgbColor c = strip.GetPixelColor(XY(x, y));
    nscale8x3(c.R, c.G, c.B, value);
    //        c.Darken(value);
    strip.SetPixelColor(XY(x, y), c);
}

void LedMatrix::CopyPixelColor(int16_t xDst, int16_t yDst, int16_t xSrc, int16_t ySrc)
{
    RgbColor c = strip.GetPixelColor(XY(xSrc, ySrc));
    strip.SetPixelColor(XY(xDst, yDst), c);
}
void LedMatrix::CopyPixelColor(int16_t xDst, int16_t yDst, int16_t xSrc, int16_t ySrc, uint8_t scale)
{
    RgbColor c = strip.GetPixelColor(XY(xSrc, ySrc));
    nscale8x3(c.R, c.G, c.B, scale);
    strip.SetPixelColor(XY(xDst, yDst), c);
}

void LedMatrix::FillScreen(RgbColor color)
{
    strip.ClearTo(color);
}

RgbColor LedMatrix::ColorFromCurrentPalette(uint8_t index, uint8_t brightness)
{

    CRGB c = ColorFromPalette(PartyColors_p, index, brightness);
    return RgbColor(c.r, c.g, c.b);
}

uint16_t LayoutMap(int16_t x, int16_t y)
{
    return 1 + topo.MapProbe(x, 9 - y);
}

void LedMatrix::ShowWifi()
{
    strip.SetPixelColor(0, green);
    // image.Blt(strip, 1, 0, 0, PixelCount);
    image.Blt(strip, 0, 0, 0, 0, 10, 10, LayoutMap);
    strip.Show();
}

// All the caleidoscope functions work directly within the screenbuffer (leds array).
// Draw whatever you like in the area x(0-15) and y (0-15) and then copy it arround.

// rotates the first 16x16 quadrant 3 times onto a 32x32 (+90 degrees rotation for each one)
void LedMatrix::Caleidoscope1()
{
    for (int x = 0; x < MATRIX_CENTER_X; x++)
    {
        for (int y = 0; y < MATRIX_CENTER_Y; y++)
        {
            CopyPixelColor(MATRIX_WIDTH - 1 - x, y, x, y);
            CopyPixelColor(MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y, x, y);
            CopyPixelColor(x, MATRIX_HEIGHT - 1 - y, x, y);
        }
    }
}

// mirror the first 16x16 quadrant 3 times onto a 32x32
void LedMatrix::Caleidoscope2()
{
    for (int x = 0; x < MATRIX_CENTER_X; x++)
    {
        for (int y = 0; y < MATRIX_CENTER_Y; y++)
        {
            CopyPixelColor(MATRIX_WIDTH - 1 - x, y, y, x);
            CopyPixelColor(x, MATRIX_HEIGHT - 1 - y, y, x);
            CopyPixelColor(MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y, x, y);
        }
    }
}

// copy one diagonal triangle into the other one within a 16x16
void LedMatrix::Caleidoscope3()
{
    for (int x = 0; x <= MATRIX_CENTRE_X; x++)
    {
        for (int y = 0; y <= x; y++)
        {
            CopyPixelColor(x, y, y, x);
        }
    }
}

// copy one diagonal triangle into the other one within a 16x16 (90 degrees rotated compared to Caleidoscope3)
void LedMatrix::Caleidoscope4()
{
    for (int x = 0; x <= MATRIX_CENTRE_X; x++)
    {
        for (int y = 0; y <= MATRIX_CENTRE_Y - x; y++)
        {
            CopyPixelColor(MATRIX_CENTRE_Y - y, MATRIX_CENTRE_X - x, x, y);
        }
    }
}

// copy one diagonal triangle into the other one within a 8x8
void LedMatrix::Caleidoscope5()
{
    for (int x = 0; x < MATRIX_WIDTH / 4; x++)
    {
        for (int y = 0; y <= x; y++)
        {
            CopyPixelColor(x, y, y, x);
        }
    }

    for (int x = MATRIX_WIDTH / 4; x < MATRIX_WIDTH / 2; x++)
    {
        for (int y = MATRIX_HEIGHT / 4; y >= 0; y--)
        {
            CopyPixelColor(x, y, y, x);
        }
    }
}

void LedMatrix::Caleidoscope6()
{
    for (int x = 1; x < MATRIX_CENTER_X; x++)
    {
        CopyPixelColor(7 - x, 7, x, 0);
    } //a
    for (int x = 2; x < MATRIX_CENTER_X; x++)
    {
        CopyPixelColor(7 - x, 6, x, 1);
    } //b
    for (int x = 3; x < MATRIX_CENTER_X; x++)
    {
        CopyPixelColor(7 - x, 5, x, 2);
    } //c
    for (int x = 4; x < MATRIX_CENTER_X; x++)
    {
        CopyPixelColor(7 - x, 4, x, 3);
    } //d
    for (int x = 5; x < MATRIX_CENTER_X; x++)
    {
        CopyPixelColor(7 - x, 3, x, 4);
    } //e
    for (int x = 6; x < MATRIX_CENTER_X; x++)
    {
        CopyPixelColor(7 - x, 2, x, 5);
    } //f
    for (int x = 7; x < MATRIX_CENTER_X; x++)
    {
        CopyPixelColor(7 - x, 1, x, 6);
    } //g
}

// create a square twister to the left or counter-clockwise
// x and y for center, r for radius
void LedMatrix::SpiralStream(int x, int y, int r, byte dimm)
{
    // for (int d = r; d >= 0; d--)
    // { // from the outside to the inside
    //     for (int i = x - d; i <= x + d; i++)
    //     {
    //         strip.SetPixelColor(XY(i, y - d)] +,strip.GetPixelColor(XY(i + 1, y - d ))); // lowest row to the right
    //         leds[XY(i, y - d)].nscale8(dimm);
    //     }
    //     for (int i = y - d; i <= y + d; i++)
    //     {
    //         strip.SetPixelColor(XY(x + d, i)] +,strip.GetPixelColor(XY(x + d, i + 1 ))); // right colum up
    //         leds[XY(x + d, i)].nscale8(dimm);
    //     }
    //     for (int i = x + d; i >= x - d; i--)
    //     {
    //         strip.SetPixelColor(XY(i, y + d)] +,strip.GetPixelColor(XY(i - 1, y + d ))); // upper row to the left
    //         leds[XY(i, y + d)].nscale8(dimm);
    //     }
    //     for (int i = y + d; i >= y - d; i--)
    //     {
    //         strip.SetPixelColor(XY(x - d, i)] +,strip.GetPixelColor(XY(x - d, i - 1 ))); // left colum down
    //         leds[XY(x - d, i)].nscale8(dimm);
    //     }
    // }
}

// expand everything within a circle
void Expand(int centerX, int centerY, int radius, byte dimm)
{
    // if (radius == 0)
    //     return;

    // int currentRadius = radius;

    // while (currentRadius > 0)
    // {
    //     int a = radius, b = 0;
    //     int radiusError = 1 - a;

    //     int nextRadius = currentRadius - 1;
    //     int nextA = nextRadius - 1, nextB = 0;
    //     int nextRadiusError = 1 - nextA;

    //     while (a >= b)
    //     {
    //         // move them out one pixel on the radius
    //         strip.SetPixelColor(XY(a + centerX, b + centerY) ,strip.GetPixelColor(XY(nextA + centerX, nextB + centerY )));
    //         strip.SetPixelColor(XY(b + centerX, a + centerY) ,strip.GetPixelColor(XY(nextB + centerX, nextA + centerY )));
    //         strip.SetPixelColor(XY(-a + centerX, b + centerY) ,strip.GetPixelColor(XY(-nextA + centerX, nextB + centerY )));
    //         strip.SetPixelColor(XY(-b + centerX, a + centerY) ,strip.GetPixelColor(XY(-nextB + centerX, nextA + centerY )));
    //         strip.SetPixelColor(XY(-a + centerX, -b + centerY) ,strip.GetPixelColor(XY(-nextA + centerX, -nextB + centerY )));
    //         strip.SetPixelColor(XY(-b + centerX, -a + centerY) ,strip.GetPixelColor(XY(-nextB + centerX, -nextA + centerY )));
    //         strip.SetPixelColor(XY(a + centerX, -b + centerY) ,strip.GetPixelColor(XY(nextA + centerX, -nextB + centerY )));
    //         strip.SetPixelColor(XY(b + centerX, -a + centerY) ,strip.GetPixelColor(XY(nextB + centerX, -nextA + centerY )));

    //         // dim them
    //         leds[XY(a + centerX, b + centerY)].nscale8(dimm);
    //         leds[XY(b + centerX, a + centerY)].nscale8(dimm);
    //         leds[XY(-a + centerX, b + centerY)].nscale8(dimm);
    //         leds[XY(-b + centerX, a + centerY)].nscale8(dimm);
    //         leds[XY(-a + centerX, -b + centerY)].nscale8(dimm);
    //         leds[XY(-b + centerX, -a + centerY)].nscale8(dimm);
    //         leds[XY(a + centerX, -b + centerY)].nscale8(dimm);
    //         leds[XY(b + centerX, -a + centerY)].nscale8(dimm);

    //         b++;
    //         if (radiusError < 0)
    //             radiusError += 2 * b + 1;
    //         else
    //         {
    //             a--;
    //             radiusError += 2 * (b - a + 1);
    //         }

    //         nextB++;
    //         if (nextRadiusError < 0)
    //             nextRadiusError += 2 * nextB + 1;
    //         else
    //         {
    //             nextA--;
    //             nextRadiusError += 2 * (nextB - nextA + 1);
    //         }
    //     }

    //     currentRadius--;
    // }
}

// give it a linear tail to the right
void LedMatrix::StreamRight(byte scale, int fromX, int toX, int fromY, int toY)
{
    for (int x = fromX + 1; x < toX; x++)
    {
        for (int y = fromY; y < toY; y++)
        {
            CopyPixelColor(x, y, x - 1, y, scale);
        }
    }
    for (int y = fromY; y < toY; y++)
        DimPixelColor(0, y, scale);
}

// give it a linear tail to the left
void LedMatrix::StreamLeft(byte scale, int fromX, int toX, int fromY, int toY)
{
    for (int x = toX; x < fromX; x++)
    {
        for (int y = fromY; y < toY; y++)
        {
            CopyPixelColor(x, y, x + 1, y, scale);
        }
    }
    for (int y = fromY; y < toY; y++)
        DimPixelColor(0, y, scale);
}

// give it a linear tail downwards
void LedMatrix::StreamDown(byte scale)
{
    for (int x = 0; x < MATRIX_WIDTH; x++)
    {
        for (int y = 1; y < MATRIX_HEIGHT; y++)
        {
            CopyPixelColor(x, y, x, y - 1, scale);
        }
    }
    for (int x = 0; x < MATRIX_WIDTH; x++)
        DimPixelColor(x, 0, scale);
}

// give it a linear tail upwards
void LedMatrix::StreamUp(byte scale)
{
    for (int x = 0; x < MATRIX_WIDTH; x++)
    {
        for (int y = MATRIX_HEIGHT - 2; y >= 0; y--)
        {
            CopyPixelColor(x, y, x, y + 1, scale);
        }
    }
    for (int x = 0; x < MATRIX_WIDTH; x++)
        DimPixelColor(x, MATRIX_HEIGHT - 1, scale);
}

// give it a linear tail up and to the left
void LedMatrix::StreamUpAndLeft(byte scale)
{
    for (int x = 0; x < MATRIX_WIDTH - 1; x++)
    {
        for (int y = MATRIX_HEIGHT - 2; y >= 0; y--)
        {
            CopyPixelColor(x, y, x + 1, y + 1, scale);
        }
    }
    for (int x = 0; x < MATRIX_WIDTH; x++)
        DimPixelColor(x, MATRIX_HEIGHT - 1, scale);
    for (int y = 0; y < MATRIX_HEIGHT; y++)
        DimPixelColor(MATRIX_WIDTH - 1, y, scale);
}

// give it a linear tail up and to the right
void LedMatrix::StreamUpAndRight(byte scale)
{
    for (int x = 0; x < MATRIX_WIDTH - 1; x++)
    {
        for (int y = MATRIX_HEIGHT - 2; y >= 0; y--)
        {
            CopyPixelColor(x + 1, y, x, y + 1, scale);
        }
    }
    // fade the bottom row
    for (int x = 0; x < MATRIX_WIDTH; x++)
        DimPixelColor(x, MATRIX_HEIGHT - 1, scale);

    // fade the right column
    for (int y = 0; y < MATRIX_HEIGHT; y++)
        DimPixelColor(MATRIX_WIDTH - 1, y, scale);
}

// just move everything one line down
void LedMatrix::MoveDown()
{
    for (int y = MATRIX_HEIGHT - 1; y > 0; y--)
    {
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            CopyPixelColor(x, y, x, y - 1);
        }
    }
}

// just move everything one line down
void LedMatrix::VerticalMoveFrom(int start, int end)
{
    for (int y = end; y > start; y--)
    {
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            CopyPixelColor(x, y, x, y - 1);
        }
    }
}

// copy the rectangle defined with 2 points x0, y0, xDst, yDst
// to the rectangle beginning at xSrc, x3
void LedMatrix::Copy(byte x0, byte y0, byte xDst, byte yDst, byte xSrc, byte ySrc)
{
    for (int y = y0; y < yDst + 1; y++)
    {
        for (int x = x0; x < xDst + 1; x++)
        {
            CopyPixelColor(x + xSrc - x0, y + ySrc - y0, x, y);
        }
    }
}

// rotate + copy triangle (MATRIX_CENTER_X*MATRIX_CENTER_X)
void LedMatrix::RotateTriangle()
{
    for (int x = 1; x < MATRIX_CENTER_X; x++)
    {
        for (int y = 0; y < x; y++)
        {
            CopyPixelColor(x, 4 - y, 4 - x, y);
        }
    }
}

// mirror + copy triangle (MATRIX_CENTER_X*MATRIX_CENTER_X)
void LedMatrix::MirrorTriangle()
{
    for (int x = 1; x < MATRIX_CENTER_X; x++)
    {
        for (int y = 0; y < x; y++)
        {
            CopyPixelColor(4 - y, x, 4 - x, y);
        }
    }
}
