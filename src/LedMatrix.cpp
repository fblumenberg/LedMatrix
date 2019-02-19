#include <Arduino.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include "FS.h"

#include "LedMatrix.h"

const int PixelCount = NUM_LEDS + 1; //First LED is for levelshift

#define colorSaturation 128

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> strip(PixelCount, 3);
NeoPixelAnimator animations(PixelCount, NEO_CENTISECONDS);
NeoTopology<ColumnMajorLayout> topo(10, 10);

// our NeoBitmapFile will use the same color feature as NeoPixelBus and
// we want it to use the SD File object
NeoBitmapFile<NeoGrbFeature, File> image;

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

void LedMatrix::Show(){
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
    strip.SetPixelColor(1 + topo.Map(x, y), color);
}

uint16_t LayoutMap(int16_t x, int16_t y)
{
    return 1 + topo.MapProbe(x, 9-y);
}

void LedMatrix::ShowWifi()
{
    strip.SetPixelColor(0, green);
    // image.Blt(strip, 1, 0, 0, PixelCount);
    image.Blt(strip, 0, 0, 0, 0, 10, 10, LayoutMap);
    strip.Show();
}