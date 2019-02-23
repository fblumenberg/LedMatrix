#pragma once

#include "Drawable.h"

class Maze : public Drawable
{
  private:
    enum Directions
    {
        None = 0,
        Up = 1,
        Down = 2,
        Left = 4,
        Right = 8,
    };

    struct Point
    {
        int x;
        int y;

        static Point New(int x, int y)
        {
            Point point;
            point.x = x;
            point.y = y;
            return point;
        }

        Point Move(Directions direction)
        {
            switch (direction)
            {
            case Up:
                return New(x, y - 1);

            case Down:
                return New(x, y + 1);

            case Left:
                return New(x - 1, y);

            case Right:
            default:
                return New(x + 1, y);
            }
        }

        static Directions Opposite(Directions direction)
        {
            switch (direction)
            {
            case Up:
                return Down;

            case Down:
                return Up;

            case Left:
                return Right;

            case Right:
            default:
                return Left;
            }
        }
    };

    static const int width = 5;
    static const int height = 5;

    int imageWidth = width * 2;
    int imageHeight = height * 2;

    Directions grid[width][height];

    Point point;

    Point cells[256];
    int cellCount = 0;

    int algorithm = 0;
    int algorithmCount = 1;

    byte hue = 0;
    byte hueOffset = 0;

    Directions directions[4] = {Up, Down, Left, Right};

    void removeCell(int index)
    { // shift cells after index down one
        for (int i = index; i < cellCount - 1; i++)
        {
            cells[i] = cells[i + 1];
        }

        cellCount--;
    }

    void shuffleDirections()
    {
        for (int a = 0; a < 4; a++)
        {
            int r = random(a, 4);
            Directions temp = directions[a];
            directions[a] = directions[r];
            directions[r] = temp;
        }
    }

    Point createPoint(int x, int y)
    {
        Point point;
        point.x = x;
        point.y = y;
        return point;
    }

    RgbColor chooseColor(int index)
    {
        byte h = index + hueOffset;

        switch (algorithm)
        {
        case 0:
        default:
            return matrix.ColorFromCurrentPalette(h);

        case 1:
            return RgbColor(Colors::Blue); //effects.ColorFromCurrentPalette(hue++);
        }
    }

    int chooseIndex(int max)
    {
        switch (algorithm)
        {
        case 0:
        default:
            // choose newest (recursive backtracker)
            return max - 1;

        case 1:
            // choose random(Prim's)
            return random(max);

            // case 2:
            //     // choose oldest (not good, so disabling)
            //     return 0;
        }
    }

    void generateMaze()
    {
        while (cellCount > 1)
        {
            drawNextCell();
        }
    }

    void drawNextCell()
    {
        int index = chooseIndex(cellCount);

        if (index < 0)
            return;

        point = cells[index];

        Point imagePoint = createPoint(point.x * 2, point.y * 2);

        shuffleDirections();

        RgbColor color = chooseColor(index);

        for (int i = 0; i < 4; i++)
        {
            Directions direction = directions[i];

            Point newPoint = point.Move(direction);
            if (newPoint.x >= 0 && newPoint.y >= 0 && newPoint.x < width && newPoint.y < height && grid[newPoint.y][newPoint.x] == None)
            {
                grid[point.y][point.x] = (Directions)((int)grid[point.y][point.x] | (int)direction);
                grid[newPoint.y][newPoint.x] = (Directions)((int)grid[newPoint.y][newPoint.x] | (int)point.Opposite(direction));

                Point newImagePoint = imagePoint.Move(direction);

                matrix.SetPixelColor(newImagePoint.x, newImagePoint.y, color);
                cellCount++;
                cells[cellCount - 1] = newPoint;

                index = -1;
                break;
            }
        }

        if (index > -1)
        {
            Point finishedPoint = cells[index];
            imagePoint = createPoint(finishedPoint.x * 2, finishedPoint.y * 2);
            matrix.SetPixelColor(imagePoint.x, imagePoint.y, color);

            removeCell(index);
        }
    }

  public:
    Maze()
    {
        name = (char *)"Maze";
    }

    unsigned int DrawFrame()
    {
        if (cellCount < 1)
        {
            matrix.FillScreen(RgbColor(Colors::Black));

            // reset the maze grid
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    grid[y][x] = None;
                }
            }

            int x = random(width);
            int y = random(height);

            cells[0] = createPoint(x, y);

            cellCount = 1;

            hue = 0;
            hueOffset = random(0, 256);
        }

        drawNextCell();

        if (cellCount < 1)
        {
            algorithm++;
            if (algorithm >= algorithmCount)
                algorithm = 0;

            return 1000;
        }

        return 0;
    }

    void Start()
    {
        matrix.FillScreen(RgbColor(Colors::Black));
        cellCount = 0;
        hue = 0;
    }
};
