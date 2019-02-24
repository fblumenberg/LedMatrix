#include <Arduino.h>
#include <FastLED.h>

#include "LedMatrix.h"

#include "Patterns.h"

#include "PatternFire.h"
#include "PatternMaze.h"
#include "PatternPendulumWave.h"
#include "PatternElectricMandala.h"
#include "PatternPlasma.h"
#include "PatternPlasma2.h"
#include "PatternMunch.h"
#include "PatternIncrementalDrift.h"
#include "PatternIncrementalDrift2.h"
#include "PatternInfinity.h"
#include "PatternLife.h"
#include "PatternSimplexNoise.h"
#include "Test.h"

Fire fire;
Maze maze;
PendulumWave pendulumWave;
ElectricMandala electricMandala;
PatternPlasma plasma;
PatternPlasma2 plasma2;
PatternMunch munch;
PatternIncrementalDrift incrementalDrift;
PatternIncrementalDrift2 incrementalDrift2;
PatternInfinity inf;
PatternLife life;
PatternSimplexNoise noisePattern;
Test test;

static Drawable *items[] = {
    &fire,
    &maze,
    &electricMandala,
    &pendulumWave,
    &plasma,
    &plasma2,
    &munch,
    &incrementalDrift,
    &incrementalDrift2,
    &inf,
    &noisePattern,
    &test,
    &life,
};

static const int patternCount = 13; //sizeof(items) / sizeof(Drawable *);

static Drawable *shuffledItems[patternCount];

Patterns::Patterns()
{
    // add the items to the shuffledItems array
    for (int a = 0; a < patternCount; a++)
    {
        shuffledItems[a] = items[a];
    }

    ShuffleItems();

    this->currentItem = items[0];
}

void Patterns::MoveTo(int index)
{
    if (currentItem)
        currentItem->Stop();

    currentIndex = index;

    currentItem = items[currentIndex];

    if (currentItem)
        currentItem->Start();
}

void Patterns::ShuffleItems()
{
    for (int a = 0; a < patternCount; a++)
    {
        int r = random(a, patternCount);
        Drawable *temp = shuffledItems[a];
        shuffledItems[a] = shuffledItems[r];
        shuffledItems[r] = temp;
    }
}

bool Patterns::SetPattern(String name)
{
    for (int i = 0; i < patternCount; i++)
    {
        Serial.printf("Check pattern %d/%d: %s == %s\n", i, patternCount, items[i]->name, name.c_str());
        if (name.compareTo(items[i]->name) == 0)
        {
            MoveTo(i);
            return true;
        }
    }

    return false;
}

bool Patterns::SetPattern(int index)
{
    if (index >= patternCount || index < 0)
        return false;

    MoveTo(index);

    return true;
}

void Patterns::GetList(JsonArray &array)
{
    for (int i = 0; i < patternCount; i++)
    {
        Serial.printf("Fill pattern %d/%d: %s\n", i, patternCount, items[i]->name);
        array.add(items[i]->name);
    }
}
