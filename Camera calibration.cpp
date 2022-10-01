
/* The detail explanation of opencv camera calibration can found in this page..http://learnopencv.com/camera-calibration-using-opencv/... */


//import c++ library
#include <stdio.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdio>


// import opencv library
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// namespaces
using namespace std;
using namespace cv;

// declaration of checkboard information
int CHECKERBOARD[2]{ 11,8 };              // number of linked squares corners in x and y direction (consider both white and black squares) / if the different pattern is used it must be change according to the chessboard

int main()
{
    // declaration of imagepoints and objectpoints vector
    vector<vector<Point3f> > objpoints;
    vector<vector<Point2f> > imgpoints;

    // define 3D Real world distance coordinate like (1,0,0).....(1,2,0).....
    vector<Point3f> objp;
    for (int i{ 0 }; i < CHECKERBOARD[1]; i++)
    {
        for (int j{ 0 }; j < CHECKERBOARD[0]; j++)
            objp.push_back(cv::Point3f(j, i, 0));
    }

    // loading the calibration image from stored image directory
    vector<String> images;
    string path = "D:/calibration_photos/new_bigsize_pattern/iloveimg-resized/*.jpg";
    glob(path, images);


    // find checkboard corners with opencv lib functions
    Mat frame, gray;
    // vector to store the pixel coordinates of detected checker board corners 
    vector<Point2f> corner_pts;
    bool success;

    // Looping over all the images in the directory
    for (int i{ 0 }; i < images.size(); i++)
    {
        // counting the image number in calibration
        cout << "Image_" << i+1 << endl;

        // Finding checker board corners
        // If desired number of corners are found in the image then success = true  
        frame = imread(images[i]);
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        success = findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
       
        if (success)
        {
            TermCriteria criteria(TermCriteria::EPS | TermCriteria::MAX_ITER, 30, 0.001);
            cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);

            // Displaying the detected corner points 
            drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);

            // push back the detected corners in object and image points
            objpoints.push_back(objp);
            imgpoints.push_back(corner_pts);
        }

        imshow("Image", frame);
        waitKey(0);
    }

        destroyAllWindows();

        // declaring the required calibrating matrixes
        Mat cameraMatrix, distCoeffs, R, T;

        // calibration
        calibrateCamera(objpoints, imgpoints, Size(gray.rows, gray.cols), cameraMatrix, distCoeffs, R, T);

        // displaying the calculated calibration matrixes
        cout << "cameraMatrix : " << cameraMatrix << endl;
        cout << "distCoeffs : " << distCoeffs << endl;
        cout << "Rotation vector : " << R << endl;
        cout << "Translation vector : " << T << endl;



        // undistorsion of the image 
        
        // Precompute lens correction interpolation
        Mat mapX, mapY;
        initUndistortRectifyMap(cameraMatrix, distCoeffs, Matx33f::eye(), cameraMatrix, Size(gray.rows, gray.cols), CV_32FC1, mapX, mapY);

            // read the image taken by same camera
            Mat img = imread("D:/calibration_photos/mobile_photos/iloveimg-resized/IMG_20220601_093050.jpg", IMREAD_COLOR);

            // undistorsion
            Mat imgUndistorted;
            remap(img, imgUndistorted, mapX, mapY, INTER_LINEAR);

            // Display undistorted image
            imshow("undistorted image", imgUndistorted);
            
            
            waitKey(0);
            destroyAllWindows();

    return 0; 
}

