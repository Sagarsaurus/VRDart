#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace cv;

double alpha; /**< Simple contrast control */
int beta;  /**< Simple brightness control */

class VRDETECTOR {
    VideoCapture cap;
    SimpleBlobDetector detector;
    std::vector<KeyPoint> refPoints;
    std::vector<KeyPoint> hitPoints;
    int sockHit = 0;
    int sockHitTimer = 0;
    struct PointCount {
        KeyPoint p;
        int count, age;
    };
    
    std::vector<PointCount> pointHistory;
    
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
        params.minArea = 200.0f;
        params.maxArea = 5000.0f;
        params.blobColor = 0;
        detector = SimpleBlobDetector( params );
        
//        namedWindow("edges",1);

    }
    
    bool inline isSimilar(KeyPoint p1, KeyPoint p2, int threshold){
        return abs(p1.pt.x - p2.pt.x) < threshold &&
            abs(p1.pt.y - p2.pt.y) < threshold &&
            abs(p1.size - p2.size) < threshold;
    }
    
    void updateRefPoints(std::vector<KeyPoint> keypoints){
        std::vector<PointCount> newPoints;
        float hitX = 0.0;
        float hitY = 0.0;
        for (KeyPoint new_p: keypoints){
            bool isNew = true;
            for (PointCount &old_p: pointHistory){
                if (isSimilar(new_p, old_p.p, 50)) {
                    old_p.count++;
                    isNew = false;
                    break;
                }
            }
            if (isNew) 
            {
                newPoints.push_back({new_p, 0, 0});
                sockHit = !sockHitTimer;
            }
        }

        if(sockHit)
        {
            for (PointCount hit_p: newPoints)
            {
                hitX += hit_p.p.pt.x;
                hitY += hit_p.p.pt.y; 
                
            }
            hitX /= newPoints.size();
            hitY /= newPoints.size();
            
            hitPoints.push_back(KeyPoint(hitX, hitY, 1));
            sockHit = 0;
            sockHitTimer++;
        }

        if(sockHitTimer) //count and reset sockHit to not count extra points
        {
            sockHitTimer++;
            if(sockHitTimer >= 20)
            {
                sockHitTimer = 0;
            }
        }
	
	

        for (PointCount &old_p: pointHistory){
            old_p.age++;
        }
        pointHistory.erase(std::remove_if(pointHistory.begin(), pointHistory.end(), [](PointCount p){
            return (p.age > 3) && (((float)p.count / p.age) < 0.95);
        }), pointHistory.end());
        for (PointCount new_p: newPoints)
            pointHistory.push_back(new_p);
        refPoints.clear();
        for (PointCount p: pointHistory)
            refPoints.push_back(p.p);
        for (KeyPoint p: hitPoints)
            refPoints.push_back(p);

        for( std::vector<PointCount>::const_iterator i = pointHistory.begin(); i != pointHistory.end(); ++i)
            if (i->age > 25) {
                std::cout << " ("<< (int) i->p.pt.x << ' '<< (int) i->p.pt.y << ") ";
            }
        std::cout << std::endl;
        for( std::vector<PointCount>::const_iterator i = pointHistory.begin(); i != pointHistory.end(); ++i)
            if(i->age < 7) {
                std::cout << " ("<< (int) i->p.pt.x << ' '<< (int) i->p.pt.y << ") ";
            }
        std::cout << std::endl;
    }
    
    void eventLoop(){
        Mat image;
        std::vector<KeyPoint> keypoints;
        while (true){
            cap >> image; // get a new frame from camera
            
            //cvtColor(image,image,CV_RGB2GRAY);
            image = image(cv::Rect(100, 100, 600, 500));
            
            image.convertTo(image, -1, 2, 0);
            
            detector.detect( image, keypoints );
            
            updateRefPoints(keypoints);
            
            std::cout<<keypoints.size()<< " " << pointHistory.size()<< std::endl;
            
            //-- Draw keypoints
            Mat img_keypoints_1;
            
            drawKeypoints( image, refPoints, img_keypoints_1, Scalar(255, 0, 255), DrawMatchesFlags::DEFAULT );
            
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

