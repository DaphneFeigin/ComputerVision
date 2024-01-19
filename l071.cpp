#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace std;

using namespace cv;

int find_mode_radius(vector<Vec3f>& circles){
    vector<int> radii;
    for(int i = 0; i < circles.size(); i++) {
        radii.push_back(circles[i][2]);
    }
    sort(radii.begin(), radii.end());
    int count = 1;
    int max_occur = 1;
    int moder = radii[0];
    for(int i = 1; i < radii.size(); i++) {
        if(radii[i] == radii[i - 1])
            count++;
        else
            count = 1;
        if(count > max_occur){
            max_occur = count;
            moder = radii[i];
        }
    }
    return moder;
}

void classify_circles(Mat image, vector<Vec3f>& circles, int other_coins) {
    //cout << "finished call to get_ratios" << endl;
    int quarters = 0;
    int nickels = 0;
    int pennies = 0;
    int dimes = 0;
    int halfs = 0;
    int dollars = 0;
   
    int penny = find_mode_radius(circles);
    double dime = (.705/.750) * penny;
    double nickel = (0.835/.750) * penny;
    double quarter = (0.955/.750) * penny;
    double halfd = (1.205/.750) * penny;
    double dollar = (1.043/.750) * penny;
//     cout << "penny (mode): " << penny << endl;
//     cout << "quarter: " << quarter << endl;
    
    for (int i = 0; i < circles.size(); i++) {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        Vec3b color = image.at<Vec3b>(center); //might cause issues
        int radius = cvRound(circles[i][2]);
        vector<double> check;
        check.push_back(abs(radius-penny));
        check.push_back(abs(radius-nickel));
        check.push_back(abs(radius-quarter));
        if(other_coins = 1){
            check.push_back(abs(radius-dollar));
            check.push_back(abs(radius-dime));
            check.push_back(abs(radius-halfd));
        }
            
        sort(check.begin(), check.end());
        
        //Color is BGR
        if(color[2] > color[1] + 7 and color[2] > color[0] + 10){
           pennies++;
            circle(image, center, radius, Scalar(0, 0, 255), 3, 4, 0); 
        }
        
        else{
            if(check[0] == abs(radius-penny)){
                pennies++;
                circle(image, center, radius, Scalar(0, 0, 255), 3, 4, 0);
            }
            if(check[0] == abs(radius-dime)){
                dimes++;
                circle(image, center, radius, Scalar(255, 0, 0), 3, 4, 0);
            }
            else if(check[0] == abs(radius-nickel)){
                nickels++;
                circle(image, center, radius, Scalar(0, 255, 255), 3, 4, 0);
            }
            else if(check[0] == abs(radius-quarter)){
                quarters++;
                circle(image, center, radius, Scalar(180, 0, 120), 3, 4, 0);
            }
            else if(check[0] == abs(radius-halfd)){
                halfs++;
                circle(image, center, radius, Scalar(0, 255, 0), 3, 4, 0);
            }
            else if(check[0] == abs(radius-dollar)){
                dollars++;
                circle(image, center, radius, Scalar(255, 0, 255), 3, 4, 0);
            }
        }
    }
    
    imwrite("./imageCoins.jpg", image);
    
    //generate text file
    ofstream myfile;
    myfile.open("results.txt");
    myfile << dollars << " dollars, " << halfs << " half dollars, " << quarters << " quarters, " << dimes << " dimes, "<< nickels << " nickels, " 
        << pennies << " pennies" << endl;
    double total = dollars + 0.5 * halfs + quarters * 0.25 + dimes * 0.1 + nickels * 0.05 + pennies * 0.01;
    myfile << "Total sum: $" << setprecision(3) << total;
    myfile.close();

    cout << dollars << " dollars, " << halfs << " half dollars, " << quarters << " quarters, " << dimes << " dimes, "<< nickels << " nickels, " 
        << pennies << " pennies" << endl;
    cout << "Total sum: $" << setprecision(3) << total;
}

void find_coins(string file, int ht, int other_coins, int minD, int tc, int minR, int maxR, int inv_ratio){
    
    Mat original;
    original = imread( file, 1 );

    Mat image;
    image = imread( file, 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return;
    }

    Mat gray;
    gray = imread( file, IMREAD_GRAYSCALE );
    imwrite("./imageg.jpg", gray);
    medianBlur(gray, gray, 5);
    
    Mat edges;
    int lt = ht/3;
    Canny( gray, edges, lt, ht, 3);
    imwrite("./imagef.jpg", edges);
    
    vector<Vec3f> coins;
    HoughCircles(gray, coins, HOUGH_GRADIENT, inv_ratio, minD, ht, tc, minR, maxR);
    // Argument 1: Input image mode
    // Argument 2: A vector that stores 3 values: x,y and r for each circle.
    // Argument 3: CV_HOUGH_GRADIENT: Detection method.
    // Argument 4: The inverse ratio of resolution. (inv_ratio)
    // Argument 5: Minimum distance between centers. (minD)
    // Argument 6: Upper threshold for Canny edge detector. (ht)
    // Argument 7: Threshold for center detection. (tc)
    // Argument 8: Minimum radius to be detected. Put zero as default (minR)
    // Argument 9: Maximum radius to be detected. Put zero as default (maxR)
    int l = coins.size();
    cout << "number of coins: " << l << endl;
    
    for (size_t i = 0; i < coins.size(); i++)
    {
        Point center(cvRound(coins[i][0]), cvRound(coins[i][1]));
        int radius = cvRound(coins[i][2]);
        //circle(image, center, 3, Scalar(0, 255, 0), -1, 8, 0);
        circle(image, center, radius, Scalar(0, 0, 255), 3, 4, 0);
//         cout << " Center location for circle " << i + 1 << " :"<<center<<"\n Diameter : "<<2*radius<<"\n";
    }
    
    imwrite("./imageCircles.jpg", image);
    
    classify_circles(original, coins, other_coins);
}

int main(int argc, char** argv )
{
    string f = "easy.jpg";
    int ht = 120;
    int other_coins = 0;
    int minD = 100;
    int tc = 60;
    int minR = 85;
    int maxR = 130;
    int inv_ratio = 2;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            f = argv[i + 1];
        }
        else if (strcmp(argv[i], "-ht") == 0) {
            ht = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-oc") == 0) {
            other_coins = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-minD") == 0) {
            minD = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-tc") == 0) {
            tc = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-minR") == 0) {
            minR = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-maxR") == 0) {
            maxR = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-ir") == 0) {
            inv_ratio = stoi(argv[i + 1]);
        }
     }
    find_coins( f,  ht,  other_coins,  minD,  tc,  minR,  maxR,  inv_ratio);
    return 0;
}