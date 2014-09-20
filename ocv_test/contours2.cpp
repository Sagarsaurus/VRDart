#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <math.h>
#include <iostream>

using namespace cv;
using namespace std;

static void help()
{
    cout
    << "\nThis program illustrates the use of findContours and drawContours\n"
    << "The original image is put up along with the image of drawn contours\n"
    << "Usage:\n"
    << "./contours2\n"
    << "\nA trackbar is put up which controls the contour level from -3 to 3\n"
    << endl;
}

const int w = 1000;
int levels = 3;

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;

static void on_trackbar(int, void*)
{
    Mat cnt_img = Mat::zeros(w, w, CV_8UC3);
    int _levels = levels - 3;
    drawContours( cnt_img, contours, _levels <= 0 ? 3 : -1, Scalar(128,255,255),
                  3, CV_AA, hierarchy, std::abs(_levels) );

    imshow("contours", cnt_img);
}

int main( int argc, char** argv)
{
    std:string arg = argv[1];
    VideoCapture capture(arg); //try to open string, this will attempt to open it as a video file
    if (!capture.isOpened()) //if this fails, try to open as a video camera, through the use of an integer param
        capture.open(atoi(arg.c_str()));
    if (!capture.isOpened()) {
        cerr << "Failed to open a video device or video file!\n" << endl;
      //  help(av);
        return 1;
    }
  //  Mat img = Mat::zeros(w, w, CV_8UC1);
//    if(argc > 1)
 //   {
  //      help();
   //     return -1;
    //}
 //   const char* imagename = argc > 1 ? argv[1] : "lena.jpg";
  //  Ptr<IplImage> iplimg = cvLoadImage(imagename);
   // Mat imgin(iplimg);

    Mat imgin;
    Mat img;
    namedWindow( "image", CV_WINDOW_KEEPRATIO );
    std::vector<KeyPoint> keypoints_n;
    for(;;)
    {
	    capture >> imgin;
	    
	    if (imgin.empty())
                break;
	//     imshow("image", imgin);
	//     char key = (char)waitKey(5);

	    //show the faces
	    imgin.convertTo(img, CV_8U);
	    cvtColor(img, img, CV_BGRA2GRAY);
	    threshold( img, img, 10, 255, 1);

	    
	 //   imshow( "image", img );

	    int minHessian = 2000;  //SERF 2000
	    SurfFeatureDetector detector( minHessian );

	    std::vector<KeyPoint> keypoints_1;
	   // std::vector<KeyPoint> keypoints_n;
	     detector.detect( img, keypoints_1 );

	     //-- Draw keypoints
	     Mat img_keypoints_1;
	     float x[4];
	     float y[4];
	     int t[4];

	     for(int i = 0; i < 4; i ++)
	     {
		 x[i] = -1;
		 y[i] = -1;
		 t[i] = 0;
	     } 
	     int j = 0;
	     for(int i = 0; i < keypoints_1.size(); i++)
	     {
	     	char found = 0;
		float kx = keypoints_1[i].pt.x;
		float ky = keypoints_1[i].pt.y;
		j = 0;
		while(!found && j < 4)
		{
		   cout << endl;
		   if(t[j] == 0)
		   {
		       found = 1;
		       x[j] += kx;
		       y[j] += ky;
		       t[j]++;
		   }
		   else if(kx > (x[j] / t[j]) - 50 && kx < (x[j] / t[j]) + 50 && ky > (y[j] / t[j]) - 50 && ky < (y[j] / t[j]) + 50)
		   {
		       found = 1;
		       x[j] += kx;
		       y[j] += ky;
		       t[j]++;
		   }
		   j++;
		}
	     }

	     for(int i = 0; i < 4; i++)
	     {
		 keypoints_n.push_back(KeyPoint(x[i] / t[i], y[i] / t[i], 1));
	     }
	     drawKeypoints( imgin, keypoints_n, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

	     //-- Show detected (drawn) keypoints
	     imshow("image", img_keypoints_1 );
	     char key = (char)waitKey(5);
	}
    //Extract the icontours so that
 //   vector<vector<Point> > contours0;
   // findContours( img, contours0, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	
   // cout << contours0.size() << endl;
   // contours.resize(contours0.size());
   // for( size_t k = 0; k < contours0.size(); k++ )
    //    approxPolyDP(Mat(contours0[k]), contours[k], 3, true);

   // namedWindow( "contours", WINDOW_NORMAL );
  //  createTrackbar( "levels+3", "contours", &levels, 7, on_trackbar );

   // on_trackbar(0,0);
    waitKey();

    return 0;
}
