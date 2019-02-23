#pragma once

#include "Drawable.h"

class Fire : public Drawable
{
  public:
    Fire();

    virtual unsigned int DrawFrame();
    virtual void Start();
};