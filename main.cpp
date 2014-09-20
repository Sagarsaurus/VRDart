#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <iostream>

using namespace cv;

double alpha; /**< Simple contrast control */
int beta;  /**< Simple brightness control */

int main( int argc, char** argv )
{
    VideoCapture cap(0);
    if(!cap.isOpened()) // check if we succeeded
        return -1;
    
    Mat image;
    namedWindow("edges",1);
    
    SimpleBlobDetector::Params params = SimpleBlobDetector::Params();
    
    params.minDistBetweenBlobs = 50.0f;
    params.filterByInertia = false;
    params.filterByConvexity = false;
    params.filterByColor = false;
    params.filterByCircularity = false;
    params.filterByArea = true;
    params.minArea = 1000.0f;
    params.maxArea = 5000.0f;
    params.blobColor = 0;
    
    SimpleBlobDetector detector( params );
    std::vector<KeyPoint> keypoints_1;

    for(;;) {
        cap >> image; // get a new frame from camera
        
        image.convertTo(image, -1, 2, 0);
        
        detector.detect( image, keypoints_1 );
        
        std::cout<<keypoints_1.size()<<std::endl;
        
        
        
        //-- Draw keypoints
        Mat img_keypoints_1;
        
        drawKeypoints( image, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
        
        imshow("feed", img_keypoints_1);
        
        std::cout<<std::endl;
        
        if(waitKey(30) >= 0) break;
        
    }
    
//    resize(src, image, Size(), 0.2, 0.2, CV_INTER_AREA);
    
    // Wait until user press some key
    waitKey();
    return 0;
}