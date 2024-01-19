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

vector<vector<int>> grayscale(string filename) {
    cout << "in grayscale method" << endl;
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
            for (int i = 0; i < v.size(); i++) {
                cout << v[i] << endl;
            }
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
            //gaussian_x[row][col] = -1 * intensity[row - 1][col - 1] + 1 * intensity[row - 1][col + 1] + -2 * intensity[row][col - 1] + 2 * intensity[row][col + 1] + -1 * intensity[row + 1][col - 1] + 1 * intensity[row + 1][col + 1];
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

void part1() {
    vector<vector<int>> intensity = grayscale("image1.ppm");
    vector<vector<int>> gx = gaussian_x(intensity, intensity[0].size(), intensity.size());
    vector<vector<int>> gy = gaussian_y(intensity, intensity[0].size(), intensity.size());
    vector<vector<int>> combined = combine_gx_gy(gx, gy, intensity[0].size(), intensity.size());
    single_threshold(combined, intensity[0].size(), intensity.size());
}

int main()
{
    part1();
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

