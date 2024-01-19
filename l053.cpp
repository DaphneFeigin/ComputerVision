// l051.cpp : This file contains the 'main' function. Program execution begins and ends there.

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

vector<vector<int>> grayscale(string filename, string fg) {
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
    ppmwriter.open(fg);
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

//vector<vector<int>> sort_ambiguous(vector<vector<int>> combined, int width, int height, int row, int col) {
//    //base case: if we are out of bounds or at a 0 or at a spot that was already visited
//    if (row < 0 or row >= height or col < 0 or col >= width or combined[row][col] == 0 or combined[row][col] == 3) {
//        return combined;
//    }
//
//    combined[row][col] = 3;
//    sort_ambiguous(combined, width, height, row - 1, col - 1);
//    sort_ambiguous(combined, width, height, row - 1, col);
//    sort_ambiguous(combined, width, height, row - 1, col + 1);
//    sort_ambiguous(combined, width, height, row, col - 1);
//    sort_ambiguous(combined, width, height, row, col + 1);
//    sort_ambiguous(combined, width, height, row + 1, col - 1);
//    sort_ambiguous(combined, width, height, row + 1, col);
//    sort_ambiguous(combined, width, height, row + 1, col + 1);
//
//    combined[row][col] = 0;
//    return combined;
//}
bool isValid(vector<vector<int>> combined, int width, int height, int row, int col) {
    if (row < 0 or row >= height or col < 0 or col >= height or combined[row][col] == 0 or combined[row][col] == 3) {
        return false;
    }
    return true;
}

void change_ones(vector<vector<int>>& combined, int width, int height, int row, int col) {
    if (row < 0 or row >= height or col < 0 or col >= width) {
        return;
    }
    if (combined[row][col] == 0 or combined[row][col] == 3) {
        return;
    }
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

vector<vector<int>> get_angles(vector<vector<int>> gx, vector<vector<int>> gy, int width, int height) {
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

void nms(vector<vector<int>> edges, vector<vector<int>> angles, vector<vector<int>> combined, int width, int height, string f2, string ff) {
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
            if(edge == true)
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
}

//applies the single threshold and generates the new ppm
void single_threshold(vector<vector<int>> combined, int width, int height) {
    ofstream myfile;
    myfile.open("imagem.ppm");
    myfile << "P3  " << width << "  " << height << "  1 ";
    //220000 ideal for colosseum
    int threshold = 20000;
    for (int row = 0; row < height; row++) {
        myfile << "\n";
        for (int col = 0; col < width; col++) {
            if (combined[row][col] > threshold) {
                myfile << "1 1 1 ";
            }
            else {
                myfile << "0 0 0 ";
            }
        }
    }
    myfile.close();
}

vector<vector<int>> double_threshold(vector<vector<int>>& combined, int width, int height, int lower, int upper, string of) {
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
    loop_through_twos(combined, width, height);

    ofstream myfile;
    myfile.open(of);
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

void part1() {
    vector<vector<int>> intensity = grayscale("image1.ppm", "irrelevant.ppm");
    vector<vector<int>> gx = gaussian_x(intensity, intensity[0].size(), intensity.size());
    vector<vector<int>> gy = gaussian_y(intensity, intensity[0].size(), intensity.size());
    vector<vector<int>> combined = combine_gx_gy(gx, gy, intensity[0].size(), intensity.size());
    single_threshold(combined, intensity[0].size(), intensity.size());
}

void part2(int argc, char** argv) {
    string f = "";
    int lt = -1;
    int ht = -1;
    string of = "";
    string fg = "";
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            f = argv[i + 1];
        }
        else if (strcmp(argv[i], "-lt") == 0) {
            lt = stoi(argv[i + 1]) * stoi(argv[i + 1]);
            //cout << lt << endl;
        }
        else if (strcmp(argv[i], "-ht") == 0) {
            ht = stoi(argv[i + 1]) * stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-of") == 0) {
            of = argv[i + 1];
        }
        else if (strcmp(argv[i], "-fg") == 0) {
            fg = argv[i + 1];
        }
    }
    if (f == "")
        f = "image.ppm";
    if (lt == -1)
        lt = 15000;
    if (ht == -1)
        ht = 30000;
    if (of == "")
        of = "image1.ppm";
    if(fg == "")
        fg = "imageg.ppm";
    
    //cout << "variables processed" << endl;
    vector<vector<int>> intensity = grayscale(f, fg);
    //cout << "grayscale done" << endl;
    vector<vector<int>> gx = gaussian_x(intensity, intensity[0].size(), intensity.size());
    //cout << "gx done" << endl;
    vector<vector<int>> gy = gaussian_y(intensity, intensity[0].size(), intensity.size());
    //cout << "gx and gy done" << endl;
    vector<vector<int>> combined = combine_gx_gy(gx, gy, intensity[0].size(), intensity.size());
    //cout << "combining done" << endl;
    double_threshold(combined, intensity[0].size(), intensity.size(), lt, ht, of);
    //cout << "double threshold done" << endl;
}

void part3(int argc, char** argv) {
    string f = "";
    int lt = -1;
    int ht = -1;
    string fg = "";
    string f2 = "";
    string ff = "";
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            f = argv[i + 1];
        }
        else if (strcmp(argv[i], "-lt") == 0) {
            lt = stoi(argv[i + 1]) * stoi(argv[i + 1]);
            //cout << lt << endl;
        }
        else if (strcmp(argv[i], "-ht") == 0) {
            ht = stoi(argv[i + 1]) * stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-fg") == 0) {
            fg = argv[i + 1];
        }
        else if (strcmp(argv[i], "-f2") == 0) {
            f2 = argv[i + 1];
        }
        else if (strcmp(argv[i], "-ff") == 0) {
            ff = argv[i + 1];
        }
    }
    if (f == "")
        f = "image.ppm";
    if (lt == -1)
        lt = 15000;
    if (ht == -1)
        ht = 30000;
    if(fg == "")
        fg = "imageg.ppm";
    if (f2 == "")
        f2 = "image2.ppm";
    if(ff == "")
        ff = "imagef.ppm";
    
    vector<vector<int>> intensity = grayscale(f, fg);
    vector<vector<int>> gx = gaussian_x(intensity, intensity[0].size(), intensity.size());
    vector<vector<int>> gy = gaussian_y(intensity, intensity[0].size(), intensity.size());
    vector<vector<int>> combined = combine_gx_gy(gx, gy, intensity[0].size(), intensity.size());
    vector<vector<int>> combined_unchanged = combine_gx_gy(gx, gy, intensity[0].size(), intensity.size());
    vector<vector<int>> edges = double_threshold(combined, intensity[0].size(), intensity.size(), lt, ht, f2);
    vector<vector<int>> angles = get_angles(gx, gy, intensity[0].size(), intensity.size());
    nms(edges, angles, combined_unchanged, intensity[0].size(), intensity.size(), f2, ff);
}

int main(int argc, char** argv)
{
    //part2(argc, argv);
    part3(argc, argv);
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

