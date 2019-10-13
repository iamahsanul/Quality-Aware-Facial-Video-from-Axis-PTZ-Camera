/** Basic OpenCV example for face detecion and tracking with CamShift.
* First run Haar classifier face detection on every frame until finding face,
* then switch over to CamShift using face region to track.
**/
#include "trackface.h"
#include "camshifting.h"
 
char *classifer = "C:\\Program Files\\opencv\\data\\haarcascades\\haarcascade_frontalface_alt_tree.xml";
 
int main1 () {
CvHaarClassifierCascade* cascade = 0;
CvMemStorage* storage = 0;
char* window_name = "haar window";
//initialize
cascade = (CvHaarClassifierCascade*) cvLoad(classifer, 0, 0, 0 );
storage = cvCreateMemStorage(0);
//validate
assert(cascade && storage);
 
//create window
cvNamedWindow(window_name, CV_WINDOW_AUTOSIZE);
 
CvRect* face_rect = 0;
//video file?
//CvCapture* capture = cvCaptureFromFile("http://root:n24434@172.28.247.53/axis-cgi/mjpg/video.cgi?resolution=4CIF&.mjpg");
CvCapture* capture = cvCaptureFromFile("http://root:n24434@169.254.164.233/axis-cgi/mjpg/video.cgi?resolution=4CIF&.mjpg");

IplImage* image = 0;
if (capture) {
//run loop, exit on ESC
while (1) {
image = capture_video_frame(capture);
if (!image) break;
 
face_rect = detect_face(image, cascade, storage);
cvShowImage(window_name, image); //display
 
//face detected, exit loop and track
if (face_rect) break;
//exit program on ESC
if ((char)27 == cvWaitKey(10)) {
cvReleaseCapture(&capture);
cleanup(window_name, cascade, storage);
exit(0);
}
}
printf("Face detected, tracking with CamShift...\n");
TrackedObj* tracked_obj = create_tracked_object(image, face_rect);
 
CvBox2D face_box; //area to draw
//track detected face with camshift
while (1) {
//get next video frame
image = capture_video_frame(capture);
if (!image) break;
 
//track the face in the new frame
face_box = camshift_track_face(image, tracked_obj);
 
//outline face ellipse
cvEllipseBox(image, face_box, CV_RGB(255,0,0), 3, CV_AA, 0);
cvShowImage(window_name, image); //display
 
//exit on ESC
if ((char)27 == cvWaitKey(10)) break;
}
 
//free memory
destroy_tracked_object(tracked_obj);
cvReleaseCapture(&capture);
}
 
//release resources and exit
cleanup(window_name, cascade, storage);
}
 
 
/* Given an image and a classider, detect and return region. */
CvRect* detect_face (IplImage* image,
CvHaarClassifierCascade* cascade,
CvMemStorage* storage) {
 
CvRect* rect = 0;
//get a sequence of faces in image
CvSeq *faces = cvHaarDetectObjects(image, cascade, storage,
1.1, //increase search scale by 10% each pass
6, //require 6 neighbors
CV_HAAR_DO_CANNY_PRUNING, //skip regions unlikely to contain a face
cvSize(0, 0)); //use default face size from xml
 
//if one or more faces are detected, return the first one
if(faces && faces->total)
rect = (CvRect*) cvGetSeqElem(faces, 0);
 
return rect;
}
 
/* Capture frame and return a copy so not to write to source. */
IplImage* capture_video_frame (CvCapture* capture) {
//capture the next frame
//frame_copy = cvQueryFrame(capture);
frame_curr = cvQueryFrame(capture);
frame_copy = cvCreateImage(cvGetSize(frame_curr), 8, 3);
assert(frame_curr && frame_copy); //make sure it's there
 
//make copy of frame so we don't write to src
cvCopy(frame_curr, frame_copy, NULL);
frame_copy->origin = frame_curr->origin;
 
//invert if needed, 1 means the image is inverted
if (frame_copy->origin == 1) {
cvFlip(frame_copy, 0, 0);
frame_copy->origin = 0;
}
return frame_copy;
}
 
void cleanup (char* name,
CvHaarClassifierCascade* cascade,
CvMemStorage* storage) {
//cleanup and release resources
cvDestroyWindow(name);
if(cascade) cvReleaseHaarClassifierCascade(&cascade);
if(storage) cvReleaseMemStorage(&storage);
}
 
void print_help (char* name) {
printf("Usage: %s [video]\n", name);
exit(-1);
}