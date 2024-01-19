// Project4Part1.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
                array[i][j] = 1;
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
            arrayPointer[row][col] = 0;
        }
        //else {
        //    //cout << "point drawing error" << endl;
        //}
    }

    void illuminateRed(int col, int row) {
        if (col < myWidth && row < myHeight && col >= 0 && row >= 0)
        {
            arrayPointer[row][col] = 2;
        }
    }

    void create_ppm() {
        ofstream myfile;
        myfile.open("grahamscan.ppm");
        myfile << "P3  " << myWidth << "  " << myHeight << "  1 \n";
        for (int i = getHeight() - 1; i >= 0; i--) {
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

class xDrivenPosSlope {
public:
    int x1;
    int x2;
    int y1;
    int y2;
    Canvas* myCanvas;
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
public:
    int x1;
    int x2;
    int y1;
    int y2;
    Canvas* myCanvas;
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

};

class yDrivenPosSlope {
public:
    int x1, y1, x2, y2;
    Canvas* myCanvas;
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
public:
    int x1;
    int x2;
    int y1;
    int y2;
    Canvas* myCanvas;
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
public:
    int myy;
    int myx1;
    int myx2;
    Canvas* myCanvas;
    Horizontal(int x1, int y1, int x2, int y2, Canvas* canvas) {
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
public:
    int x;
    int y1;
    int y2;
    Canvas* myCanvas;
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
    double x1;
    double y1;
    double x2;
    double y2;
    Canvas* myCanvas;
public:
    Line(double x1, double y1, double x2, double y2, Canvas* myCanvas) {
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
        this->myCanvas = myCanvas;
    }
    void drawLine() {
        double slope = (y2 - y1) / (x2 - x1);
        if (slope >= 1) {
            yDrivenPosSlope line(x1 * myCanvas->getHeight(), y1 * myCanvas->getHeight(), x2 * myCanvas->getHeight(), y2 * myCanvas->getHeight(), myCanvas);
            line.getPoints();
        }
        else if (slope > 0.01) {
            xDrivenPosSlope line(x1 * myCanvas->getHeight(), y1 * myCanvas->getHeight(), x2 * myCanvas->getHeight(), y2 * myCanvas->getHeight(), myCanvas);
            line.getPoints();
        }
        else if (slope <= -1) {
            yDrivenNegSlope line(x1 * myCanvas->getHeight(), y1 * myCanvas->getHeight(), x2 * myCanvas->getHeight(), y2 * myCanvas->getHeight(), myCanvas);
            line.getPoints();
        }
        else if (slope < -0.01) {
            xDrivenNegSlope line(x1 * myCanvas->getHeight(), y1 * myCanvas->getHeight(), x2 * myCanvas->getHeight(), y2 * myCanvas->getHeight(), myCanvas);
            line.getPoints();
        }
        else if (slope < 0.01 and slope > -0.01) {
            Horizontal line(x1 * myCanvas->getHeight(), y1 * myCanvas->getHeight(), x2 * myCanvas->getHeight(), y2 * myCanvas->getHeight(), myCanvas);
            line.getPoints();
        }
        else {
            //vertical
            Vertical line(x1 * myCanvas->getHeight(), y1 * myCanvas->getHeight(), x2 * myCanvas->getHeight(), y2 * myCanvas->getHeight(), myCanvas);
            line.getPoints();
        }
    }
};

//draws the rasterized circle
class Circle {
public:
    int centerX, centerY, radius;
    Canvas* myCanvas;
    Circle(int myX, int myY, int r, Canvas* canvas) {
        centerX = myX;
        centerY = myY;
        radius = r;
        myCanvas = canvas;
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

class Point {
private:
    double x;
    double y;
public:
    Point() {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;
    }
    Point(double myX, double myY) {
        x = myX;
        y = myY;
    }
    double getX() {
        return x;
    }
    void setX(double newX) {
        x = newX;
    }
    double getY() {
        return y;
    }
    void setY(double newY) {
        y = newY;
    }
    double distanceTo(Point* p) {
        return pow((x - p->getX()) * (x - p->getX()) + (y - p->getY()) * (y - p->getY()), 0.5);
    }
    bool equals(Point* p) {
        return x == p->getX() and y == p->getY();
    }
    //returns two points for E, one in each direction such that |AC| = |[this]E|
    Point* findE(Point* a, Point* c) {
        double dx = a->getX() - c->getX();
        double dy = a->getY() - c->getY();
        Point* possible_points = new Point[2]; //how to make array of points?
        possible_points[0].setX(getX() + dy);
        possible_points[0].setY(getY() - dx);
        possible_points[1].setX(getX() - dy);
        possible_points[1].setY(getY() + dx);
        return possible_points;
    }
    void display(Canvas* myCanvas) {
        Circle* c = new Circle(x * myCanvas->getWidth(), y * myCanvas->getHeight(), 3, myCanvas);
        c->getPoints();
        Circle* c2 = new Circle(x * myCanvas->getWidth(), y * myCanvas->getHeight(), 2, myCanvas);
        c2->getPoints();
    }
    void display_red(Canvas* myCanvas) {
        Circle* c = new Circle(x * myCanvas->getWidth(), y * myCanvas->getHeight(), 3, myCanvas);
        c->getPointsRed();
        Circle* c2 = new Circle(x * myCanvas->getWidth(), y * myCanvas->getHeight(), 2, myCanvas);
        c2->getPointsRed();
    }
    //right = true, left = false
    bool sideOfLine(Point* a, Point* b) {
        double m = (a->getY() - b->getY()) / (a->getX() - b->getX());
        double yint = a->getY() - m * a->getX();
        //if the actual x > expected x (therefore on the right side of the line)
        if (x > ((y - yint) / m)) {
            return true;
        }
        return false;
        //int side = ((y - a->getY()) * (b->getX() - a->getX())) - ((x - a->getX()) * (b->getY() - a->getY()));
        //if (side > 0) {
        //    return true;
        //}
        //return false;
    }

    //determines whether p->q->c is a right turn
    bool rightTurn(Point* p, Point* q) {
        double d = (q->getX() - p->getX()) * (y - p->getY()) - (q->getY() - p->getY()) * (x - p->getX());
        return d < 0;
    }

    bool AboveOrBelow(Point* a, Point* b) {
        double m = (a->getY() - b->getY()) / (a->getX() - b->getX());
        double yint = a->getY() - m * a->getX();
        //if the actual x > expected x (therefore on the right side of the line)
        if (y > m * x + yint) {
            return true;
        }
        return false;
    }

    double distanceToLine(Point* p, Point* q) {
        //ax + by + c = 0 -- line equation
        double a = (p->getY() - q->getY()) / (p->getX() - q->getX());
        double b = -1;
        double c = p->getY() - a * p->getX();
        return abs(a * x + b * y + c) / sqrt(a * a + b * b);
    }

    double getDistance(double xx1, double yy1, double xx2, double yy2) {
        return sqrt((xx1 - xx2) * (xx1 - xx2) + (yy1 - yy2) * (yy1 - yy2));
    }

    double getArea(double side1, double side2, double side3) {
        double s = (side1 + side2 + side3) / 2;
        return sqrt(s * (s - side1) * (s - side2) * (s - side3));
    }

    bool inTri(Point* point1, Point* point2, Point* point3) {
        double side1 = getDistance(point1->getX(), point1->getY(), point2->getX(), point2->getY());
        double side2 = getDistance(point1->getX(), point1->getY(), point3->getX(), point3->getY());
        double side3 = getDistance(point2->getX(), point2->getY(), point3->getX(), point3->getY());
        double inside1 = getDistance(point1->getX(), point1->getY(), x, y);
        double inside2 = getDistance(point2->getX(), point2->getY(), x, y);
        double inside3 = getDistance(point3->getX(), point3->getY(), x, y);

        //find the area of the triangle formed by the first three points
        double bigArea = getArea(side1, side2, side3);
        //find the area of the triangles formed with the inner(?) point
        double small1 = getArea(side1, inside1, inside2);
        double small2 = getArea(side2, inside1, inside3);
        double small3 = getArea(side3, inside2, inside3);

        return (abs(bigArea - (small1 + small2 + small3)) < 0.01);
    }
};

//returns the angle between the input three points, where p1 is the vertex
//works! (as far as I can tell)
double polar(Point* p1, Point* p3) {
    Point* p2 = new Point(p1->getX() + 1, p1->getY());
    double ax = p2->getX() - p1->getX();
    double ay = p2->getY() - p1->getY();
    double aMag = p1->distanceTo(p2);
    double bx = p3->getX() - p1->getX();
    double by = p3->getY() - p1->getY();
    double bMag = p1->distanceTo(p3);
    double dotProd = ax * bx + ay * by;
    double angle = acos(dotProd / (aMag * bMag));
    return angle;
}
bool comparePointsX(Point* p1, Point* p2) {
    return p1->getX() < p2->getX();
}
bool comparePointsY(Point* p1, Point* p2) {
    return p1->getY() < p2->getY();
}
//global point p0
Point* p0;
bool comparePointsPolar(Point* p1, Point* p2) {
    return polar(p0, p1) < polar(p0, p2);
}

stack<Point*> grahamScan(vector<Point*> v) {
    stack<Point*> s;

    //find the lowest y-coordinate and leftmost point, called p0
    sort(v.begin(), v.end(), &comparePointsY);
    p0 = v[0];
    for (int i = 0; i < v.size(); i++) {
        if (p0->getY() != v[i]->getY()) {
            break;
        }
        else {
            if (v[i]->getX() < p0->getX()) {
                p0 = v[i];
            }
        }
    }

    //remove p0 from vector
    vector<Point*>::iterator it;
    it = v.begin();
    v.erase(it);

    //sort remaining points by polar angle with p0
    sort(v.begin(), v.end(), &comparePointsPolar);

    //add p0 and p1 in the stack
    s.push(p0);
    s.push(v[0]);

    //for point in [p2, ...]
    for (int i = 1; i < v.size(); i++) {
        //B = pop from the stack
        Point* b = s.top();
        s.pop();
        //A = peek from the stack
        Point* a = s.top();
        //while count stack > 1 and NotALeftTurn(A, B, point)
        while (s.size() > 1 and v[i]->rightTurn(a, b)) {
            //B = pop from stack
            b = s.top();
            s.pop();
            //A = peek from stack
            a = s.top();
        }
        //push B on the stack
        s.push(b);
        //push point to stack
        s.push(v[i]);
    }
    s.push(p0);
    return s;
}

vector<Point*> findHull(vector<Point*> sk, Point* p, Point* q, vector<Point*> convexHull, Canvas* myCanvas) {
    if (sk.size() == 0) {
        return convexHull;
    }
    Point* c = sk[0];
    double maxd = sk[0]->distanceToLine(p, q);
    for (int i = 0; i < sk.size(); i++) {
        double d = sk[i]->distanceToLine(p, q);
        if (d > maxd) {
            c = sk[i];
            maxd = d;
        }
    }
    //inserting the point 
    int index = 0;
    for (int i = 0; i < convexHull.size(); i++) {
        if (convexHull[i]->equals(q)) {
            index = i;
        }
    }
    convexHull.insert(convexHull.begin() + index, c);
    vector<Point*> s1;
    vector<Point*> s2;
    //bool above = c->AboveOrBelow(p, q);
    for (int i = 0; i < sk.size(); i++) {
        if (sk[i]->equals(p) || sk[i]->equals(q) || sk[i]->equals(c)) {
            continue;
        }
        if (sk[i]->rightTurn(p, c)) {
            s1.push_back(sk[i]);
        }
        if (sk[i]->rightTurn(c, q)) {
            s2.push_back(sk[i]);
        }
    }
    convexHull = findHull(s1, p, c, convexHull, myCanvas);
    convexHull = findHull(s2, c, q, convexHull, myCanvas);
    return convexHull;

}

vector<Point*> quickHull(vector<Point*> v) {
    vector <Point*> convexHull;
    sort(v.begin(), v.end(), &comparePointsX);
    Point* a = v[0];
    Point* b = v[v.size() - 1];
    convexHull.push_back(a);
    convexHull.push_back(b);
    convexHull.push_back(a);
    vector<Point*> s1;
    vector<Point*> s2;
    int size = 800;
    Canvas* myCanvas = new Canvas(size, size);
    for (int i = 0; i < v.size(); i++) {
        if (v[i]->equals(a) || v[i]->equals(b)) {
            continue;
        }
        if (v[i]->rightTurn(a, b)) {
            s1.push_back(v[i]);
        }
        if (v[i]->rightTurn(b, a)) {
            s2.push_back(v[i]);
        }
    }
    convexHull = findHull(s1, a, b, convexHull, myCanvas);
    convexHull = findHull(s2, b, a, convexHull, myCanvas);
    myCanvas->create_ppm();
    return convexHull;
}

void part0() {
    int num_points = 60;
    string yn;
    cout << "Would you like to generate the points? (yes/no)" << endl;
    cin >> yn;
    transform(yn.begin(), yn.end(), yn.begin(), ::tolower);

    if (yn == "yes")
    {
        srand(time(NULL));
        ofstream points;
        points.open("points.txt");
        for (int i = 0; i < num_points; i++) {
            points << setprecision(20) << (double)rand() / RAND_MAX << "  " << (double)rand() / RAND_MAX << endl;
        }
        points.close();
    }
    else {
        cout << "You chose not to generate points" << endl;
    }
}

vector<Point*> read_points(string filename) {
    string myline = "";
    ifstream myfile;
    vector<Point*> points;

    myfile.open(filename);
    if (myfile.is_open()) {
        int space;
        double x, y;
        while (myfile.good()) {
            getline(myfile, myline); //assign line from file to myline
            if (myline.length() == 0) {
                break;
            }
            space = myline.find_first_of(' ');
            x = stod(myline.substr(0, space));
            myline = myline.substr(space + 2);
            y = stod(myline);
            points.push_back(new Point(x, y));
        }
    }
    else {
        std::cout << "Couldn't open file\n";
    }
    return points;
}

void part1() {
    int wh = 400;
    Canvas* myCanvas = new Canvas(wh, wh);
    vector<Point*> v;
    v = read_points("points.txt");
    for (int i = 0; i < v.size(); i++) {
        v[i]->display(myCanvas);
    }
    vector<Point*> convexHull = quickHull(v);
    for (int i = 0; i < convexHull.size(); i++) {
        Circle* c = new Circle(int(convexHull[i]->getX() * 400), int(convexHull[i]->getY() * 400), 3, myCanvas);
        c->getPoints();
        Line* l;
        if (i < convexHull.size() - 1) {
            l = new Line(convexHull[i]->getX(), convexHull[i]->getY(), convexHull[i + 1]->getX(), convexHull[i + 1]->getY(), myCanvas);
        }
        else {
            l = new Line(convexHull[i]->getX(), convexHull[i]->getY(), convexHull[0]->getX(), convexHull[0]->getY(), myCanvas);
        }
        l->drawLine();
    }
    myCanvas->create_ppm();
}

void part2() {
    int wh = 400;
    Canvas* myCanvas = new Canvas(wh, wh);

    vector<Point*> v;
    v = read_points("points.txt");
    for (int i = 0; i < v.size(); i++) {
        v[i]->display(myCanvas);
    }

    stack<Point*> convexHull = grahamScan(v);

    int chs = convexHull.size();
    for (int i = 0; i < chs; i++) {
        Point* current = convexHull.top();
        convexHull.pop();
        Point* next = new Point();
        if (convexHull.size() > 0) {
            next = convexHull.top();
        }
        //draw the point
        Circle* c = new Circle(int(current->getX() * 400), int(current->getY() * 400), 3, myCanvas);
        c->getPoints();
        Line* l;
        if (i < chs - 1) {
            l = new Line(current->getX(), current->getY(), next->getX(), next->getY(), myCanvas);
        }
        else {
            l = new Line(current->getX(), current->getY(), p0->getX(), p0->getY(), myCanvas);
        }
        l->drawLine();
    }
    myCanvas->create_ppm();
}

int main()
{
    part0();
    //part1();
    part2();
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
