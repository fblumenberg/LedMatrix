#pragma once

class LedMatrix;

class Fire
{
    private:
    LedMatrix* _matrix;

  public:

    Fire(LedMatrix& matrix);

    void Setup();

    void Loop();
};