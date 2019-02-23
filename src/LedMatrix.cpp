#include <Arduino.h>
#include <NeoPixelBus.h>
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

static Colors::HTMLColorCode palette[16] = {

    Colors::MidnightBlue,
    Colors::DarkBlue,
    Colors::MidnightBlue,
    Colors::Navy,

    Colors::DarkBlue,
    Colors::MediumBlue,
    Colors::SeaGreen,
    Colors::Teal,

    Colors::CadetBlue,
    Colors::Blue,
    Colors::DarkCyan,
    Colors::CornflowerBlue,

    Colors::Aquamarine,
    Colors::SeaGreen,
    Colors::Aqua,
    Colors::LightSkyBlue};

NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> strip(PixelCount, 3);
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
        RgbColor c = strip.GetPixelColor(i+1);

        // CRGB c1 = CRGB(c.R,c.G,c.B);
        // c1.nscale8(value);
        // c.R=c1.r;
        // c.G=c1.g;
        // c.B=c1.b;
        c.Darken(value);
        strip.SetPixelColor(i + 1, c);
    }
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
            strip.SetPixelColor(XY(MATRIX_WIDTH - 1 - x, y), strip.GetPixelColor(XY(x, y)));
            strip.SetPixelColor(XY(MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y), strip.GetPixelColor(XY(x, y)));
            strip.SetPixelColor(XY(x, MATRIX_HEIGHT - 1 - y), strip.GetPixelColor(XY(x, y)));
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
            strip.SetPixelColor(XY(MATRIX_WIDTH - 1 - x, y), strip.GetPixelColor(XY(y, x)));
            strip.SetPixelColor(XY(x, MATRIX_HEIGHT - 1 - y), strip.GetPixelColor(XY(y, x)));
            strip.SetPixelColor(XY(MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y), strip.GetPixelColor(XY(x, y)));
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
            strip.SetPixelColor(XY(x, y), strip.GetPixelColor(XY(y, x)));
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
            strip.SetPixelColor(XY(MATRIX_CENTRE_Y - y, MATRIX_CENTRE_X - x), strip.GetPixelColor(XY(x, y)));
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
            strip.SetPixelColor(XY(x, y), strip.GetPixelColor(XY(y, x)));
        }
    }

    for (int x = MATRIX_WIDTH / 4; x < MATRIX_WIDTH / 2; x++)
    {
        for (int y = MATRIX_HEIGHT / 4; y >= 0; y--)
        {
            strip.SetPixelColor(XY(x, y), strip.GetPixelColor(XY(y, x)));
        }
    }
}

void LedMatrix::Caleidoscope6()
{
    for (int x = 1; x < MATRIX_CENTER_X; x++)
    {
        strip.SetPixelColor(XY(7 - x, 7), strip.GetPixelColor(XY(x, 0)));
    } //a
    for (int x = 2; x < MATRIX_CENTER_X; x++)
    {
        strip.SetPixelColor(XY(7 - x, 6), strip.GetPixelColor(XY(x, 1)));
    } //b
    for (int x = 3; x < MATRIX_CENTER_X; x++)
    {
        strip.SetPixelColor(XY(7 - x, 5), strip.GetPixelColor(XY(x, 2)));
    } //c
    for (int x = 4; x < MATRIX_CENTER_X; x++)
    {
        strip.SetPixelColor(XY(7 - x, 4), strip.GetPixelColor(XY(x, 3)));
    } //d
    for (int x = 5; x < MATRIX_CENTER_X; x++)
    {
        strip.SetPixelColor(XY(7 - x, 3), strip.GetPixelColor(XY(x, 4)));
    } //e
    for (int x = 6; x < MATRIX_CENTER_X; x++)
    {
        strip.SetPixelColor(XY(7 - x, 2), strip.GetPixelColor(XY(x, 5)));
    } //f
    for (int x = 7; x < MATRIX_CENTER_X; x++)
    {
        strip.SetPixelColor(XY(7 - x, 1), strip.GetPixelColor(XY(x, 6)));
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
