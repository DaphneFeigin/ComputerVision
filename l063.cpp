#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <ostream>
#include <string>
#include <math.h>
#include <list>
#include <thread>
#include <stdio.h>
#include <stack>
#include <cstring>
#include <chrono>

using namespace std;

vector<string> split(string str)
{
    string s = "";
    vector<string> v;
    for (auto x : str)
    {
        if (x == ' ')
        {
            v.push_back(s);
            s = "";
        }
        else
        {
            s = s + x;
        }
    }
    v.push_back(s);
    return v;
}

class Canvas
{
private:
    int myWidth;
    int myHeight;
    int** arrayPointer;

public:
    Canvas(int width, int height)
    {
        myWidth = width;
        myHeight = height;
        int** array = new int* [height];
        for (int i = 0; i < height; i++)
            array[i] = new int[width];
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                array[i][j] = 0;
            }
        }
        arrayPointer = array;
    }

    int getHeight()
    {
        return myHeight;
    }

    int getWidth()
    {
        return myWidth;
    }

    int** getArrayPointer()
    {
        return arrayPointer;
    }

    void illuminate(int col, int row)
    {
        if (col < myWidth && row < myHeight && col >= 0 && row >= 0)
        {
            arrayPointer[row][col] += 1;
        }
    }

    // red=2, yellow=3, purple=4
    void illuminateColor(int col, int row, int color)
    {
        if (col < myWidth && row < myHeight && col >= 0 && row >= 0)
        {
            arrayPointer[row][col] = color;
        }
    }

    // somewhat obsolete, keeping it so I don't have to change a bunch of stuff
    void illuminateRed(int col, int row)
    {
        if (col < myWidth && row < myHeight && col >= 0 && row >= 0)
        {
            arrayPointer[row][col] = 2;
        }
    }

    void vote(int col, int row)
    {
        if (col < myWidth && row < myHeight && col >= 0 && row >= 0)
        {
            arrayPointer[row][col] += 1;
        }
    }

    int get(int row, int col)
    {
        return arrayPointer[row][col];
    }

    void create_ppm(string filename, int max)
    {
        ofstream myfile;
        myfile.open(filename);
        myfile << "P3  " << myWidth << "  " << myHeight << "  " << max << " \n";
        for (int i = 0; i < getHeight(); i++)
        {
            for (int j = 0; j < getWidth(); j++)
            {
                if (getArrayPointer()[i][j] == 2)
                {
                    myfile << 1 << " " << 0 << " " << 0 << " ";
                }
                else
                {
                    myfile << getArrayPointer()[i][j] << " " << getArrayPointer()[i][j] << " " << getArrayPointer()[i][j] << " ";
                }
            }
            myfile << "\n";
        }
        myfile.close();
    }

    void annotate_ppm(string original_image, string new_file, int width, int height)
    {
        ifstream myfile;
        myfile.open(original_image);
        vector<int> numbers;
        int lineat = 0;
        string myline = "";
        if (myfile.is_open())
        {
            while (myfile.good())
            {
                getline(myfile, myline); // assign line from file to myline
                if (myline.length() == 0)
                {
                    break;
                }
                vector<string> v = split(myline);
                if (v.size() > 4 || lineat > 3)
                {
                    for (int i = 0; i < v.size() - 1; i++)
                    {
                        // cout << i << ": adding " << v[i] << "to list of numbers" << endl;
                        numbers.push_back(stod(v[i]));
                    }
                }
                lineat++;
            }
        }

        ofstream myfile2;
        myfile2.open(new_file);
        myfile2 << "P3  " << width << "  " << height << "  255 ";
        int i = 0;
        for (int row = 0; row < height; row++)
        {
            for (int col = 0; col < width; col++)
            {
                // red
                if (get(row, col) == 2)
                {
                    myfile2 << "255 0 0 ";
                }
                // yellow
                else if (get(row, col) == 3)
                {
                    myfile2 << "255 255 0 ";
                }
                // purple
                else if (get(row, col) == 4)
                {
                    myfile2 << "128 0 128 ";
                }
                //blue
                else if (get(row, col) == 5) {
                    myfile2 << "0 0 255 ";
                }
                //pink
                else if (get(row, col) == 6) {
                    myfile2 << "255 0 255 ";
                }
                //green
                else if (get(row, col) == 7) {
                    myfile2 << "0 255 0 ";
                }
                //2=red, 3=yellow, 4=purple, 5=blue, 6=pink, 7=green
                else
                {
                    myfile2 << numbers[i] << " " << numbers[i + 1] << " " << numbers[i + 2] << " ";
                }
                i += 3;
            }
        }
    }
};

// draws the rasterized circle
class Circle
{
public:
    int centerX, centerY, radius;
    Canvas* myCanvas;
    Circle(int myX, int myY, int r, Canvas& canvas)
    {
        centerX = myX;
        centerY = myY;
        radius = r;
        myCanvas = &canvas;
    }
    void getPoints()
    {
        int x, y, xmax, y2, y2_new, ty;
        xmax = (int)(radius * 0.70710678); // maximum x at radius/sqrt(2)
        y = radius;
        y2 = y * y;
        ty = (2 * y) - 1;
        y2_new = y2;
        for (x = 0; x <= xmax; x++)
        {
            if ((y2 - y2_new) >= ty)
            {
                y2 -= ty;
                y -= 1;
                ty -= 2;
            }

            myCanvas->illuminate(centerX + x, centerY + y);
            myCanvas->illuminate(centerX + x, centerY - y);
            myCanvas->illuminate(centerX - x, centerY + y);
            myCanvas->illuminate(centerX - x, centerY - y);
            myCanvas->illuminate(centerX + y, centerY + x);
            myCanvas->illuminate(centerX + y, centerY - x);
            myCanvas->illuminate(centerX - y, centerY + x);
            myCanvas->illuminate(centerX - y, centerY - x);
            y2_new -= (2 * x) + 3;
        }
    }
    void getPointsRed()
    {
        int x, y, xmax, y2, y2_new, ty;
        xmax = (int)(radius * 0.70710678); // maximum x at radius/sqrt(2)
        y = radius;
        y2 = y * y;
        ty = (2 * y) - 1;
        y2_new = y2;
        for (x = 0; x <= xmax; x++)
        {
            if ((y2 - y2_new) >= ty)
            {
                y2 -= ty;
                y -= 1;
                ty -= 2;
            }

            myCanvas->illuminateRed(centerX + x, centerY + y);
            myCanvas->illuminateRed(centerX + x, centerY - y);
            myCanvas->illuminateRed(centerX - x, centerY + y);
            myCanvas->illuminateRed(centerX - x, centerY - y);
            myCanvas->illuminateRed(centerX + y, centerY + x);
            myCanvas->illuminateRed(centerX + y, centerY - x);
            myCanvas->illuminateRed(centerX - y, centerY + x);
            myCanvas->illuminateRed(centerX - y, centerY - x);
            y2_new -= (2 * x) + 3;
        }
    }

    void getPointsColor(int color)
    {
        int x, y, xmax, y2, y2_new, ty;
        xmax = (int)(radius * 0.70710678); // maximum x at radius/sqrt(2)
        y = radius;
        y2 = y * y;
        ty = (2 * y) - 1;
        y2_new = y2;
        for (x = 0; x <= xmax; x++)
        {
            if ((y2 - y2_new) >= ty)
            {
                y2 -= ty;
                y -= 1;
                ty -= 2;
            }

            myCanvas->illuminateColor(centerX + x, centerY + y, color);
            myCanvas->illuminateColor(centerX + x, centerY - y, color);
            myCanvas->illuminateColor(centerX - x, centerY + y, color);
            myCanvas->illuminateColor(centerX - x, centerY - y, color);
            myCanvas->illuminateColor(centerX + y, centerY + x, color);
            myCanvas->illuminateColor(centerX + y, centerY - x, color);
            myCanvas->illuminateColor(centerX - y, centerY + x, color);
            myCanvas->illuminateColor(centerX - y, centerY - x, color);
            y2_new -= (2 * x) + 3;
        }
    }
};

class xDrivenPosSlope
{
private:
    int x1;
    int x2;
    int y1;
    int y2;
    Canvas* myCanvas;

public:
    xDrivenPosSlope(int myx1, int myy1, int myx2, int myy2, Canvas* canvas)
    {
        // if point 2 is up && right from point 1
        if (myx2 > myx1 && myy2 > myy1)
        {
            x1 = myx1;
            y1 = myy1;
            x2 = myx2;
            y2 = myy2;
        }
        else
        {
            x1 = myx2;
            y1 = myy2;
            x2 = myx1;
            y2 = myy1;
        }
        myCanvas = canvas;
    }
    void getPoints()
    {
        int deltaX = x2 - x1;
        int deltaY = y2 - y1;
        int j = y1;
        int error = deltaY - deltaX;

        for (int i = x1; i < x2; i++)
        {
            myCanvas->illuminate(i, j);
            if (error > 0)
            {
                j++;
                error -= deltaX;
            }
            error += deltaY;
        }
    }
};

class xDrivenNegSlope
{
private:
    int x1;
    int x2;
    int y1;
    int y2;
    Canvas* myCanvas;

public:
    xDrivenNegSlope(int myx1, int myy1, int myx2, int myy2, Canvas* canvas)
    {
        // assumes y2 > y1 but x2 < x1
        if (myy2 > myy1 && myx2 < myx1)
        {
            x1 = myx1;
            y1 = myy1;
            y2 = myy2;
            x2 = (myx1 - myx2) + myx1;
        }
        else if (myy2 < myy1 && myx2 > myx1)
        {
            x1 = myx2;
            y1 = myy2;
            y2 = myy1;
            x2 = (myx2 - myx1) + myx2;
        }
        myCanvas = canvas;
    }
    void getPoints()
    {
        int deltaX = x2 - x1;
        int deltaY = y2 - y1;
        int j = y1;
        int error = deltaY - deltaX;

        for (int i = x1; i < x2; i++)
        {
            myCanvas->illuminate(x1 - (i - x1), j);
            // myCanvas->illuminate(i, j);
            if (error > 0)
            {
                j++;
                error -= deltaX;
            }
            error += deltaY;
        }
    }
    void vote()
    {
        int deltaX = x2 - x1;
        int deltaY = y2 - y1;
        int j = y1;
        int error = deltaY - deltaX;

        for (int i = x1; i < x2; i++)
        {
            myCanvas->illuminate(x1 - (i - x1), j);
            // myCanvas->illuminate(i, j);
            if (error > 0)
            {
                j++;
                error -= deltaX;
            }
            error += deltaY;
        }
    }
};

class yDrivenPosSlope
{
private:
    int x1, y1, x2, y2;
    Canvas* myCanvas;

public:
    yDrivenPosSlope(int myx1, int myy1, int myx2, int myy2, Canvas* canvas)
    {
        if (myy2 > myy1)
        {
            x1 = myx1;
            y1 = myy1;
            x2 = myx2;
            y2 = myy2;
        }
        else if (myy2 < myy1)
        {
            x1 = myx2;
            y1 = myy2;
            x2 = myx1;
            y2 = myy1;
        }
        myCanvas = canvas;
    }
    void getPoints()
    {
        int deltaX = x2 - x1;
        int deltaY = y2 - y1;
        int i = x1;
        int error = deltaX - deltaY;

        for (int j = y1; j < y2; j++)
        {
            myCanvas->illuminate(i, j);
            if (error > 0)
            {
                i++;
                error -= deltaY;
            }
            error += deltaX;
        }
    }
};

class yDrivenNegSlope
{
private:
    int x1;
    int x2;
    int y1;
    int y2;
    Canvas* myCanvas;

public:
    yDrivenNegSlope(int myx1, int myy1, int myx2, int myy2, Canvas* canvas)
    {
        // assumes y2 > y1 but x2 < x1
        if (myy2 > myy1 && myx2 < myx1)
        {
            x1 = myx1;
            y1 = myy1;
            y2 = myy2;
            x2 = (myx1 - myx2) + myx1;
        }
        else if (myy2 < myy1 && myx2 > myx1)
        {
            x1 = myx2;
            y1 = myy2;
            y2 = myy1;
            x2 = (myx2 - myx1) + myx2;
        }
        myCanvas = canvas;
    }
    void getPoints()
    {
        int deltaX = x2 - x1;
        int deltaY = y2 - y1;
        int i = x1;
        int error = deltaX - deltaY;

        for (int j = y1; j < y2; j++)
        {
            myCanvas->illuminate(x1 - (i - x1), j);
            if (error > 0)
            {
                i++;
                error -= deltaY;
            }
            error += deltaX;
        }
    }
};

class Horizontal
{
private:
    int myy;
    int myx1;
    int myx2;
    Canvas* myCanvas;

public:
    Horizontal(int x1, int y1, int x2, int y2, Canvas* canvas)
    {
        // cout << "Horizontal Line" << endl;
        myy = y1;
        if (x2 > x1)
        {
            myx1 = x1;
            myx2 = x2;
        }
        else
        {
            myx1 = x2;
            myx2 = x1;
        }
        myCanvas = canvas;
    }
    void getPoints()
    {
        for (int i = myx1; i <= myx2; i++)
        {
            myCanvas->illuminate(i, myy);
        }
    }
};

class Vertical
{
private:
    int x;
    int y1;
    int y2;
    Canvas* myCanvas;

public:
    Vertical(int myx1, int myy1, int myx2, int myy2, Canvas* canvas)
    {
        x = myx1;
        if (myy2 > myy1)
        {
            y1 = myy1;
            y2 = myy2;
        }
        else
        {
            y1 = myy2;
            y2 = myy1;
        }
        myCanvas = canvas;
    }
    void getPoints()
    {
        for (int j = y1; j <= y2; j++)
        {
            myCanvas->illuminate(x, j);
        }
    }
};

class Line
{
private:
    int x1;
    int y1;
    int x2;
    int y2;
    Canvas* myCanvas;

public:
    Line(double x1, double y1, double x2, double y2, Canvas& myCanvas)
    {
        this->x1 = x1 * myCanvas.getWidth();
        this->y1 = y1 * myCanvas.getHeight();
        this->x2 = x2 * myCanvas.getWidth();
        this->y2 = y2 * myCanvas.getHeight();
        this->myCanvas = &myCanvas;
    }
    // may need to come back && modify for vertical edge case
    Line(int x, int y, double angle, Canvas& myCanvas)
    {
        // given: the actual integer point on the Canvas, the angle
        // need: the intersection points
        double slope = tan(angle);
        // CHANGED FROM 150 TO 200
        double k = 150 / sqrt(1 + slope * slope);
        int xa = x + k;
        int xb = x - k;
        int ya = y + k * slope;
        int yb = y - k * slope;

        double intercept = y - slope * x;
        // cout << "y-intercept: " << intercept << endl;
        x1 = xa;
        x2 = xb;
        y1 = ya;
        y2 = yb;
        this->myCanvas = &myCanvas;
        //         int b = round(y - slope * x);
        //         x1 = x - 200;
        //         y1 = round(slope * x1 + intercept);
        //         x2 = x + 200;
        //         y2 = round(slope * x2 + intercept);
        //         this->myCanvas = myCanvas;
    }
    void drawLine()
    {
        // cout << "in the draw line method" << endl;
        // cout << "Line's given values:  x1: " << x1 << " y1: "<< y1 << " x2: "<< x2 << " y2: " << y2 <<  endl;
        double dy = y2 - y1;
        double dx = x2 - x1;
        double slope = dy / dx;
        // cout << "slope again (in drawLine method) = " << slope << endl;
        if (slope >= 1)
        {
            yDrivenPosSlope line(x1, y1, x2, y2, myCanvas);
            line.getPoints();
        }
        else if (slope > 0)
        {
            xDrivenPosSlope line(x1, y1, x2, y2, myCanvas);
            line.getPoints();
        }
        else if (slope <= -1)
        {
            yDrivenNegSlope line(x1, y1, x2, y2, myCanvas);
            line.getPoints();
        }
        else if (slope < 0)
        {
            xDrivenNegSlope line(x1, y1, x2, y2, myCanvas);
            line.getPoints();
        }
        else if (slope == 0)
        {
            Horizontal line(x1, y1, x2, y2, myCanvas);
            line.getPoints();
        }
        else
        {
            // vertical
            Vertical line(x1, y1, x2, y2, myCanvas);
            line.getPoints();
        }
    }
};

// Canvas + lines (already written classes) **********************************************************************************************************************

vector<vector<int>> create_empty(int width, int height)
{
    vector<vector<int>> empty;
    for (int row = 0; row < height; row++)
    {
        vector<int> v;
        empty.push_back(v);
        for (int col = 0; col < width; col++)
        {
            empty[row].push_back(0);
        }
    }
    // cout << "empty width: " << width << " empty height: " << height << endl;
    return empty;
}

vector<vector<int>> grayscale(string filename, string fg)
{
    // cout << "in grayscale method" << endl;
    vector<vector<int>> grayscale;
    vector<int> numbers;
    string myline = "";
    ifstream myfile;
    myfile.open(filename);
    int lineat = 0;
    int row = 0;
    int col = 0;
    int width = 0;
    int height = 0;

    if (myfile.is_open())
    {
        while (myfile.good())
        {
            getline(myfile, myline); // assign line from file to myline
            if (myline.length() == 0)
            {
                break;
            }
            vector<string> v = split(myline);
            // for (int i = 0; i < v.size(); i++) {
            //     cout << v[i] << endl;
            // }
            if (v.size() <= 4 && lineat < 4)
            {
                if (v.size() == 2)
                {
                    width = stod(v[0]);
                    height = stod(v[1]);
                }
                if (v.size() == 4)
                {
                    width = stod(v[1]);
                    height = stod(v[2]);
                }
                grayscale = create_empty(width, height);
            }
            else
            {
                for (int i = 0; i < v.size() - 1; i++)
                {
                    // cout << i << ": adding " << v[i] << "to list of numbers" << endl;
                    numbers.push_back(stod(v[i]));
                }
            }
            lineat++;
        }
    }
    // NEED ACCESS TO NUMBERS

    // cout << "finished adding numbers to the list" << endl;

    for (int i = 0; i < numbers.size(); i += 3)
    {
        int gray = numbers[i] + numbers[i + 1] + numbers[i + 2];
        gray = gray / 3;
        grayscale[row][col] = gray;
        col++;
        if (col == width)
        {
            // cout << "at final column: " << col << " && row: " << row << endl;
            row++;
            col = 0;
            // cout << "moving to row " << row << endl;
        }
    }

    // generate the ppm
    ofstream ppmwriter;
    ppmwriter.open(fg);
    ppmwriter << "P3  " << width << "  " << height << "  255 \n";
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            ppmwriter << grayscale[row][col] << " " << grayscale[row][col] << " " << grayscale[row][col] << " ";
        }
        ppmwriter << "\n";
    }
    ppmwriter.close();

    return grayscale;
}

vector<vector<int>> gaussian_blur(vector<vector<int>>& gs, int width, int height)
{
    vector<vector<int>> gb = create_empty(width, height);
    for (int row = 2; row < height - 2; row++)
    {
        for (int col = 2; col < width - 2; col++)
        {
            int row1 = gs[row - 2][col - 2] + 4 * gs[row - 2][col - 1] + 6 * gs[row - 2][col] + 4 * gs[row - 2][col + 1] + gs[row - 2][col + 2];
            // cout << "row 1: " << row1 << endl;
            int row2 = 4 * gs[row - 1][col - 2] + 16 * gs[row - 1][col - 1] + 24 * gs[row - 1][col] + 16 * gs[row - 1][col + 1] + 4 * gs[row - 1][col + 2];
            int row3 = 6 * gs[row][col - 2] + 24 * gs[row][col - 1] + 36 * gs[row][col] + 24 * gs[row][col + 1] + 6 * gs[row][col + 2];
            int row4 = 4 * gs[row + 1][col - 2] + 16 * gs[row + 1][col - 1] + 24 * gs[row + 1][col] + 16 * gs[row + 1][col + 1] + 4 * gs[row + 1][col + 2];
            int row5 = gs[row + 2][col - 2] + 4 * gs[row + 2][col - 1] + 6 * gs[row + 2][col] + 4 * gs[row + 2][col + 1] + gs[row + 2][col + 2];
            gb[row][col] = (1.0 / 256) * (row1 + row2 + row3 + row4 + row5);
        }
    }
    return gb;
}

vector<vector<int>> gaussian_x(vector<vector<int>>& intensity, int width, int height)
{
    vector<vector<int>> gaussian_x = create_empty(width, height);
    for (int row = 1; row < height - 1; row++)
    {
        for (int col = 1; col < width - 1; col++)
        {
            gaussian_x[row][col] = 1 * intensity[row - 1][col - 1] - 1 * intensity[row - 1][col + 1] + 2 * intensity[row][col - 1] - 2 * intensity[row][col + 1] + 1 * intensity[row + 1][col - 1] - 1 * intensity[row + 1][col + 1];
        }
    }
    return gaussian_x;
}

vector<vector<int>> gaussian_y(vector<vector<int>>& intensity, int width, int height)
{
    vector<vector<int>> gaussian_y = create_empty(width, height);
    for (int row = 1; row < height - 1; row++)
    {
        for (int col = 1; col < width - 1; col++)
        {
            // gaussian_y[row][col] = -1 * intensity[row - 1][col - 1] + 1 * intensity[row - 1][col + 1] + 1 * intensity[row + 1][col - 1] + 2 * intensity[row + 1][col] + 1 * intensity[row + 1][col + 1];
            gaussian_y[row][col] = 1 * intensity[row - 1][col - 1] - 1 * intensity[row + 1][col - 1] + 2 * intensity[row - 1][col] - 2 * intensity[row + 1][col] + 1 * intensity[row - 1][col + 1] - 1 * intensity[row + 1][col + 1];
        }
    }
    return gaussian_y;
}

vector<vector<int>> combine_gx_gy(vector<vector<int>>& gx, vector<vector<int>>& gy, int width, int height)
{
    vector<vector<int>> combined = create_empty(width, height);
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            combined[row][col] = gx[row][col] * gx[row][col] + gy[row][col] * gy[row][col];
        }
    }
    return combined;
}

bool isValid(vector<vector<int>>& combined, int width, int height, int row, int col)
{
    if (row < 0 || row >= height || col < 0 || col >= height || combined[row][col] == 0 || combined[row][col] == 3)
    {
        return false;
    }
    return true;
}

void change_ones(vector<vector<int>>& combined, int width, int height, int row, int col)
{
    // cout << "row: " << row << " col: " << col << endl;
    if (row < 0 || row >= height || col < 0 || col >= width)
    {
        return;
    }
    if (combined[row][col] == 0 || combined[row][col] == 3)
    {
        return;
    }
    // cout << "row: " << row << " col: " << col << endl;
    combined[row][col] = 3;
    change_ones(combined, width, height, row - 1, col - 1); // top left
    change_ones(combined, width, height, row - 1, col);     // above
    change_ones(combined, width, height, row - 1, col + 1); // top right
    change_ones(combined, width, height, row, col - 1);     // left
    change_ones(combined, width, height, row, col + 1);     // right
    change_ones(combined, width, height, row + 1, col - 1); // bottom left
    change_ones(combined, width, height, row + 1, col);     // below
    change_ones(combined, width, height, row + 1, col + 1); // bottom right
}

// goes through && finds all twos where recursive call must be made
void loop_through_twos(vector<vector<int>>& combined, int width, int height)
{
    // cout << combined[0].size() << " " << combined.size() << endl;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (combined[row][col] == 2)
            {
                change_ones(combined, width, height, row, col);
            }
        }
    }

    // changes each remaining 1 to 0
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (combined[row][col] == 1)
                combined[row][col] = 0;
        }
    }
}

vector<vector<double>> get_angles(vector<vector<int>>& gx, vector<vector<int>>& gy, int width, int height)
{
    // create empty vector of correct size with doubles
    vector<vector<double>> angles;
    for (int row = 0; row < height; row++)
    {
        vector<double> v;
        angles.push_back(v);
        for (int col = 0; col < width; col++)
        {
            angles[row].push_back(0);
        }
    }

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            angles[row][col] = atan2(gy[row][col], gx[row][col]);
        }
    }
    return angles;
}

vector<vector<int>> double_threshold(vector<vector<int>>& combined, int width, int height, int lower, int upper, string f1)
{
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (combined[row][col] < lower)
                combined[row][col] = 0;
            else if (combined[row][col] > upper)
                combined[row][col] = 2;
            else
                combined[row][col] = 1;
        }
    }
    // cout << "done with first for loop" << endl;
    loop_through_twos(combined, width, height);
    // cout << "done looping through twos" << endl;

    ofstream myfile;
    myfile.open(f1);
    myfile << "P3  " << width << "  " << height << "  1 ";
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (combined[row][col] == 0)
            {
                myfile << "0 0 0 ";
            }
            else
            {
                myfile << "1 1 1 ";
            }
        }
    }
    return combined;
}

vector<vector<int>> angles_nms(vector<vector<int>>& gx, vector<vector<int>>& gy, int width, int height)
{
    vector<vector<int>> angles = create_empty(width, height);
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            double tan = atan2(-1 * gy[row][col], gx[row][col]);
            // radians to degrees: * 180/pi
            double degrees = tan * (57.2957795);
            int rounded = 0;
            if (degrees <= -157.5)
                rounded = -180;
            else if (degrees <= -112.5)
                rounded = -135;
            else if (degrees <= -67.5)
                rounded = -90;
            else if (degrees <= -22.5)
                rounded = -45;
            else if (degrees <= 22.5)
                rounded = 0;
            else if (degrees <= 67.5)
                rounded = 45;
            else if (degrees <= 112.5)
                rounded = 90;
            else if (degrees <= 157.5)
                rounded = 135;
            else
                rounded = 180;
            // cout << degrees << " rounded to: " << rounded << endl;
            angles[row][col] = rounded;
        }
    }
    return angles;
}

vector<vector<int>> nms(vector<vector<int>>& edges, vector<vector<int>>& angles, vector<vector<int>>& combined, int width, int height, string f2, string ff)
{
    vector<vector<int>> image2 = create_empty(width, height);
    vector<vector<int>> finaledges = create_empty(width, height);
    for (int row = 1; row < height - 1; row++)
    {
        for (int col = 1; col < width - 1; col++)
        {
            bool edge = false;
            if (angles[row][col] == 0 || angles[row][col] == 180 || angles[row][col] == -180)
            {
                if (combined[row][col] >= combined[row][col - 1] && combined[row][col] >= combined[row][col + 1])
                {
                    edge = true;
                }
            }
            else if (angles[row][col] == 45 || angles[row][col] == -135)
            {
                if (combined[row][col] >= combined[row - 1][col + 1] && combined[row][col] >= combined[row + 1][col - 1])
                {
                    edge = true;
                }
            }
            else if (angles[row][col] == 90 || angles[row][col] == -90)
            {
                if (combined[row][col] >= combined[row - 1][col] && combined[row][col] >= combined[row + 1][col])
                {
                    edge = true;
                }
            }
            else if (angles[row][col] == 135 || angles[row][col] == -45)
            {
                if (combined[row][col] >= combined[row + 1][col + 1] && combined[row][col] >= combined[row - 1][col - 1])
                {
                    edge = true;
                }
            }
            // cout << edge << endl;
            if (edge == true)
                image2[row][col] = 1;

            if (edge == true && edges[row][col] != 0)
            {
                finaledges[row][col] = 1;
            }
        }
    }

    ofstream myfile;
    // image2.ppm
    myfile.open(f2);
    myfile << "P3  " << width << "  " << height << "  1 ";
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (image2[row][col] == 0)
            {
                myfile << "0 0 0 ";
            }
            else
            {
                myfile << "1 1 1 ";
            }
        }
    }

    ofstream myfile2;
    // imagef.ppm
    myfile2.open(ff);
    myfile2 << "P3  " << width << "  " << height << "  1 ";
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (finaledges[row][col] == 0)
            {
                myfile2 << "0 0 0 ";
            }
            else
            {
                myfile2 << "1 1 1 ";
            }
        }
    }

    return finaledges;
}

vector<vector<int>> center_voting(vector<vector<int>>& edges, vector<vector<double>>& angles, int width, int height, int tc, string f, string fv, string fcc)
{
    // create a voting canvas
    auto start = std::chrono::high_resolution_clock::now();
    Canvas votesCanvas = Canvas(width, height);
    int max = 0;
    int count = 0;
    int threshold = tc;
    vector<vector<int>> votes = create_empty(width, height);
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (edges[row][col] != 0)
            {
                Line l = Line(col, row, angles[row][col], votesCanvas);
                l.drawLine();
                if (votesCanvas.get(row, col) > max)
                {
                    max = votesCanvas.get(row, col);
                }
            }
        }
    }
    votesCanvas.create_ppm(fv, max);

    vector<vector<int>> centers = create_empty(width, height);
    Canvas circles(width, height);
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            // added this
            if (votesCanvas.get(row, col) >= threshold)
            {
                centers[row][col] = 1;
                Circle c1(col, row, 1, circles);
                Circle c2(col, row, 2, circles);
                Circle c3(col, row, 3, circles);
                Circle c4(col, row, 4, circles);
                c1.getPointsRed();
                c2.getPointsRed();
                c3.getPointsRed();
                c4.getPointsRed();
                count++;
            }
        }
    }

    circles.annotate_ppm(f, fcc, width, height);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> double_ms = end - start;
    cout << "number of centers: " << count << endl;
    // cout << "time to run: " << double_ms.count() << "ms" << endl;
    return centers;
}

int check_edge(vector<vector<int>>& edges, int row, int col, int& width, int& height)
{
    if (row < 0 || row >= height || col < 0 || col >= width)
        return 0;
    if (edges[row][col] != 0)
        return 1;
    return 0;
}

int edge_exists(int row, int col, int& width, int& height)
{
    if (row < 0 || row >= height || col < 0 || col >= width)
        return 0;
    return 1;
}

int check_circle_edges(vector<vector<int>>& edges, int centerX, int centerY, int& radius, int& width, int& height)
{
    int yes = 0;
    // int total = 0;
    int x, y, xmax, y2, y2_new, ty;
    xmax = (int)(radius * 0.70710678); // maximum x at radius/sqrt(2)
    y = radius;
    y2 = y * y;
    ty = (2 * y) - 1;
    y2_new = y2;
    for (x = 0; x <= xmax; x++)
    {
        if ((y2 - y2_new) >= ty)
        {
            y2 -= ty;
            y -= 1;
            ty -= 2;
        }
        yes += check_edge(edges, centerY + y, centerX + x, width, height);
        // total += edge_exists(centerY + y, centerX + x, width, height);

        yes += check_edge(edges, centerY - y, centerX + x, width, height);
        // total += edge_exists(centerY - y, centerX + x, width, height);

        yes += check_edge(edges, centerY + y, centerX - x, width, height);
        // total += edge_exists(centerY + y, centerX - x, width, height);

        yes += check_edge(edges, centerY - y, centerX - x, width, height);
        // total += edge_exists(centerY - y, centerX - x, width, height);

        yes += check_edge(edges, centerY + x, centerX + y, width, height);
        // total += edge_exists(centerY + x, centerX + y, width, height);

        yes += check_edge(edges, centerY - x, centerX + y, width, height);
        // total += edge_exists(centerY - x, centerX + y, width, height);

        yes += check_edge(edges, centerY + x, centerX - y, width, height);
        // total += edge_exists(centerY + x, centerX - y, width, height);

        yes += check_edge(edges, centerY - x, centerX - y, width, height);
        // total += edge_exists(centerY - x, centerX - y, width, height);
    }
    return yes;
}

class PossibleCircle
{
private:
    int radius;
    int centerRow;
    int centerCol;
    int edges;

public:
    PossibleCircle(int r, int row, int col, int e)
    {
        radius = r;
        centerRow = row;
        centerCol = col;
        edges = e;
    }
    int getRadius()
    {
        return radius;
    }
    int getRow()
    {
        return centerRow;
    }
    int getCol()
    {
        return centerCol;
    }
    int getEdges()
    {
        return edges;
    }
    double distanceTo(PossibleCircle* c)
    {
        return sqrt((centerRow - c->getRow()) * (centerRow - c->getRow()) + (centerCol - c->getCol()) * (centerCol - c->getCol()));
    }
};

// get the ratio of red to blue && green
vector<vector<bool>> get_ratios(string original, int width, int height)
{
    // cout << original << " " << width <<" " << height;
    // cout << "started get_ratios" << endl;
    ifstream myfile;
    myfile.open(original);
    vector<int> numbers;
    int lineat = 0;
    string myline = "";
    if (myfile.is_open())
    {
        while (myfile.good())
        {
            getline(myfile, myline); // assign line from file to myline
            if (myline.length() == 0)
            {
                break;
            }
            vector<string> v = split(myline);
            if (v.size() > 4 || lineat > 3)
            {
                for (int i = 0; i < v.size() - 1; i++)
                {
                    // cout << i << ": adding " << v[i] << "to list of numbers" << endl;
                    numbers.push_back(stod(v[i]));
                }
            }
            lineat++;
        }
    }

    // cout << "done getting numbers1d" << endl;

    myfile.close();

    vector<vector<bool>> numbers2d;
    for (int row = 0; row < height; row++)
    {
        vector<bool> v;
        numbers2d.push_back(v);
        for (int col = 0; col < width; col++)
        {
            numbers2d[row].push_back(false);
        }
    }

    // cout << "done making empty numbers2d" << endl;
    // cout << "size: " << numbers.size();

    int i = 0;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            // cout << "looking at index " << i  << ", " << i+1 << ", && " << i+2 << endl;
            numbers2d[row][col] = ((numbers[i] > (numbers[i + 1] + 7)) && (numbers[i] > (numbers[i + 2] + 10)));
            i += 3;
        }
    }

    // cout << "done updating numbers2d" << endl;

    return numbers2d;
}

int find_mode_radius(vector<PossibleCircle>& circles)
{
    vector<int> radii;
    for (int i = 0; i < circles.size(); i++)
    {
        radii.push_back(circles[i].getRadius());
    }
    sort(radii.begin(), radii.end());
    int count = 1;
    int max_occur = 1;
    int moder = radii[0];
    for (int i = 1; i < radii.size(); i++)
    {
        if (radii[i] == radii[i - 1])
            count++;
        else
            count = 1;
        if (count > max_occur)
        {
            max_occur = count;
            moder = radii[i];
        }
    }
    return moder;
}

void classify_circles(vector<PossibleCircle>& circles, string f, string fCo, string fR, int oc, int width, int height)
{
    // cout << "started classifying circles" << endl;
    // cout << "number of circles: " << circles.size() << " (should be 66)" << endl;
    Canvas colored_circles(width, height);
    vector<vector<bool>> ratios = get_ratios(f, width, height);
    // cout << "finished call to get_ratios" << endl;
    int quarters = 0;
    int nickels = 0;
    int pennies = 0;
    int dimes = 0;
    int hds = 0;
    int dollars = 0;

    int penny = find_mode_radius(circles);
    double dime = (.705/.750) * penny;
    double nickel = (0.835/.750) * penny;
    double quarter = (0.955 / .750) * penny;
    double halfd = (1.205/.750) * penny;
    double dollar = (1.043 / .750) * penny;
    //cout << "penny (mode): " << penny << endl;
    //cout << "quarter: " << quarter << endl;

    for (int i = 0; i < circles.size(); i++)
    {
        int row = circles[i].getRow();
        int col = circles[i].getCol();
        int r = circles[i].getRadius();
        int color;

        vector<double> check;
        check.push_back(abs(r - penny));
        check.push_back(abs(r - nickel));
        check.push_back(abs(r - quarter));
        if (oc == 1) {
            check.push_back(abs(r - dollar));
            check.push_back(abs(r - dime));
            check.push_back(abs(r - halfd));
        }

        sort(check.begin(), check.end());

        if (ratios[row][col])
        {
            color = 2;
            pennies++;
        }
        else {
            //2=red, 3=yellow, 4=purple, 5=blue, 6=pink, 7=green
            if (check[0] == abs(r - dime)) {
                dimes++;
                color = 5;
            }
            else if (check[0] == abs(r - nickel) or check[0] == abs(r-penny)) {
                nickels++;
                color = 3;
            }
            else if (check[0] == abs(r - quarter)) {
                quarters++;
                color = 4;
            }
            else if (check[0] == abs(r - halfd)) {
                hds++;
                color = 7;
            }
            else if (check[0] == abs(r - dollar)) {
                dollars++;
                color = 6;
            }
        }
        Circle c(col, row, r, colored_circles);
        c.getPointsColor(color);
        c = Circle(col, row, r - 1, colored_circles);
        c.getPointsColor(color);
        c = Circle(col, row, r + 1, colored_circles);
        c.getPointsColor(color);
        Circle c1 = Circle(col, row, 1, colored_circles);
        Circle c2 = Circle(col, row, 2, colored_circles);
        Circle c3 = Circle(col, row, 3, colored_circles);
        Circle c4 = Circle(col, row, 4, colored_circles);
        c1.getPointsColor(color);
        c2.getPointsColor(color);
        c3.getPointsColor(color);
        c4.getPointsColor(color);
    }
    // generate color-coded image
    colored_circles.annotate_ppm(f, fCo, width, height);
    // generate text file
    ofstream myfile;
    myfile.open(fR);
    myfile << dollars << " dollars, " << hds << " half dollars, " << quarters << " quarters, " << dimes << " dimes, " << nickels << " nickels, "
        << pennies << " pennies" << endl;
    double total = dollars + 0.5 * hds + quarters * 0.25 + dimes * 0.1 + nickels * 0.05 + pennies * 0.01;
    myfile << "Total sum: $" << setprecision(3) << total;
    myfile.close();

    cout << dollars << " dollars, " << hds << " half dollars, " << quarters << " quarters, " << dimes << " dimes, " << nickels << " nickels, "
        << pennies << " pennies" << endl;
    cout << "Total sum: $" << setprecision(3) << total << endl;
}

void find_circles(vector<vector<int>>& edges, vector<vector<int>>& mbcenters, int tcircle, string f, string fCi, string fCo, string fR, int minD, int minR, int maxR, int oc, int width, int height)
{
    // convert percentage to decimal
    double minP = tcircle / 100.0;
    // Canvas* circles = new Canvas(width, height);
    vector<PossibleCircle> mbcircles;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (mbcenters[row][col] != 0)
            {
                for (int i = minR; i <= maxR; i++)
                {
                    int edge_count = check_circle_edges(edges, col, row, i, width, height);
                    double proportion = edge_count / (2 * 3.14 * i);
                    if (proportion > minP)
                    {
                        auto c = PossibleCircle(i, row, col, edge_count); // CHANGED
                        mbcircles.push_back(c);
                    }
                }
            }
        }
    }

    vector<PossibleCircle> actualcircles;
    for (int i = 0; i < mbcircles.size(); i++)
    {
        bool winner = true;
        vector<PossibleCircle*> options;
        for (int j = 0; j < mbcircles.size(); j++)
        {
            if (mbcircles[i].distanceTo(&mbcircles[j]) <= minD)
            {
                if (mbcircles[i].getEdges() < mbcircles[j].getEdges())
                    winner = false;
                // if they both have the same number of edges, we will pick the one that comes later in the mbcircles vector (arbitrary)
                else if (mbcircles[i].getEdges() == mbcircles[j].getEdges() && i < j)
                {
                    winner = false;
                }
            }
        }
        if (winner)
            actualcircles.push_back(mbcircles[i]);
    }

    Canvas foundcircles(width, height);
    for (int i = 0; i < actualcircles.size(); i++)
    {
        int r = actualcircles[i].getRadius();
        int row = actualcircles[i].getRow();
        int col = actualcircles[i].getCol();
        cout << "row: " << row << " col: " << col << " radius: " << r << endl;
        auto c = Circle(col, row, r, foundcircles);
        c.getPointsColor(2);
        c = Circle(col, row, r - 1, foundcircles);
        c.getPointsColor(2);
        c = Circle(col, row, r + 1, foundcircles);
        c.getPointsColor(2);
        auto c1 = Circle(col, row, 1, foundcircles);
        auto c2 = Circle(col, row, 2, foundcircles);
        auto c3 = Circle(col, row, 3, foundcircles);
        auto c4 = Circle(col, row, 4, foundcircles);
        c1.getPointsColor(2);
        c2.getPointsColor(2);
        c3.getPointsColor(2);
        c4.getPointsColor(2);
    }

    foundcircles.annotate_ppm(f, fCi, width, height);

    classify_circles(actualcircles, f, fCo, fR, oc, width, height);
}

void part1(int argc, char** argv)
{
    string f = "coins.ppm";
    int lt = 100 * 100;
    int ht = 140 * 140;
    string fg = "imageg.ppm";
    string f1 = "image1.ppm";
    string f2 = "image2.ppm";
    string ff = "imagef.ppm";
    int tc = 33;
    string fv = "imagev.ppm";
    string fcc = "imageCC.ppm";
    int oc = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            f = argv[i + 1];
        }
        else if (strcmp(argv[i], "-lt") == 0)
        {
            lt = stoi(argv[i + 1]) * stoi(argv[i + 1]);
            // cout << lt << endl;
        }
        else if (strcmp(argv[i], "-ht") == 0)
        {
            ht = stoi(argv[i + 1]) * stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-fg") == 0)
        {
            fg = argv[i + 1];
        }
        else if (strcmp(argv[i], "-f1") == 0)
        {
            f1 = argv[i + 1];
        }
        else if (strcmp(argv[i], "-f2") == 0)
        {
            f2 = argv[i + 1];
        }
        else if (strcmp(argv[i], "-ff") == 0)
        {
            ff = argv[i + 1];
        }
        else if (strcmp(argv[i], "-TC") == 0)
        {
            tc = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-fv") == 0)
        {
            fv = argv[i + 1];
        }
        else if (strcmp(argv[i], "-fcc") == 0)
        {
            fcc = argv[i + 1];
        }
        else if (strcmp(argv[i], "-oc") == 0)
        {
            oc = stoi(argv[i + 1]);
        }
    }

    cout << "variables processed" << endl;
    vector<vector<int>> intensity = grayscale(f, fg);
    cout << "grayscale done" << endl;
    vector<vector<int>> gx = gaussian_x(intensity, intensity[0].size(), intensity.size());
    cout << "gx done" << endl;
    vector<vector<int>> gy = gaussian_y(intensity, intensity[0].size(), intensity.size());
    cout << "gx && gy done" << endl;
    vector<vector<int>> combined = combine_gx_gy(gx, gy, intensity[0].size(), intensity.size());
    cout << "combining done" << endl;
    vector<vector<int>> edges = double_threshold(combined, intensity[0].size(), intensity.size(), lt, ht, f1);
    cout << "double threshold done" << endl;
    vector<vector<int>> a_nms = angles_nms(gx, gy, intensity[0].size(), intensity.size());
    edges = nms(edges, a_nms, combined, intensity[0].size(), intensity.size(), f2, ff);
    vector<vector<double>> angles = get_angles(gx, gy, intensity[0].size(), intensity.size());
    center_voting(edges, angles, intensity[0].size(), intensity.size(), tc, f, fv, fcc);
}

void part3(int argc, char** argv)
{
    // WORKS BUT SLOW: 40-70, 90, 0.45
    // also works: 50-90, 90, 0.375
    // better than the 50-90 but very slow: 60-100, 90, 0.3
    // EASY IMAGE: 60-100, 90, 30
    string f = "coins.ppm";
    //     int lt = 60 * 60;
    //     int ht = 120 * 120;
    int lt = 60 * 60;
    int ht = 100 * 100;
    string fg = "imageg.ppm";
    string f1 = "image1.ppm";
    string f2 = "image2.ppm";
    string ff = "imagef.ppm";
    int tc = 90;
    string fv = "imagev.ppm";
    string fcc = "imageCC.ppm";
    int tcircle = 30;
    int minD = 100;
    string fCi = "imageCircles.ppm";
    string fCo = "imageCoins.ppm";
    string fR = "results.txt";
    int minR = 85;
    int maxR = 130;
    int oc = 0;
    // string tci = "myImageCircles.ppm"

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            f = argv[i + 1];
            cout << "file: " << f << endl;
        }
        else if (strcmp(argv[i], "-lt") == 0)
        {
            lt = stoi(argv[i + 1]) * stoi(argv[i + 1]);
            // cout << lt << endl;
        }
        else if (strcmp(argv[i], "-ht") == 0)
        {
            ht = stoi(argv[i + 1]) * stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-fg") == 0)
        {
            fg = argv[i + 1];
        }
        else if (strcmp(argv[i], "-f1") == 0)
        {
            f1 = argv[i + 1];
        }
        else if (strcmp(argv[i], "-f2") == 0)
        {
            f2 = argv[i + 1];
        }
        else if (strcmp(argv[i], "-ff") == 0)
        {
            ff = argv[i + 1];
        }
        else if (strcmp(argv[i], "-TC") == 0)
        {
            tc = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-fv") == 0)
        {
            fv = argv[i + 1];
        }
        else if (strcmp(argv[i], "-fcc") == 0)
        {
            fcc = argv[i + 1];
        }
        else if (strcmp(argv[i], "-TCircle") == 0)
        {
            tcircle = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-minD") == 0)
        {
            minD = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-fCi") == 0)
        {
            fCi = argv[i + 1];
        }
        else if (strcmp(argv[i], "-fCo") == 0)
        {
            fCo = argv[i + 1];
        }
        else if (strcmp(argv[i], "-fR") == 0)
        {
            fR = argv[i + 1];
        }
        else if (strcmp(argv[i], "-minR") == 0)
        {
            minR = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-maxR") == 0)
        {
            maxR = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-oc") == 0)
        {
            oc = stoi(argv[i + 1]);
        }
    }

    cout << "variables processed" << endl;
    vector<vector<int>> intensity = grayscale(f, fg);
    cout << "grayscale done" << endl;
    intensity = gaussian_blur(intensity, intensity[0].size(), intensity.size());
    cout << "blurring done" << endl;
    vector<vector<int>> gx = gaussian_x(intensity, intensity[0].size(), intensity.size());
    cout << "gx done" << endl;
    vector<vector<int>> gy = gaussian_y(intensity, intensity[0].size(), intensity.size());
    cout << "gx && gy done" << endl;
    vector<vector<int>> combined = combine_gx_gy(gx, gy, intensity[0].size(), intensity.size());
    cout << "combining done" << endl;
    vector<vector<int>> combined_unchanged = combined;
    vector<vector<int>> edges = double_threshold(combined, intensity[0].size(), intensity.size(), lt, ht, f1);
    cout << "double threshold done" << endl;
    vector<vector<int>> a_nms = angles_nms(gx, gy, intensity[0].size(), intensity.size());
    vector<vector<int>> nms_edges = nms(edges, a_nms, combined_unchanged, intensity[0].size(), intensity.size(), f2, ff);
    cout << "nms done" << endl;

    vector<vector<double>> angles = get_angles(gx, gy, intensity[0].size(), intensity.size());

    vector<vector<int>> centers = center_voting(edges, angles, intensity[0].size(), intensity.size(), tc, f, fv, fcc);

    find_circles(edges, centers, tcircle, f, fCi, fCo, fR, minD, minR, maxR, oc, intensity[0].size(), intensity.size());
}

void test()
{
    get_ratios("coins.ppm", 4032, 3024);
}

int main(int argc, char** argv)
{
    part3(argc, argv);
}