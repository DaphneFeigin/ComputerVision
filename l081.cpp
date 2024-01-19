#include <stdio.h>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

using namespace std;
using namespace cv;

// Mat scale(Mat p, double scaleBy){
//     double dA[3][3] = {
//         {scaleBy,   0,   0},
//         {0,     scaleBy, 0},
//         {0,         0,   1}
//     }; 
//     Mat A(3,3, CV_64FC1, dA );
//     Mat newP = A * p;
//     return newP;
// }

// Mat translate(Mat p, double tx, double ty){
//     double dA[3][3] = {
//         {1,   0,   tx},
//         {0,   1,   ty},
//         {0,   0,   1}
//     }; 
//     Mat A(3,3, CV_64FC1, dA );
//     Mat newP = A * p;
//     return newP;
// }

Mat transform(Mat p, double tx, double ty, double scaleBy, double angle){
    double translate_scale[4][4] = {{scaleBy, 0, 0, tx}, {0, scaleBy, 0, ty}, {0, 0, scaleBy, 0}, {0, 0, 0, 1}};
    Mat ts(4, 4, CV_64FC1, translate_scale);
    double rotate_x[4][4] = {{1, 0, 0, 0}, {0, cos(angle), -1*sin(angle), 0}, {0, sin(angle), cos(angle), 0}, {0, 0, 0, 1}};
    double rotate_z[4][4] = {{cos(angle), -1*sin(angle), 0, 0}, {sin(angle), cos(angle), 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    Mat rot_x(4, 4, CV_64FC1, rotate_x);
    Mat rot_z(4, 4, CV_64FC1, rotate_z);
    Mat final_trans = ts * rot_x * rot_z;
    return final_trans * p;
}

//rotates around the z-axis
Mat rotate(Mat p, double angle){
    double dA[3][3] = {
        {cos(angle),   -1*sin(angle),   0},
        {sin(angle),   cos(angle),      0},
        {0,   0,   1}
    }; 
    Mat A(3,3, CV_64FC1, dA );
    Mat newP = A * p; 
    return newP;
}

void MyFilledCircle( Mat img, Point center )
{
  circle( img,
      center,
      4,
      Scalar( 0, 0, 255 ),
      FILLED,
      LINE_8 );
}

void MyLine( Mat img, Point start, Point end )
{
  int thickness = 2;
  int lineType = LINE_8;
  line( img,
    start,
    end,
    Scalar( 0, 0, 255 ),
    thickness,
    lineType );
}

//turn a 3D point (a Mat) and return its 2D orthographic projection (a Point)
Point ortho(Mat p){
    return Point(p.at<double>(0, 0), p.at<double>(1, 0));
}

string toString(Mat p){
    string x = to_string(p.at<double>(0,0));
    string y = to_string(p.at<double>(1,0));
    string z = to_string(p.at<double>(2,0));
    string s = "(" + x; 
    s += ", ";
    s += y;
    s += ", ";
    s += z;
    s += ")";
    return s;
}

Mat render_cube(vector<Mat> vertices){
    vector<Point> points;
    for(int i = 0; i < vertices.size(); i++){
        points.push_back(ortho(vertices[i]));
    }
    
    Mat image = Mat::zeros( 600, 800, CV_8UC3 );
    
    for(int i = 0; i < points.size(); i++){
        MyFilledCircle(image, points[i]);
    }
    
    MyLine(image, points[0], points[1]);
    MyLine(image, points[0], points[2]);
    MyLine(image, points[0], points[4]);
    MyLine(image, points[1], points[3]);
    MyLine(image, points[1], points[5]);
    MyLine(image, points[2], points[3]);
    MyLine(image, points[2], points[6]);
    MyLine(image, points[3], points[7]);
    MyLine(image, points[4], points[5]);
    MyLine(image, points[4], points[6]);
    MyLine(image, points[5], points[7]);
    MyLine(image, points[6], points[7]);
    
    return image;
}

Mat render_tetra(vector<Mat> vertices){
    vector<Point> points;
    for(int i = 0; i < vertices.size(); i++){
        points.push_back(ortho(vertices[i]));
    }
    
    Mat image = Mat::zeros( 600, 800, CV_8UC3 );
    
    for(int i = 0; i < points.size(); i++){
        MyFilledCircle(image, points[i]);
    }
    
    MyLine(image, points[0], points[1]);
    MyLine(image, points[1], points[2]);
    MyLine(image, points[2], points[3]);
    MyLine(image, points[3], points[0]);
    MyLine(image, points[1], points[3]);
    MyLine(image, points[0], points[2]);
    
    return image;
}

//return the frames for the video
vector<Mat> make_video(vector<Mat> vertices, int cube){
    vector<Mat> frames;
    ofstream myfile;
    if(cube == 1){
        myfile.open("coordinates.txt");
    }
    
    int angle = 0;
    for(int angle = 0; angle <= 360; angle += 5){
        vector<Mat> current;
        for(int i = 0; i < vertices.size(); i++){
            current.push_back(transform(vertices[i], 400, 300, 100, angle/57.2957795));
        }
      
        if(cube == 1){
            frames.push_back(render_cube(current));
            if(angle < 20){
                for(int j = 0; j < current.size(); j++){
                    myfile << toString(current[j]);
                    if(j < current.size() - 1)
                        myfile << ", ";
                    else
                        myfile << "\n";
                }    
            }
        }
        else{
            frames.push_back(render_tetra(current));
        }
    }

    myfile.close();
    return frames;
}

void part1(){
    double v1[1][4] = {{1, 1, 1, 1}};
    double v2[1][4] = {{1, 1, -1, 1}};
    double v3[1][4] = {{1, -1, 1, 1}};
    double v4[1][4] = {{1, -1, -1, 1}};
    double v5[1][4] = {{-1, 1, 1, 1}};
    double v6[1][4] = {{-1, 1, -1, 1}};
    double v7[1][4] = {{-1, -1, 1, 1}};
    double v8[1][4] = {{-1, -1, -1, 1}};
    vector<Mat> vertices = {Mat(4,1, CV_64FC1, v1), Mat(4,1, CV_64FC1, v2), Mat(4,1, CV_64FC1, v3), Mat(4,1, CV_64FC1, v4), Mat(4,1, CV_64FC1, v5),  
                            Mat(4,1, CV_64FC1, v6) , Mat(4,1, CV_64FC1, v7), Mat(4,1, CV_64FC1, v8)};
    vector<Mat> cubeframes = make_video(vertices, 1);
    
    double tv1[1][4] = {{1.0/sqrt(3), 0, 0, 1}};
    double tv2[1][4] = {{0, 0, 2.0/sqrt(6), 1}};
    double tv3[1][4] = {{-1*sqrt(3)/6.0, 1.0/2, 0 ,1}};
    double tv4[1][4] = {{-1*sqrt(3)/6.0, -1*1.0/2, 0 ,1}};
    vector<Mat> vertices_t = {Mat(4,1, CV_64FC1, tv1), Mat(4,1, CV_64FC1, tv2), Mat(4,1, CV_64FC1, tv3), Mat(4,1, CV_64FC1, tv4)};
    vector<Mat> tetraframes = make_video(vertices_t, 0);
    
    int codec = VideoWriter::fourcc('M','J','P','G'); 
    Size size = cubeframes[0].size(); 
    VideoWriter video("rotation.avi", codec, 10, size);
    for(int i = 0; i < cubeframes.size(); i++){
        video.write(cubeframes[i]);
    }
    for(int i = 0; i < tetraframes.size(); i++){
        video.write(tetraframes[i]);
    }
}

int main(int argc, char** argv ){
    part1();  
}

    