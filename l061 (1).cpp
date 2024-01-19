// 6-1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include<vector> 
#include<algorithm>
#include <iomanip>
#include <ostream>
#include <string>
#include <math.h>
#include<list>
#include <thread>
#include<stdio.h>
#include<stack>
#include<cstring>

using namespace std;

class Canvas {
private:
    int myWidth;
    int myHeight;
    int** arrayPointer;
public:
    Canvas(int width, int height) {
        myWidth = width;
        myHeight = height;
        int** array = new int* [height];
        for (int i = 0; i < height; i++) array[i] = new int[width];
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                array[i][j] = 0;
            }
        }
        arrayPointer = array;
    }

    int getHeight() {
        return myHeight;
    }

    int getWidth() {
        return myWidth;
    }

    int** getArrayPointer() {
        return arrayPointer;
    }


    void illuminate(int col, int row) {
        if (col < myWidth && row < myHeight && col >= 0 && row >= 0)
        {
            //cout << "illuminating!" << endl;
            //arrayPointer[row][col] = 0;
            arrayPointer[row][col] += 1;
        }
        //else {
        //    //cout << "point drawing error" << endl;
        //}
    }

    void vote(int col, int row) {
        if (col < myWidth && row < myHeight && col >= 0 && row >= 0)
        {
            arrayPointer[row][col] += 1;
        }
    }

    int get(int row, int col) {
        return arrayPointer[row][col];
    }

    void illuminateRed(int col, int row) {
        if (col < myWidth && row < myHeight && col >= 0 && row >= 0)
        {
            arrayPointer[row][col] = 2;
        }
    }

    void create_ppm(string filename, int max) {
        ofstream myfile;
        myfile.open(filename);
        myfile << "P3  " << myWidth << "  " << myHeight << "  " << max << " \n";
        for (int i = 0; i < getHeight(); i++) {
            for (int j = 0; j < getWidth(); j++) {
                if (getArrayPointer()[i][j] == 2) {
                    myfile << 1 << " " << 0 << " " << 0 << " ";
                }
                else {
                    myfile << getArrayPointer()[i][j] << " " << getArrayPointer()[i][j] << " " << getArrayPointer()[i][j] << " ";
                }
            }
            myfile << "\n";
        }
        myfile.close();
    }
};

//draws the rasterized circle
class Circle {
public:
    int centerX, centerY, radius;
    double proportion = 0.0;
    Canvas* myCanvas;
    Circle(int myX, int myY, int r, Canvas* canvas) {
        centerX = myX;
        centerY = myY;
        radius = r;
        myCanvas = canvas;
    }
    Circle(int myRow, int myCol, int myR, double proportion) {
        centerY = myRow;
        centerX = myCol;
        radius = myR;
        proportion = p;
    }
    int getRow() {
        return centerY;
    }
    int getCol() {
        return centerX;
    }
    int getRadius() {
        return radius;
    }
    double getProportion() {
        return proportion;
    }
    void getPoints() {
        int x, y, xmax, y2, y2_new, ty;
        xmax = (int)(radius * 0.70710678); // maximum x at radius/sqrt(2)
        y = radius;
        y2 = y * y;
        ty = (2 * y) - 1;
        y2_new = y2;
        for (x = 0; x <= xmax; x++) {
            if ((y2 - y2_new) >= ty) {
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
    void getPointsRed() {
        int x, y, xmax, y2, y2_new, ty;
        xmax = (int)(radius * 0.70710678); // maximum x at radius/sqrt(2)
        y = radius;
        y2 = y * y;
        ty = (2 * y) - 1;
        y2_new = y2;
        for (x = 0; x <= xmax; x++) {
            if ((y2 - y2_new) >= ty) {
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
};

class xDrivenPosSlope {
private:
    int x1;
    int x2;
    int y1;
    int y2;
    Canvas* myCanvas;
public:
    xDrivenPosSlope(int myx1, int myy1, int myx2, int myy2, Canvas* canvas) {
        //if point 2 is up and right from point 1
        if (myx2 > myx1 && myy2 > myy1) {
            x1 = myx1;
            y1 = myy1;
            x2 = myx2;
            y2 = myy2;
        }
        else {
            x1 = myx2;
            y1 = myy2;
            x2 = myx1;
            y2 = myy1;
        }
        myCanvas = canvas;
    }
    void getPoints() {
        int deltaX = x2 - x1;
        int deltaY = y2 - y1;
        int j = y1;
        int error = deltaY - deltaX;

        for (int i = x1; i < x2; i++) {
            myCanvas->illuminate(i, j);
            if (error > 0) {
                j++;
                error -= deltaX;
            }
            error += deltaY;
        }
    }

};

class xDrivenNegSlope {
private:
    int x1;
    int x2;
    int y1;
    int y2;
    Canvas* myCanvas;
public:
    xDrivenNegSlope(int myx1, int myy1, int myx2, int myy2, Canvas* canvas) {
        //assumes y2 > y1 but x2 < x1
        if (myy2 > myy1 && myx2 < myx1)
        {
            x1 = myx1;
            y1 = myy1;
            y2 = myy2;
            x2 = (myx1 - myx2) + myx1;
        }
        else if (myy2 < myy1 && myx2 > myx1) {
            x1 = myx2;
            y1 = myy2;
            y2 = myy1;
            x2 = (myx2 - myx1) + myx2;
        }
        myCanvas = canvas;
    }
    void getPoints() {
        int deltaX = x2 - x1;
        int deltaY = y2 - y1;
        int j = y1;
        int error = deltaY - deltaX;

        for (int i = x1; i < x2; i++) {
            myCanvas->illuminate(x1 - (i - x1), j);
            //myCanvas->illuminate(i, j);
            if (error > 0) {
                j++;
                error -= deltaX;
            }
            error += deltaY;
        }
    }
    void vote() {
        int deltaX = x2 - x1;
        int deltaY = y2 - y1;
        int j = y1;
        int error = deltaY - deltaX;

        for (int i = x1; i < x2; i++) {
            myCanvas->illuminate(x1 - (i - x1), j);
            //myCanvas->illuminate(i, j);
            if (error > 0) {
                j++;
                error -= deltaX;
            }
            error += deltaY;
        }
    }

};

class yDrivenPosSlope {
private:
    int x1, y1, x2, y2;
    Canvas* myCanvas;
public:
    yDrivenPosSlope(int myx1, int myy1, int myx2, int myy2, Canvas* canvas) {
        if (myy2 > myy1)
        {
            x1 = myx1;
            y1 = myy1;
            x2 = myx2;
            y2 = myy2;
        }
        else if (myy2 < myy1) {
            x1 = myx2;
            y1 = myy2;
            x2 = myx1;
            y2 = myy1;
        }
        myCanvas = canvas;
    }
    void getPoints() {
        int deltaX = x2 - x1;
        int deltaY = y2 - y1;
        int i = x1;
        int error = deltaX - deltaY;

        for (int j = y1; j < y2; j++) {
            myCanvas->illuminate(i, j);
            if (error > 0) {
                i++;
                error -= deltaY;
            }
            error += deltaX;
        }
    }
};

class yDrivenNegSlope {
private:
    int x1;
    int x2;
    int y1;
    int y2;
    Canvas* myCanvas;
public:
    yDrivenNegSlope(int myx1, int myy1, int myx2, int myy2, Canvas* canvas) {
        //assumes y2 > y1 but x2 < x1
        if (myy2 > myy1 && myx2 < myx1)
        {
            x1 = myx1;
            y1 = myy1;
            y2 = myy2;
            x2 = (myx1 - myx2) + myx1;
        }
        else if (myy2 < myy1 && myx2 > myx1) {
            x1 = myx2;
            y1 = myy2;
            y2 = myy1;
            x2 = (myx2 - myx1) + myx2;
        }
        myCanvas = canvas;
    }
    void getPoints() {
        int deltaX = x2 - x1;
        int deltaY = y2 - y1;
        int i = x1;
        int error = deltaX - deltaY;

        for (int j = y1; j < y2; j++) {
            myCanvas->illuminate(x1 - (i - x1), j);
            if (error > 0) {
                i++;
                error -= deltaY;
            }
            error += deltaX;
        }
    }
};

class Horizontal {
private:
    int myy;
    int myx1;
    int myx2;
    Canvas* myCanvas;
public:
    Horizontal(int x1, int y1, int x2, int y2, Canvas* canvas) {
        //cout << "Horizontal Line" << endl;
        myy = y1;
        if (x2 > x1) {
            myx1 = x1;
            myx2 = x2;
        }
        else {
            myx1 = x2;
            myx2 = x1;
        }
        myCanvas = canvas;
    }
    void getPoints() {
        for (int i = myx1; i <= myx2; i++) {
            myCanvas->illuminate(i, myy);
        }
    }
};

class Vertical {
private:
    int x;
    int y1;
    int y2;
    Canvas* myCanvas;
public:
    Vertical(int myx1, int myy1, int myx2, int myy2, Canvas* canvas) {
        x = myx1;
        if (myy2 > myy1) {
            y1 = myy1;
            y2 = myy2;
        }
        else {
            y1 = myy2;
            y2 = myy1;
        }
        myCanvas = canvas;
    }
    void getPoints() {
        for (int j = y1; j <= y2; j++) {
            myCanvas->illuminate(x, j);
        }
    }
};

class Line {
private:
    int x1;
    int y1;
    int x2;
    int y2;
    Canvas* myCanvas;
public:
    Line(double x1, double y1, double x2, double y2, Canvas* myCanvas) {
        this->x1 = x1 * myCanvas->getWidth();
        this->y1 = y1 * myCanvas->getHeight();
        this->x2 = x2 * myCanvas->getWidth();
        this->y2 = y2 * myCanvas->getHeight();
        this->myCanvas = myCanvas;
    }
    //may need to come back and modify for vertical edge case
    Line(int x, int y, double angle, Canvas* myCanvas) {
        //given: the actual integer point on the Canvas, the angle
        //need: the intersection points
        double slope = tan(angle);
        double k = 150 / sqrt(1 + slope * slope);
        int xa = x + k;
        int xb = x - k;
        int ya = y + k * slope;
        int yb = y - k * slope;

        double intercept = y - slope * x;
        //cout << "y-intercept: " << intercept << endl;
        x1 = xa;
        x2 = xb;
        y1 = ya;
        y2 = yb;
        this->myCanvas = myCanvas;
        //         int b = round(y - slope * x);
        //         x1 = x - 200;
        //         y1 = round(slope * x1 + intercept);
        //         x2 = x + 200;
        //         y2 = round(slope * x2 + intercept);
        //         this->myCanvas = myCanvas;
    }
    void drawLine() {
        //cout << "in the draw line method" << endl;
        //cout << "Line's given values:  x1: " << x1 << " y1: "<< y1 << " x2: "<< x2 << " y2: " << y2 <<  endl;
        double dy = y2 - y1;
        double dx = x2 - x1;
        double slope = dy / dx;
        //cout << "slope again (in drawLine method) = " << slope << endl;
        if (slope >= 1) {
            yDrivenPosSlope line(x1, y1, x2, y2, myCanvas);
            line.getPoints();
        }
        else if (slope > 0) {
            xDrivenPosSlope line(x1, y1, x2, y2, myCanvas);
            line.getPoints();
        }
        else if (slope <= -1) {
            yDrivenNegSlope line(x1, y1, x2, y2, myCanvas);
            line.getPoints();
        }
        else if (slope < 0) {
            xDrivenNegSlope line(x1, y1, x2, y2, myCanvas);
            line.getPoints();
        }
        else if (slope == 0) {
            Horizontal line(x1, y1, x2, y2, myCanvas);
            line.getPoints();
        }
        else {
            //vertical
            Vertical line(x1, y1, x2, y2, myCanvas);
            line.getPoints();
        }
    }
};

//Canvas + lines (already written classes) **********************************************************************************************************************

vector<vector<int>> create_empty(int width, int height) {
    vector<vector<int>> empty;
    for (int row = 0; row < height; row++) {
        vector<int> v;
        empty.push_back(v);
        for (int col = 0; col < width; col++) {
            empty[row].push_back(0);
        }
    }
    //cout << "empty width: " << width << " empty height: " << height << endl;
    return empty;
}

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
        else {
            s = s + x;
        }
    }
    v.push_back(s);
    return v;
}

vector<vector<int>> grayscale(string filename) {
    //cout << "in grayscale method" << endl;
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

    if (myfile.is_open()) {
        while (myfile.good()) {
            getline(myfile, myline); //assign line from file to myline
            if (myline.length() == 0) {
                break;
            }
            vector<string> v = split(myline);
            //for (int i = 0; i < v.size(); i++) {
            //    cout << v[i] << endl;
            //}
            if (lineat <= 1 && (v.size() == 2 || v.size() == 4)) {
                if (v.size() == 2) {
                    width = stod(v[0]);
                    height = stod(v[1]);
                }
                else {
                    width = stod(v[1]);
                    height = stod(v[2]);
                }
                grayscale = create_empty(width, height);
            }
            if (lineat >= 3) {
                for (int i = 0; i < v.size() - 1; i++) {
                    //cout << i << ": adding " << v[i] << "to list of numbers" << endl;
                    numbers.push_back(stod(v[i]));
                }
            }
            lineat++;
        }
    }
    //NEED ACCESS TO NUMBERS

    //cout << "finished adding numbers to the list" << endl;

    for (int i = 0; i < numbers.size(); i += 3) {
        int gray = numbers[i] + numbers[i + 1] + numbers[i + 2];
        gray = gray / 3;
        grayscale[row][col] = gray;
        col++;
        if (col == width) {
            //cout << "at final column: " << col << " and row: " << row << endl;
            row++;
            col = 0;
            //cout << "moving to row " << row << endl;
        }
    }

    //generate the ppm
    ofstream ppmwriter;
    ppmwriter.open("imageg.ppm");
    ppmwriter << "P3  " << width << "  " << height << "  255 \n";
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            ppmwriter << grayscale[row][col] << " " << grayscale[row][col] << " " << grayscale[row][col] << " ";
        }
        ppmwriter << "\n";
    }
    ppmwriter.close();

    return grayscale;
}

vector<vector<int>> gaussian_x(vector<vector<int>> intensity, int width, int height) {
    vector<vector<int>> gaussian_x = create_empty(width, height);
    for (int row = 1; row < height - 1; row++) {
        for (int col = 1; col < width - 1; col++) {
            gaussian_x[row][col] = 1 * intensity[row - 1][col - 1] - 1 * intensity[row - 1][col + 1] + 2 * intensity[row][col - 1] - 2 * intensity[row][col + 1] + 1 * intensity[row + 1][col - 1] - 1 * intensity[row + 1][col + 1];
        }
    }
    return gaussian_x;
}

vector<vector<int>> gaussian_y(vector<vector<int>> intensity, int width, int height) {
    vector<vector<int>> gaussian_y = create_empty(width, height);
    for (int row = 1; row < height - 1; row++) {
        for (int col = 1; col < width - 1; col++) {
            //gaussian_y[row][col] = -1 * intensity[row - 1][col - 1] + 1 * intensity[row - 1][col + 1] + 1 * intensity[row + 1][col - 1] + 2 * intensity[row + 1][col] + 1 * intensity[row + 1][col + 1];
            gaussian_y[row][col] = 1 * intensity[row - 1][col - 1] - 1 * intensity[row + 1][col - 1] + 2 * intensity[row - 1][col] - 2 * intensity[row + 1][col] + 1 * intensity[row - 1][col + 1] - 1 * intensity[row + 1][col + 1];
        }
    }
    return gaussian_y;
}

vector<vector<int>> combine_gx_gy(vector<vector<int>> gx, vector<vector<int>> gy, int width, int height) {
    vector<vector<int>> combined = create_empty(width, height);
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            combined[row][col] = gx[row][col] * gx[row][col] + gy[row][col] * gy[row][col];
        }
    }
    return combined;
}

bool isValid(vector<vector<int>> combined, int width, int height, int row, int col) {
    if (row < 0 or row >= height or col < 0 or col >= height or combined[row][col] == 0 or combined[row][col] == 3) {
        return false;
    }
    return true;
}

void change_ones(vector<vector<int>>& combined, int width, int height, int row, int col) {
    //cout << "row: " << row << " col: " << col << endl;
    if (row < 0 or row >= height or col < 0 or col >= width) {
        return;
    }
    if (combined[row][col] == 0 or combined[row][col] == 3) {
        return;
    }
    //cout << "row: " << row << " col: " << col << endl;
    combined[row][col] = 3;
    change_ones(combined, width, height, row - 1, col - 1); //top left
    change_ones(combined, width, height, row - 1, col); //above
    change_ones(combined, width, height, row - 1, col + 1); //top right
    change_ones(combined, width, height, row, col - 1); // left
    change_ones(combined, width, height, row, col + 1); // right
    change_ones(combined, width, height, row + 1, col - 1); //bottom left
    change_ones(combined, width, height, row + 1, col); //below
    change_ones(combined, width, height, row + 1, col + 1); //bottom right
}

//goes through and finds all twos where recursive call must be made
void loop_through_twos(vector<vector<int>>& combined, int width, int height) {
    cout << combined[0].size() << " " << combined.size() << endl;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (combined[row][col] == 2) {
                change_ones(combined, width, height, row, col);
            }
        }
    }

    //changes each remaining 1 to 0
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (combined[row][col] == 1)
                combined[row][col] = 0;
        }
    }

}

vector<vector<double>> get_angles(vector<vector<int>> gx, vector<vector<int>> gy, int width, int height) {
    //create empty vector of correct size with doubles
    vector<vector<double>> angles;
    for (int row = 0; row < height; row++) {
        vector<double> v;
        angles.push_back(v);
        for (int col = 0; col < width; col++) {
            angles[row].push_back(0);
        }
    }

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            angles[row][col] = atan2(gy[row][col], gx[row][col]);
            //radians to degrees: * 180/pi
            //angles[row][col] = angles[row][col] * (180.0 / 2 * acos(0.0));
        }
    }
    return angles;
}

vector<vector<int>> double_threshold(vector<vector<int>>& combined, int width, int height, int lower, int upper, string f1) {
    cout << "here" << endl;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (combined[row][col] < lower)
                combined[row][col] = 0;
            else if (combined[row][col] > upper)
                combined[row][col] = 2;
            else
                combined[row][col] = 1;
        }
    }
    cout << "done with first for loop" << endl;
    loop_through_twos(combined, width, height);
    cout << "done looping through twos" << endl;

    ofstream myfile;
    myfile.open(f1);
    myfile << "P3  " << width << "  " << height << "  1 ";
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (combined[row][col] == 0) {
                myfile << "0 0 0 ";
            }
            else {
                myfile << "1 1 1 ";
            }
        }
    }
    return combined;
}

vector<vector<int>> angles_nms(vector<vector<int>> gx, vector<vector<int>> gy, int width, int height) {
    vector<vector<int>> angles = create_empty(width, height);
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            double tan = atan2(gy[row][col], gx[row][col]);
            //radians to degrees: * 180/pi
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
            //cout << degrees << " rounded to: " << rounded << endl;
            angles[row][col] = rounded;
        }
    }
    return angles;
}

vector<vector<int>> nms(vector<vector<int>> edges, vector<vector<int>> angles, vector<vector<int>> combined, int width, int height, string f2, string ff) {
    vector<vector<int>> image2 = create_empty(width, height);
    vector<vector<int>> final = create_empty(width, height);
    for (int row = 1; row < height - 1; row++) {
        for (int col = 1; col < width - 1; col++) {
            bool edge = false;
            if (angles[row][col] == 0 or angles[row][col] == 180 or angles[row][col] == -180) {
                if (combined[row][col] >= combined[row][col - 1] and combined[row][col] >= combined[row][col + 1]) {
                    edge = true;
                }
            }
            else if (angles[row][col] == 45 or angles[row][col] == -135) {
                if (combined[row][col] >= combined[row - 1][col + 1] and combined[row][col] >= combined[row + 1][col - 1]) {
                    edge = true;
                }
            }
            else if (angles[row][col] == 90 or angles[row][col] == -90) {
                if (combined[row][col] >= combined[row - 1][col] and combined[row][col] >= combined[row + 1][col]) {
                    edge = true;
                }
            }
            else if (angles[row][col] == 135 or angles[row][col] == -45) {
                if (combined[row][col] >= combined[row + 1][col + 1] and combined[row][col] >= combined[row - 1][col - 1]) {
                    edge = true;
                }
            }
            //cout << edge << endl;
            if (edge == true)
                image2[row][col] = 1;
            else
                image2[row][col] = 0;
            if (edge == true and edges[row][col] != 0) {
                final[row][col] = 1;
            }
            else {
                final[row][col] = 0;
            }
        }
    }

    ofstream myfile;
    //image2.ppm
    myfile.open(f2);
    myfile << "P3  " << width << "  " << height << "  1 ";
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (image2[row][col] == 0) {
                myfile << "0 0 0 ";
            }
            else {
                myfile << "1 1 1 ";
            }
        }
    }

    ofstream myfile2;
    //imagef.ppm
    myfile2.open(ff);
    myfile2 << "P3  " << width << "  " << height << "  1 ";
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (final[row][col] == 0) {
                myfile2 << "0 0 0 ";
            }
            else {
                myfile2 << "1 1 1 ";
            }
        }
    }

    return final;
}


vector<vector<int>> voting(vector<vector<int>> edges, vector<vector<double>> angles, int width, int height, int tc, string f, string fv, string fcc) {
    //create a voting canvas
    Canvas* votesCanvas = new Canvas(width, height);
    int max = 0;
    int threshold = tc;
    vector<vector<int>> votes = create_empty(width, height);
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (edges[row][col] != 0) {
                Line* l = new Line(col, row, angles[row][col], votesCanvas);
                l->drawLine();
                if (votesCanvas->get(row, col) > max)
                    max = votesCanvas->get(row, col);
            }
        }
    }
    votesCanvas->create_ppm(fv, max);

    Canvas* circles = new Canvas(width, height);
    vector<vector<int>> centers = create_empty(width, height);
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            //added this
            if (votesCanvas->get(row, col) >= threshold){
                centers[row][col] = 1;
                Circle* c1 = new Circle(col, row, 1, circles);
                Circle* c2 = new Circle(col, row, 2, circles);
                Circle* c3 = new Circle(col, row, 3, circles);
                Circle* c4 = new Circle(col, row, 4, circles);
                c1->getPoints();
                c2->getPoints();
                c3->getPoints();
                c4->getPoints();
            }
        }
    }

    ifstream myfile;
    myfile.open(f);
    vector<int> numbers;
    int lineat = 0;
    string myline = "";
    if (myfile.is_open()) {
        while (myfile.good()) {
            getline(myfile, myline); //assign line from file to myline
            if (myline.length() == 0) {
                break;
            }
            vector<string> v = split(myline);
            if (lineat >= 3) {
                for (int i = 0; i < v.size() - 1; i++) {
                    //cout << i << ": adding " << v[i] << "to list of numbers" << endl;
                    numbers.push_back(stod(v[i]));
                }
            }
            lineat++;
        }
    }

    ofstream myfile2;
    myfile2.open(fcc);
    myfile2 << "P3  " << width << "  " << height << "  255 ";
    int i = 0;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (circles->get(row, col) != 0) {
                myfile2 << "255 0 0 ";
            }
            else {
                myfile2 << numbers[i] << " " << numbers[i + 1] << " " << numbers[i + 2] << " ";
            }
            i += 3;
        }
    }

    return centers;
}

void find_coins(vector<vector<int>> edges, vector<vector<int>> centers, int width, int height) {
    int minR = 85;
    int maxR = 130;
    
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            for (int i = minR; i <= maxR; i++) {
                Circle* c = new Circle(row, col, i);

            }
        }
    }
}

void part1(int argc, char** argv) {
    string f = "coins.ppm";
    int lt = 110*110;
    int ht = 170*170;
    string fg = "gray.ppm";
    string f1 = "image1.ppm";
    string f2 = "image2.ppm"; 
    string ff = "imagef.ppm";
    int tc = 33;
    string fv = "imagev.ppm";
    string fcc = "imageCC.ppm";
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            f = argv[i + 1];
        }
        else if (strcmp(argv[i], "-lt") == 0) {
            lt = stoi(argv[i + 1]) * stoi(argv[i + 1]);
            cout << lt << endl;
        }
        else if (strcmp(argv[i], "-ht") == 0) {
            ht = stoi(argv[i + 1]) * stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-fg") == 0) {
            fg = argv[i + 1];
        }
        else if (strcmp(argv[i], "-f1") == 0) {
            f1 = argv[i + 1];
        }
        else if (strcmp(argv[i], "-f2") == 0) {
            f2 = argv[i + 1];
        }
        else if (strcmp(argv[i], "-ff") == 0) {
            ff = argv[i + 1];
        }
        else if (strcmp(argv[i], "-TC") == 0) {
            tc = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-fv") == 0) {
            fv = argv[i + 1];
        }
        else if (strcmp(argv[i], "-fcc") == 0) {
            fcc = argv[i + 1];
        }
    }

    cout << "variables processed" << endl;
    vector<vector<int>> intensity = grayscale(f);
    cout << "grayscale done" << endl;
    vector<vector<int>> gx = gaussian_x(intensity, intensity[0].size(), intensity.size());
    cout << "gx done" << endl;
    vector<vector<int>> gy = gaussian_y(intensity, intensity[0].size(), intensity.size());
    cout << "gx and gy done" << endl;
    vector<vector<int>> combined = combine_gx_gy(gx, gy, intensity[0].size(), intensity.size());
    cout << "combining done" << endl;
    vector<vector<int>> edges = double_threshold(combined, intensity[0].size(), intensity.size(), lt, ht, f1);
    cout << "double threshold done" << endl;
    vector<vector<int>> a_nms = angles_nms(gx, gy, intensity[0].size(), intensity.size());
    edges = nms(edges, a_nms, combined, intensity[0].size(), intensity.size(), f2, ff);
    vector<vector<double>> angles = get_angles(gx, gy, intensity[0].size(), intensity.size());
    voting(edges, angles, intensity[0].size(), intensity.size(), tc, f, fv, fcc);
}

void part2(int argc, char** argv) {

}

int main(int argc, char** argv)
{
    part1(argc, argv);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
