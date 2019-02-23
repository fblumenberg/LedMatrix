#pragma once

#include <ArduinoJson.h>

#include "Drawable.h"

class Patterns
{
  private:
    int currentIndex = 0;
    Drawable *currentItem;

  public:
    Patterns();

    int CurrentIndex()
    {
        return currentIndex;
    }

    void Stop()
    {
        if (currentItem)
            currentItem->Stop();
    }

    void Start()
    {
        if (currentItem)
            currentItem->Start();
    }

    unsigned int DrawFrame()
    {
        if (currentItem)
            return currentItem->DrawFrame();

        return 0;
    }

    void MoveTo(int index);
    void ShuffleItems();

    bool SetPattern(String name);
    bool SetPattern(int index);

    void GetList(JsonArray& array);
};