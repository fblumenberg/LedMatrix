#pragma once

typedef uint16_t accum88;

uint8_t beatcos8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timebase = 0, uint8_t phase_offset = 0)
{
  uint8_t beat = beat8(beats_per_minute, timebase);
  uint8_t beatcos = cos8(beat + phase_offset);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatcos, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

uint8_t beattriwave8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timebase = 0, uint8_t phase_offset = 0)
{
  uint8_t beat = beat8(beats_per_minute, timebase);
  uint8_t beatcos = triwave8(beat + phase_offset);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatcos, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

uint8_t mapsin8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255) {
  uint8_t beatsin = sin8(theta);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatsin, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

uint8_t mapcos8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255) {
  uint8_t beatcos = cos8(theta);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatcos, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

uint32_t noise_x;
uint32_t noise_y;
uint32_t noise_z;
uint32_t noise_scale_x;
uint32_t noise_scale_y;

uint8_t noise[MATRIX_WIDTH][MATRIX_HEIGHT];

uint8_t noisesmoothing;

  void NoiseVariablesSetup() {
    noisesmoothing = 200;

    noise_x = random16();
    noise_y = random16();
    noise_z = random16();
    noise_scale_x = 6000;
    noise_scale_y = 6000;
  }

  void FillNoise() {
    for (uint8_t i = 0; i < MATRIX_WIDTH; i++) {
      uint32_t ioffset = noise_scale_x * (i - MATRIX_CENTRE_Y);

      for (uint8_t j = 0; j < MATRIX_HEIGHT; j++) {
        uint32_t joffset = noise_scale_y * (j - MATRIX_CENTRE_Y);

        byte data = inoise16(noise_x + ioffset, noise_y + joffset, noise_z) >> 8;

        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8(olddata, noisesmoothing) + scale8(data, 256 - noisesmoothing);
        data = newdata;

        noise[i][j] = data;
      }
    }
  }

  // Oscillators and Emitters

  // the oscillators: linear ramps 0-255
  byte osci[6];

  // sin8(osci) swinging between 0 to MATRIX_WIDTH - 1
  byte p[6];

  // set the speeds (and by that ratios) of the oscillators here
  void MoveOscillators() {
    osci[0] = osci[0] + 5;
    osci[1] = osci[1] + 2;
    osci[2] = osci[2] + 3;
    osci[3] = osci[3] + 4;
    osci[4] = osci[4] + 1;
    if (osci[4] % 2 == 0)
      osci[5] = osci[5] + 1; // .5
    for (int i = 0; i < 4; i++) {
      p[i] = map8(sin8(osci[i]), 0, MATRIX_WIDTH - 1); //why? to keep the result in the range of 0-MATRIX_WIDTH (matrix size)
    }
  }
