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

class VRDETECTOR {
    VideoCapture cap;
    SimpleBlobDetector detector;
    std::vector<KeyPoint> refPoints;
    
    struct PointCount {
        KeyPoint p;
        int count, age;
    };
    
public:
    VRDETECTOR(){
        cap = VideoCapture(0);
        if(!cap.isOpened()) // check if we succeeded
            std::cout<< "Camera not recognized";
        
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
        detector = SimpleBlobDetector( params );
        
//        namedWindow("edges",1);

    }
    
    void eventLoop(){
        Mat image;
        std::vector<KeyPoint> keypoints;
        while (true){
            cap >> image; // get a new frame from camera
            
            image.convertTo(image, -1, 2, 0);
            
            detector.detect( image, keypoints );
            
            std::cout<<keypoints.size()<<std::endl;
            
            //-- Draw keypoints
            Mat img_keypoints_1;
            
            drawKeypoints( image, keypoints, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
            
            imshow("feed", img_keypoints_1);
            
            std::cout<<std::endl;
            
            if(waitKey(30) >= 0) break;
            
        }

    }
};

int main( int argc, char** argv )
{
    VRDETECTOR det = VRDETECTOR();
    det.eventLoop();
    return 0;
}

