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

Mat transform(Mat p, double tx, double ty, double scaleBy, double angle){
    double translate_scale[4][4] = {{scaleBy, 0, 0, 0}, {0, scaleBy, 0, 0}, {0, 0, scaleBy, 0}, {0, 0, 0, 1}};
    Mat ts(4, 4, CV_64FC1, translate_scale);
    double rotate_x[4][4] = {{1, 0, 0, 0}, {0, cos(angle), -1*sin(angle), 0}, {0, sin(angle), cos(angle), 0}, {0, 0, 0, 1}};
    double rotate_y[4][4] = {{cos(angle), 0, sin(angle), 0}, {0, 1, 0, 0}, {-1*sin(angle), 0, cos(angle), 0}, {0, 0, 0, 1}};
    double rotate_z[4][4] = {{cos(angle), -1*sin(angle), 0, 0}, {sin(angle), cos(angle), 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    Mat rot_x(4, 4, CV_64FC1, rotate_x);
    Mat rot_y(4, 4, CV_64FC1, rotate_y);
    Mat rot_z(4, 4, CV_64FC1, rotate_z);
    //Mat final_trans = ts * rot_x * rot_y * rot_z;
    Mat final_trans = ts * rot_z;
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
      6,
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

Point perspec(Mat p){
    //placing the eye at (0, 0, 5)
    double d = 700.0;
    double project[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, -1.0/d, 1}};
    Mat proj(4, 4, CV_64FC1, project);
    Mat newP = proj * p;
    return Point(newP.at<double>(0, 0)/newP.at<double>(3, 0), newP.at<double>(1, 0)/newP.at<double>(3, 0));
}

Point perspec2(Mat p){
    double fov = 90;
    double s = 1/tan((fov/2)*(3.141592/180));
    double f = 50;
    double n = 0.1;
    double project[4][4] = {{s, 0, 0, 0}, {0, s, 0, 0}, {0, 0, -1*f/(f-n), -2*(f*n)/(f-n)}, {0, 0, -1, 0}};
    Mat proj(4, 4, CV_64FC1, project);
    Mat newP = proj * p;
    return Point(newP.at<double>(0, 0)/newP.at<double>(3, 0), newP.at<double>(1, 0)/newP.at<double>(3, 0));
}

Point perspec3(Mat p){
    double d = 700.0;
    double project[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 1/d, 0}};
    Mat proj(4, 4, CV_64FC1, project);
    Mat newP = proj * p;
    return Point(newP.at<double>(0, 0)/newP.at<double>(3, 0), newP.at<double>(1, 0)/newP.at<double>(3, 0));
}

double dot(Mat a, Mat b){
    return a.at<double>(0,0) * b.at<double>(0,0) + a.at<double>(1,0) * b.at<double>(1,0) + a.at<double>(2,0) * b.at<double>(2,0);
}

Mat normalize(Mat m){
    double mag = sqrt(m.at<double>(0,0)*m.at<double>(0,0) + m.at<double>(1,0)*m.at<double>(1,0) + m.at<double>(2,0)*m.at<double>(2,0));
    return (1/mag)*m;
}

Mat subtract(Mat a, Mat b){
    return (Mat_<double>(3, 1) << a.at<double>(0,0) - b.at<double>(0,0), a.at<double>(1,0) - b.at<double>(1,0), a.at<double>(2,0) - b.at<double>(2,0));
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

//v is the point, a and n define the plane, and e is the eye
Mat project(Mat v, Mat a, Mat n, Mat e){
    Mat a_minus_e;
    Mat v_minus_e;
    a_minus_e = subtract(a, e);
    v_minus_e = subtract(v, e);
    double num = dot(a_minus_e, n);
    double denom = dot(v_minus_e, n);
    double scalar = num/denom;
    Mat toadd = scalar * v_minus_e;
    Mat projected;
    add(toadd, e, projected);
    return projected;
}

vector<Mat> getW1andW2(Mat a, Mat n, Mat e, string filename){
    ofstream log;
    log.open(filename, std::ios_base::app);
    log << "Vertices I used to create the 2d coordinate system and their projections are:" << endl;
    vector<Mat> w;
    Mat v1 = (Mat_<double>(3, 1) << 100, 100, 100);
    Mat v2 = (Mat_<double>(3,1) << 100, 100, -100);
    Mat v3 = (Mat_<double>(3,1) << 100, -100, 100);
    log << "   v1 = " << toString(v1) << endl;
    log << "   v2 = " << toString(v2) << endl;
    log << "   v3 = " << toString(v3) << endl << endl;
    Mat p1 = project(v1, a, n, e);
    Mat p2 = project(v2, a, n, e);
    Mat p3 = project(v3, a, n, e);
    log << "   pv1 = " << toString(p1) << endl;
    log << "   pv2 = " << toString(p2) << endl;
    log << "   pv3 = " << toString(p3) << endl << endl;
    Mat p12;
    Mat p31;
    Mat w2;
    p12 = subtract(p1, p2);
    w.push_back(normalize(p12));
    p31 = subtract(p3, p1);
    log << "The two vectors a and b that are in the plane are: " << endl;
    log << "   a = pv1-pv2 = " << toString(p12) << endl;
    log << "   b = pv3-pv1 = " << toString(p31) << endl << endl;
    w2 = subtract(p31, (dot(p31, p12)/dot(p12, p12))*p12);
    w.push_back(normalize(w2));
    log << "The w1 and w2 obtained from a and b (these now are perpendicular and of magnitude 1) are: " << endl;
    log << "   w1 = " << toString(w[0]) << endl;
    log << "   w2 = " << toString(w[1]) << endl << endl;
    log.close();
    return w;
}

Mat getPx0(Mat a, Mat n, Mat e, string filename){
    ofstream log;
    log.open(filename, std::ios_base::app);
    Mat center = (Mat_<double>(3,1) << 0, 0, 0);
    Mat projected = project(center, a, n, e);
    log << "The center of the cube in the first frame and its projection are: " << endl;
    log << "   center = " << toString(center) << endl;
    log << "   p0 = " << toString(projected) << endl << endl;
    log.close();
    return projected;
}

Point getUV(Mat x, Mat w1, Mat w2, Mat px0){
    //cout << "trying to find (projected point): " << x.at<double>(0, 0) << ", " << x.at<double>(1, 0) << ", " << x.at<double>(2, 0) << endl;
    //double l[2][2] = {{w1.at<double>(2, 0), w1.at<double>(1, 0)}, {w2.at<double>(2, 0), w2.at<double>(1, 0)}};
    double l[2][2] = {{w1.at<double>(2, 0), w2.at<double>(2, 0)}, {w1.at<double>(1, 0), w2.at<double>(1, 0)}};
    Mat left(2, 2, CV_64FC1, l);
    double r[1][2] = {{x.at<double>(2, 0) - px0.at<double>(2,0), x.at<double>(1, 0) - px0.at<double>(1, 0)}};
    Mat right(2, 1, CV_64FC1, r);
    //Mat solution = left.inv() * right;
    Mat solution;
    solve(left, right, solution);
    double u = solution.at<double>(0, 0);
    double v = solution.at<double>(1, 0);
    //cout << "Check u and v: " << (u * w1.at<double>(0, 0) + v * w2.at<double>(0, 0) + px0.at<double>(0, 0)) - x.at<double>(0, 0) << endl;
    //cout << "u = " << u << ", v = " << v << endl;
    //cout << "(x, y): (" << u+400 << ", " << v+300 << ")" << endl;
    //return Point(solution.at<double>(0,0) + 400, solution.at<double>(1,0) + 300);
    return Point(solution.at<double>(0,0) + 400, solution.at<double>(1,0) + 300);
}

Mat render_cube(vector<Mat> vertices, int render_type, string filename, int iteration, Mat w1, Mat w2, Mat px0, Mat a, Mat n, Mat e){
    vector<Point> points;
    ofstream log;
    log.open("log.txt", std::ios_base::app);
    ofstream coor2d;
    if(iteration == 0)
        coor2d.open("coordinates2d.txt");
    else
        coor2d.open("coordinates2d.txt", std::ios_base::app);
    
    for(int i = 0; i < vertices.size(); i++){
        if(render_type == 1)
            points.push_back(ortho(vertices[i]));
        if(render_type == 2){
            Point pp = perspec(vertices[i]);
            points.push_back(pp);
            if(iteration < 4){
                coor2d << "(" << pp.x << ", " << pp.y << ")";
                if(i < vertices.size() - 1)
                    coor2d << ", ";
                else
                    coor2d << endl;
            }
        }
        
        if(render_type == 3){
            //cout << "hello" << endl;
            Mat projected = project(vertices[i], a, n, e);
            Point pp = getUV(projected, w1, w2, px0);
            points.push_back(pp);
            if(iteration < 4){
                if(i == 0)
                    log << "The frame" << iteration + 1 << " in 3d has the following edges:" << endl;
                log << toString(vertices[i]);
                if(i < vertices.size() - 1)
                    log << ", ";
                else
                    log << endl << endl;
            }
            if(iteration < 4){
                coor2d << "(" << pp.x << ", " << pp.y << ")";
                if(i < vertices.size() - 1)
                    coor2d << ", ";
                else
                    coor2d << endl;
            }
        }
    }
    
    
    Mat image = Mat::zeros( 600, 800, CV_8UC3 );
    
    for(int i = 0; i < points.size(); i++){
        MyFilledCircle(image, points[i]);
        if(iteration < 4){
            if(i == 0)
                log << "The frame" << iteration + 1 << " in 2d has the following edges:" << endl;
            log << "(" << points[i].x << ", " << points[i].y << ")";
            if(i < vertices.size() - 1)
                log << ", ";
            else
                log << endl << endl;
        }
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
    
    coor2d.close();
    log.close();
    
    return image;
}

Mat render_tetra(vector<Mat> vertices, int render_type, Mat w1, Mat w2, Mat px0, Mat a, Mat n, Mat e){
    vector<Point> points;
    for(int i = 0; i < vertices.size(); i++){
        if(render_type == 1)
            points.push_back(ortho(vertices[i]));
        if(render_type == 2)
            points.push_back(perspec(vertices[i]));
        if(render_type == 3){
            Mat projected = project(vertices[i], a, n, e);
            Point pp = getUV(projected, w1, w2, px0);
            points.push_back(pp);
            //cout << "Point: " << pp.x << " " << pp.y << endl;
        }
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
vector<Mat> make_video(vector<Mat> vertices, int cube, int render_type, Mat a, Mat n, Mat e){
    ofstream log;
    log.open("log.txt");
    log << "The plane defined by (x-a)*n = 0 is:" << endl;
    log << "   a = (" << a.at<double>(0, 0) << ", " << a.at<double>(1, 0) << ", " << a.at<double>(2, 0) << ")" << endl;
    log << "   n = (" << n.at<double>(0, 0) << ", " << n.at<double>(1, 0) << ", " << n.at<double>(2, 0) << ")" << endl;
    n = normalize(n);
    log << "   e = (" << e.at<double>(0, 0) << ", " << e.at<double>(1, 0) << ", " << e.at<double>(2, 0) << ")" << endl << endl;
    log.close();
    
    vector<Mat> w = getW1andW2(a, n, e, "log.txt");
    Mat w1 = w[0];
    Mat w2 = w[1];
    
    Mat px0 = getPx0(a, n, e, "log.txt");
    
    log.open("log.txt", std::ios_base::app);
    log << "The coordinates in the 2d plane x = p0 + u*w1 + v*w2 are: \n(where p0 is the origin, preferably the projection of the center of the cube in first frame, w1 and w2 are 2 perpendicular vertices in the plane)" << endl;
    log << "   p0 = " << toString(px0) << endl;
    log << "   w1 = " << toString(w[0]) << endl;
    log << "   w2 = " << toString(w[1]) << endl << endl;
    log.close();
    
    vector<Mat> frames;
    ofstream myfile;
    if(cube == 1){
        myfile.open("coordinates.txt");
        log.open("log.txt", std::ios_base::app);
    }
    
    int angle = 0;
    for(int angle = 0; angle <= 360; angle += 5){
        vector<Mat> current;
        for(int i = 0; i < vertices.size(); i++){
            current.push_back(transform(vertices[i], 400, 300, 100, angle/57.2957795));
        }
      
        if(cube == 1){
            frames.push_back(render_cube(current, render_type, "log.txt", angle/5, w1, w2, px0, a, n, e));
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
            frames.push_back(render_tetra(current, render_type, w1, w2, px0, a, n, e));
        }
    }

    myfile.close();
    
//     cout << "w1: [" << w[0].at<double>(0, 0) << ", " << w[0].at<double>(1, 0) << ", " << w[0].at<double>(2, 0) << "]" << endl;
//     cout << "w2: [" << w[1].at<double>(0, 0) << ", " << w[1].at<double>(1, 0) << ", " << w[1].at<double>(2, 0) << "]" << endl;
//     cout << "Checking w1 dot w2: " << dot(w[0], w[1]) << endl;
//     cout << "p0: (" << px0.at<double>(0, 0) << ", " << px0.at<double>(1, 0) << ", " << px0.at<double>(2, 0) << ")" << endl;
    
    return frames;
}

// void part1(){
//     double v1[1][4] = {{1, 1, 1, 1}};
//     double v2[1][4] = {{1, 1, -1, 1}};
//     double v3[1][4] = {{1, -1, 1, 1}};
//     double v4[1][4] = {{1, -1, -1, 1}};
//     double v5[1][4] = {{-1, 1, 1, 1}};
//     double v6[1][4] = {{-1, 1, -1, 1}};
//     double v7[1][4] = {{-1, -1, 1, 1}};
//     double v8[1][4] = {{-1, -1, -1, 1}};
//     vector<Mat> vertices = {Mat(4,1, CV_64FC1, v1), Mat(4,1, CV_64FC1, v2), Mat(4,1, CV_64FC1, v3), Mat(4,1, CV_64FC1, v4), Mat(4,1, CV_64FC1, v5),  
//                             Mat(4,1, CV_64FC1, v6) , Mat(4,1, CV_64FC1, v7), Mat(4,1, CV_64FC1, v8)};
//     vector<Mat> cubeframes = make_video(vertices, 1, 1);
    
//     double tv1[1][4] = {{1.0/sqrt(3), 0, 0, 1}};
//     double tv2[1][4] = {{0, 0, 2.0/sqrt(6), 1}};
//     double tv3[1][4] = {{-1*sqrt(3)/6.0, 1.0/2, 0 ,1}};
//     double tv4[1][4] = {{-1*sqrt(3)/6.0, -1*1.0/2, 0 ,1}};
//     vector<Mat> vertices_t = {Mat(4,1, CV_64FC1, tv1), Mat(4,1, CV_64FC1, tv2), Mat(4,1, CV_64FC1, tv3), Mat(4,1, CV_64FC1, tv4)};
//     vector<Mat> tetraframes = make_video(vertices_t, 0, 1);
    
//     int codec = VideoWriter::fourcc('M','J','P','G'); 
//     Size size = cubeframes[0].size(); 
//     VideoWriter video("rotation.avi", codec, 10, size);
//     for(int i = 0; i < cubeframes.size(); i++){
//         video.write(cubeframes[i]);
//     }
//     for(int i = 0; i < tetraframes.size(); i++){
//         video.write(tetraframes[i]);
//     }
// }

// void part2(){
//     double v1[1][4] = {{1, 1, 1, 1}};
//     double v2[1][4] = {{1, 1, -1, 1}};
//     double v3[1][4] = {{1, -1, 1, 1}};
//     double v4[1][4] = {{1, -1, -1, 1}};
//     double v5[1][4] = {{-1, 1, 1, 1}};
//     double v6[1][4] = {{-1, 1, -1, 1}};
//     double v7[1][4] = {{-1, -1, 1, 1}};
//     double v8[1][4] = {{-1, -1, -1, 1}};
//     vector<Mat> vertices = {Mat(4,1, CV_64FC1, v1), Mat(4,1, CV_64FC1, v2), Mat(4,1, CV_64FC1, v3), Mat(4,1, CV_64FC1, v4), Mat(4,1, CV_64FC1, v5),  
//                             Mat(4,1, CV_64FC1, v6) , Mat(4,1, CV_64FC1, v7), Mat(4,1, CV_64FC1, v8)};
//     vector<Mat> cubeframes = make_video(vertices, 1, 2);
    
//     double tv1[1][4] = {{1.0/sqrt(3), 0, 0, 1}};
//     double tv2[1][4] = {{0, 0, 2.0/sqrt(6), 1}};
//     double tv3[1][4] = {{-1*sqrt(3)/6.0, 1.0/2, 0 ,1}};
//     double tv4[1][4] = {{-1*sqrt(3)/6.0, -1*1.0/2, 0 ,1}};
//     vector<Mat> vertices_t = {Mat(4,1, CV_64FC1, tv1), Mat(4,1, CV_64FC1, tv2), Mat(4,1, CV_64FC1, tv3), Mat(4,1, CV_64FC1, tv4)};
//     vector<Mat> tetraframes = make_video(vertices_t, 0, 2);
    
//     int codec = VideoWriter::fourcc('M','J','P','G'); 
//     Size size = cubeframes[0].size(); 
//     VideoWriter video("rotation.avi", codec, 10, size);
//     for(int i = 0; i < cubeframes.size(); i++){
//         video.write(cubeframes[i]);
//     }
//     for(int i = 0; i < tetraframes.size(); i++){
//         video.write(tetraframes[i]);
//     }
// }

void part3(Mat a, Mat n, Mat e){
    double tv1[1][4] = {{1.0/sqrt(3), 0, 0, 1}};
    double tv2[1][4] = {{0, 0, 2.0/sqrt(6), 1}};
    double tv3[1][4] = {{-1*sqrt(3)/6.0, 1.0/2, 0 ,1}};
    double tv4[1][4] = {{-1*sqrt(3)/6.0, -1*1.0/2, 0 ,1}};
    vector<Mat> vertices_t = {Mat(4,1, CV_64FC1, tv1), Mat(4,1, CV_64FC1, tv2), Mat(4,1, CV_64FC1, tv3), Mat(4,1, CV_64FC1, tv4)};
    vector<Mat> tetraframes = make_video(vertices_t, 0, 3, a, n, e);
        
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
    vector<Mat> cubeframes = make_video(vertices, 1, 3, a, n, e);
    
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

// void test(){
//     Mat a = (Mat_<double>(3, 1) << 700, 0, 700);
//     Mat n = (Mat_<double>(3, 1) << 1, 0, 0);
//     Mat e = (Mat_<double>(3, 1) << 400, 100, 800);
//     getW1andW2(a, n, e);
//     getPx0(a, n, e);
// }

int main(int argc, char** argv ){
    Mat a = (Mat_<double>(3, 1) << 600, 0, 0);
    Mat n = (Mat_<double>(3, 1) << 1, 0, 0);
    n = normalize(n);
    Mat e = (Mat_<double>(3, 1) << 800, 0, 0);
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            string astr = argv[i + 1];
            double d1 = stod(astr.substr(1, astr.find(',')));
            astr = astr.substr(astr.find(',')+1);
            double d2 = stod(astr.substr(0, astr.find(',')));
            double d3 = stod(astr.substr(astr.find(',')+1));
            a = (Mat_<double>(3, 1) << d1, d2, d3);
        }
        else if (strcmp(argv[i], "-n") == 0) {
            string astr = argv[i + 1];
            double d1 = stod(astr.substr(1, astr.find(',')));
            astr = astr.substr(astr.find(',')+1);
            double d2 = stod(astr.substr(0, astr.find(',')));
            double d3 = stod(astr.substr(astr.find(',')+1));
            n = (Mat_<double>(3, 1) << d1, d2, d3);
        }
        else if (strcmp(argv[i], "-e") == 0) {
            string astr = argv[i + 1];
            double d1 = stod(astr.substr(1, astr.find(',')));
            astr = astr.substr(astr.find(',')+1);
            double d2 = stod(astr.substr(0, astr.find(',')));
            double d3 = stod(astr.substr(astr.find(',')+1));
            e = (Mat_<double>(3, 1) << d1, d2, d3);
        }
     }
    //part1();
    part3(a, n, e);
    //test();
}

    