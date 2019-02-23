#pragma once

#include "Colors.h"
#include "LedMatrix.h"

class Drawable
{
  public:
    char *name;

    // a single frame should be drawn as fast as possible, without any delay or blocking
    // return how many millisecond delay is requested before the next call to DrawFrame()
    virtual unsigned int DrawFrame()
    {
        matrix.FillScreen(RgbColor(Colors::Black));

        return 0;
    };

   virtual void Start(){};
   virtual void Stop(){};
};