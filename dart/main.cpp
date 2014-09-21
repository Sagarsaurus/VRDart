#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
using namespace cv;

double alpha; /**< Simple contrast control */
int beta;  /**< Simple brightness control */

pthread_mutex_t netMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  netCond   = PTHREAD_COND_INITIALIZER;
pthread_t netThread;

float hX;
float hY;
char readyToSend = 0;

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
        params.filterByColor = true;
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
        float markX = -1.0;
        float markY = -1.0;
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

            
            for (PointCount old_p: pointHistory){
                if(markX < 0 || markY < 0 || (markX < old_p.p.pt.x && markY < old_p.p.pt.y))
                {
                    markX = old_p.p.pt.x;
                    markY = old_p.p.pt.y;
                }
            }
           
            //loads the x,y location and tells the net thread to send it 
            pthread_mutex_lock( &netMutex );
            hX = hitX - markX;
            hY = hitY - markY;
            readyToSend = 1;
            pthread_cond_signal( &netCond);
            pthread_mutex_unlock(&netMutex);
            
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
            
            vector<Mat> channels(3);
            
            split(image, channels);
            
            image = channels[2];
            
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

int initNetwork(int port)
{
    int listenfd,connfd,n;
    struct sockaddr_in serverAddr,clientAddr;
    socklen_t clientLen;

    listenfd=socket(AF_INET,SOCK_STREAM,0);

    bzero(&serverAddr,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    serverAddr.sin_port=htons(port);
    bind(listenfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr));

    listen(listenfd,1024); 
    connfd = accept(listenfd,(struct sockaddr *)&clientAddr,&clientLen);

    return connfd;
}

void *ui_comm( void *ptr )
{
    
    int buf[2];
    int rc = 0;
    int s = *((int*)ptr);
    while(1)
    {
        pthread_mutex_lock( &netMutex );
        while (!readyToSend && rc == 0)
        {
            rc = pthread_cond_wait( &netCond, &netMutex );
        }
        buf[0] = htonl((int)hX);
        buf[1] = htonl((int)hY);
        readyToSend = 0; 
        pthread_mutex_unlock(&netMutex);
        
        send(s, buf, sizeof(buf),0);
        std::cout << "test" << std::endl; 
    }
}

int main( int argc, char** argv )
{
//    int s = initNetwork(56465);
//    int net1 = pthread_create(&netThread, NULL, ui_comm, (void*)&s);
    VRDETECTOR det = VRDETECTOR();
        
    det.eventLoop();
//    pthread_join(net1, NULL);
//  close(s);
    return 0;
}

