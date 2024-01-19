#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
 
// Defining the dimensions of checkerboard
int CHECKERBOARD[2]{7, 7}; 

void MyLine( cv::Mat img, cv::Point start, cv::Point end )
{
  int thickness = 2;
  int lineType = cv::LINE_8;
  line( img,
    start,
    end,
    cv::Scalar( 0, 0, 255 ),
    thickness,
    lineType );
}

void MyFilledCircle( cv::Mat img, cv::Point center )
{
  circle( img,
      center,
      6,
      cv::Scalar( 0, 0, 255 ),
      cv::FILLED,
      cv::LINE_8 );
}

cv::Mat drawCube(std::vector<cv::Point2f> points, cv::Mat image){
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
 
int main()
{
  // Creating vector to store vectors of 3D points for each checkerboard image
  std::vector<std::vector<cv::Point3f> > objpoints;
 
  // Creating vector to store vectors of 2D points for each checkerboard image
  std::vector<std::vector<cv::Point2f> > imgpoints;
    
  //Creating vector to store vectors of 3D points for the 4 inner corners of the checkerboards image
  std::vector<std::vector<cv::Point2f> > basepoints;
 
  // Defining the world coordinates for 3D points
  std::vector<cv::Point3f> objp;
  for(int i{0}; i<CHECKERBOARD[1]; i++)
  {
    for(int j{0}; j<CHECKERBOARD[0]; j++)
      objp.push_back(cv::Point3f(j,i,0));
  }
 
 
//   // Extracting path of individual image stored in a given directory
//   std::vector<cv::String> images;
//   // Path of the folder containing checkerboard images
//   std::string path = "./images/*.jpg";
 
//   cv::glob(path, images);
    
    cv::VideoCapture cap("withChessBoard.MOV");
    std::vector<cv::Mat> frames;
    for(int i = 0; i <=422; i++) {
        //std::cout<<i<<std::endl;
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) 
            break;
        frames.push_back(frame);
        //cv::imwrite("./capturedFrames/frame"+std::__cxx11::to_string(i)+".jpg",frame);
    }
 
  cv::Mat frame, gray;
  // vector to store the pixel coordinates of detected checker board corners 
  std::vector<cv::Point2f> corner_pts;
  std::vector<cv::Point2f> base_pts;
  std::vector<cv::Point2f> prevCorners;
  bool success;
 
  // Looping over all the images in the directory
  for(int i = 0; i<frames.size(); i++)
  {
    //std::cout<<i<<std::endl;
    frame = frames[i];
    cv::cvtColor(frame,gray,cv::COLOR_BGR2GRAY);
 
    // Finding checker board corners
    // If desired number of corners are found in the image then success = true  
    success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
     
    /* 
     * If desired number of corner are detected,
     * we refine the pixel coordinates and display 
     * them on the images of checker board
    */
    if(success)
    {
      cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 30, 0.001);
       
      // refining pixel coordinates for given 2d points.
      cv::cornerSubPix(gray,corner_pts,cv::Size(11,11), cv::Size(-1,-1),criteria);
       
      // Displaying the detected corner points on the checker board
      //cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);
       
      if(i % 9 == 0){
      objpoints.push_back(objp);
      imgpoints.push_back(corner_pts);
    }
        std::vector<cv::Point2f> b;
        b.push_back(corner_pts[16]); 
        b.push_back(corner_pts[18]);
        b.push_back(corner_pts[30]);
        b.push_back(corner_pts[32]);
        basepoints.push_back(b);
        
        prevCorners = corner_pts;
        //cv::goodFeaturesToTrack(frame, prevCorners, 49, 0.1, 10);
    }
    else{
        //std::cout << "in the else" << std::endl;
        std::vector<uchar> status;
        std::vector<float> err;
        cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);
        cv::Mat prevImg = frames[i-1].clone();
        cv::Mat nextImg = frame.clone();
        cv::cvtColor(prevImg,gray,cv::COLOR_BGR2GRAY);
        cv::cvtColor(nextImg,gray,cv::COLOR_BGR2GRAY);
        //std::vector<cv::Point2f> prevPts = basepoints[i-1];
        cv::calcOpticalFlowPyrLK(prevImg, nextImg, prevCorners, corner_pts, status, err);
        std::vector<cv::Point2f> b;
        b.push_back(corner_pts[16]); 
        b.push_back(corner_pts[18]);
        b.push_back(corner_pts[30]);
        b.push_back(corner_pts[32]);
        //added this line
        prevCorners = corner_pts;
        basepoints.push_back(b);
        //std::cout << "succesfully completed opticalflow" << std::endl;
    }
 
    //cv::imwrite("./calibratedFrames/frame"+std::__cxx11::to_string(i)+".jpg",frame);
    //cv::waitKey(0);
  }
 
  //cv::destroyAllWindows();
 
  cv::Mat cameraMatrix,distCoeffs,R,T;
 
  /*
   * Performing camera calibration by 
   * passing the value of known 3D points (objpoints)
   * and corresponding pixel coordinates of the 
   * detected corners (imgpoints)
  */
  cv::calibrateCamera(objpoints, imgpoints, cv::Size(gray.rows,gray.cols), cameraMatrix, distCoeffs, R, T);
 
//   std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
//   std::cout << "distCoeffs : " << distCoeffs << std::endl;
//   std::cout << "Rotation vector : " << R << std::endl;
//   std::cout << "Translation vector : " << T << std::endl;

    std::vector<cv::Point3f> basic_cube = {cv::Point3f(1,1,2), cv::Point3f(1, 1, -1), cv::Point3f(1,-1,2), cv::Point3f(1,-1,-1), cv::Point3f(-1,1,2), cv::Point3f(-1,1,-1), cv::Point3f(-1,-1,2), cv::Point3f(-1,-1,-1)};
//     std::vector<cv::Point3f> basic_cube = {
//     cv::Point3f(2, 2, 0),
//     cv::Point3f(2, 4, 0),
//     cv::Point3f(4, 4, 0),
//     cv::Point3f(4, 2, 0),
//     cv::Point3f(2, 2, 2),
//     cv::Point3f(2, 4, 2),
//     cv::Point3f(4, 4, 2),
//     cv::Point3f(4, 2, 2)
// };
    
    std::vector<cv::Point3f> base = {
    cv::Point3f(1, 1, -1),
    cv::Point3f(1, -1, -1),
    cv::Point3f(-1, 1, -1),
    cv::Point3f(-1, -1, -1),
};
    
    //std::cout << "made it to the for loop to draw the cube" << std::endl;
    
    int codec = cv::VideoWriter::fourcc('M','J','P','G'); 
    cv::Size size = frames[0].size();     
    cv::VideoWriter video("vr.avi", codec, 10, size);
    
    std::vector<cv::Point2f> cubepoints;
    for(int i = 0; i < frames.size(); i++){
      cv::solvePnP(base, basepoints[i], cameraMatrix, distCoeffs, R, T);
      cv::projectPoints(basic_cube, R, T, cameraMatrix, distCoeffs, cubepoints);
      cv:: Mat newframe = drawCube(cubepoints, frames[i]);
      cv::rotate(newframe, newframe, cv::ROTATE_180);
      //cv::imwrite("./cubeFrames/frame"+std::__cxx11::to_string(i)+".jpg",newframe);
      video.write(newframe);
    }
    
    //find the 4 corners in the middle
    //solvepnp?
    //use projectpoints to project basic cube onto
 
  return 0;
}