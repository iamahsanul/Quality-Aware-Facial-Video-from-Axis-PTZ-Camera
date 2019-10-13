#ifndef INC_OPENCV
#define INC_OPENCV
#include <stdio.h>
#include <cv.h>
#include <opencv/highgui.h>
#endif
 
IplImage* capture_video_frame (CvCapture*);
CvRect* detect_face (IplImage*, CvHaarClassifierCascade*, CvMemStorage*);
void cleanup (char*, CvHaarClassifierCascade*, CvMemStorage*);
void print_help (char*);
 
//used by capture_video_frame, so we don't have to keep creating.
IplImage* frame_curr;
IplImage* frame_copy;