
/*!
*\class control
*\brief main class of tracking system
*/


/*!
*Author: Mohammad Ahsanul Haque (iamahsanul@gmail.com)
*Aalborg University, Denmark
*/

#include "../include/control.h"
#include "camshifting.h"
#include "FIQA.h"
#include<iostream>
#include<fstream>
#include <time.h>


using namespace cv;
using namespace std;

  CvMemStorage* pStorageface = 0;
  CvHaarClassifierCascade* face_cascade_classifier = 0;
  RNG rng(12345);
  
  CvVideoWriter* cameraVideo, *camVidFace, *camVidCamshiftTrack, *camVidallFace, *camVidQualifiedFace;
  int numofcamshiftFrame = 0, videoFrameCount = 0;
  time_t tstartVideoFrame=0, tendVideoFrame=0, tstartCamShiftFrame=0, tendCamShiftFrame=0, tstartQuaAssess=0, tendQuaAssess=0;
  double tSecond=0, tTotalQuaAssess=0, tTotalCamShiftFaceDect=0, tTotalVideoFrame=0;



control::control(std::string ip)
{
	/*initialisation des donnees membres
	* creation du httpclient, reccuperation du flux video
	* creation du stateManager
	*/
	ipCam=ip;
	http= new httpClient(ip.c_str());
//	http->setInitPos(178,160,200);
//	http->setInitPos(0,20,200);
	http->setInitPos(0,10,200);
	http->goToInit();


//	for(int i=0;i<200;i++)
//	http->setAreaZoom(0,20, 101);

//	cap=http->getMpg("320x240");
	cap=http->getMpg("640x480");
	init(cap);

	/*
	 * lancer le traitement de la video
	 */
	process();
}

control::control(const char* videoFileName)
{
	/*initialisation des donnees membres
	* creation du httpclient, reccuperation du flux video
	* creation du stateManager
	*/
	cap=cvCaptureFromFile(videoFileName);
	init(cap);

	/*
	 * lancer le traitement de la video
	 */
	process();
}

void control::init(CvCapture* cap)
{
	IplImage* faceFrame=0;
	CvRect* faceRect=0;
    const char * DISPLAY_WINDOW = "Haar Window";
	TrackedObj* tracked_obj = 0;
	CvRect face_box; //area to draw
	int xtr, ytr, ztr;
	int count; //rough local variable


	tmp_frame = cvQueryFrame(cap);
	if(!tmp_frame)
	{
		std::cout << "Bad video !" << std::endl;
	}

	cameraVideo = cvCreateVideoWriter("./record/CameraVideo.mpeg",CV_FOURCC('M','P','E','G'),20,cvGetSize(tmp_frame),1);
	camVidCamshiftTrack = cvCreateVideoWriter("./record/CameraVidCamshiftTrack.mpeg",CV_FOURCC('M','P','E','G'),20,cvGetSize(tmp_frame),1);


	//////////////////Run loop to detect face, exit on ESC
	face_cascade_classifier = (CvHaarClassifierCascade *)cvLoad("C:\\Program Files\\opencv\\data\\haarcascades\\haarcascade_frontalface_alt_tree.xml", 0 , 0, 0);
	for(int i=0;i<100;i++) {
		tmp_frame = cvQueryFrame(cap);
		cvWaitKey(5);
	}

	//////////////////////Processing in loop after camera initialization for the first time 
	time(&tstartVideoFrame);
	faceDetectionAndCameraInitialPhase:  //this label is called after camshift lost the object and reset the camera zoom
	while (1) {
		tmp_frame = cvQueryFrame(cap);
		videoFrameCount++;
		cvWriteFrame(cameraVideo,tmp_frame);
		faceRect = vjFaceDetect(tmp_frame, 10, 10);

		if(!faceRect) {
			tmp_frame = cvQueryFrame(cap);
			videoFrameCount++;
			cvWriteFrame(cameraVideo,tmp_frame);
			cvWriteFrame(camVidCamshiftTrack,tmp_frame);
			cvShowImage(DISPLAY_WINDOW, tmp_frame); //display
			cvWaitKey(5);
			continue;
		}

		printf("Face detected, tracking with CamShift...\n");
		cvRectangleR(tmp_frame, *faceRect, CV_RGB(255,255,255), 3, 4, 0);
		cvWriteFrame(camVidCamshiftTrack,tmp_frame);
		cvShowImage(DISPLAY_WINDOW, tmp_frame); //display
		cvWaitKey(5);
		/////////////////////////End of Run loop to detect face, exit on ESC

		/////////////////////////Camera zooming to the detected face in the full frame
		xtr=(int)(faceRect->x+(faceRect->width/2));
		ytr=(int)(faceRect->y+(faceRect->height/2));
		ztr = (int)(sqrt(80000/(faceRect->height * faceRect->width))*100); 
		http->setAreaZoom(xtr,ytr,ztr);

		for(int i=0;i<50;i++) {
			tmp_frame = cvQueryFrame(cap);
			videoFrameCount++;
			cvWriteFrame(cameraVideo,tmp_frame);
			cvWriteFrame(camVidCamshiftTrack,tmp_frame);
			cvShowImage(DISPLAY_WINDOW, tmp_frame); //display
			cvWaitKey(5);
		}
		//////////////////////////////////camera zooming end

		///////////////////////////Detecting face in the zoomed area
		count = 0;
		while(1) {

			////////////If camera missed the tracking face in the zoomed area for 50 frames
			if(count++>=50) {
//				http->setInitPos(0,20,200);
				http->setInitPos(0,10,200);
				http->goToInit();
				for(int i=0;i<50;i++) {
					tmp_frame = cvQueryFrame(cap);
					videoFrameCount++;
					cvWriteFrame(cameraVideo,tmp_frame);
					cvWriteFrame(camVidCamshiftTrack,tmp_frame);
					cvShowImage(DISPLAY_WINDOW, tmp_frame); //display
					cvWaitKey(5);
				}
				goto faceDetectionAndCameraInitialPhase;   //As the tracking object is missing in the zoomed area starting from the initial position of the camera and facedetection and tracking
			}
			/////////////End of If camera missed the tracking face in the zoomed area for 50 frames

			tmp_frame = cvQueryFrame(cap);
			videoFrameCount++;
			cvWriteFrame(cameraVideo,tmp_frame);
			faceRect = vjFaceDetect(tmp_frame, 70, 70);
			if(faceRect) {
				cvRectangleR(tmp_frame, *faceRect, CV_RGB(255,255,255), 3, 4, 0);
				cvWriteFrame(camVidCamshiftTrack,tmp_frame);
	 			cvShowImage(DISPLAY_WINDOW, tmp_frame); //display
				cvWaitKey(5);
				break;
			}
			cvWriteFrame(camVidCamshiftTrack,tmp_frame);
	 		cvShowImage(DISPLAY_WINDOW, tmp_frame); //display
			cvWaitKey(5);
		}

//		cvRectangleR(tmp_frame, *faceRect, CV_RGB(255,255,255), 3, 4, 0);
//		cvShowImage(DISPLAY_WINDOW, tmp_frame); //display
//		cvWaitKey(5);
		///////////////////////////End of Detecting face in the zoomed area


		////////////////////////////Run loop for face tracking
		tracked_obj = create_tracked_object(tmp_frame, faceRect);

		//track detected face with camshift
		while (1) {
			//get next video frame
			tmp_frame = cvQueryFrame(cap);
			if (!tmp_frame) break;
			videoFrameCount++;
			cvWriteFrame(cameraVideo,tmp_frame);
 
			//track the face in the new frame
			time(&tstartCamShiftFrame);
			face_box = camshift_track_face(tmp_frame, tracked_obj);
			///////////Resetting the camera if camshift failed to keep track
			if(face_box.height*face_box.width<=20000||face_box.height*face_box.width>=200000) {
					numofcamshiftFrame++; 
					cvRectangleR(tmp_frame, face_box, CV_RGB(255,255,255), 3, 4, 0);
					cvWriteFrame(camVidCamshiftTrack,tmp_frame);
//					http->setInitPos(0,20,200);
					http->setInitPos(0,10,200);
					http->goToInit();
					for(int i=0;i<50;i++) {
						tmp_frame = cvQueryFrame(cap);
						videoFrameCount++;
						cvWriteFrame(cameraVideo,tmp_frame);
						numofcamshiftFrame++; 
						cvWriteFrame(camVidCamshiftTrack,tmp_frame);
						cvShowImage(DISPLAY_WINDOW, tmp_frame); //display
						cvWaitKey(5);
					}
					goto faceDetectionAndCameraInitialPhase;   //As the tracked object is missing starting from the initial position of the camera and facedetection and tracking

			}
			///////////End of Resetting the camera if camshift failed to keep track


			///////////////////////////////Face image quality assesment
			numofcamshiftFrame++; 
			faceFrame = tmp_frame; faceRect = &face_box;
			faceQulalityAssess(faceFrame, faceRect);
			///////////////////////////////End of image quality assesment

//			cvRectangleR(tmp_frame, face_box, CV_RGB(255,255,255), 3, 4, 0);
			cvShowImage(DISPLAY_WINDOW, tmp_frame); //display
 
			//exit on ESC
			if ((char)27 == cvWaitKey(10)) break;
		}
		///////////////////////////////////End of run loop for face tracking
	}
	//////////////////////End of Processing in loop after camera initialization for the first time 

	//    Releasing and freeing up the memory
	destroy_tracked_object(tracked_obj);
	cvReleaseCapture(&cap);
	if(face_cascade_classifier) cvReleaseHaarClassifierCascade(&face_cascade_classifier);
	if(pStorageface) cvReleaseMemStorage(&pStorageface);
	cvReleaseVideoWriter(&cameraVideo);
	cvReleaseVideoWriter(&camVidCamshiftTrack);

//	stateSwitch = new stateManager(tmp_frame);
}

void control::process()
{
	tmp_frame = cvQueryFrame(cap);
	CvVideoWriter* MaVideo = cvCreateVideoWriter("./record/testTrack.mpeg",CV_FOURCC('M','P','E','G'),30,cvGetSize(tmp_frame),1);
	cvNamedWindow("test_tracking", 1);
	//cvNamedWindow("BG_proj", 1);
	//CvRect bla;
	int xtr=0,ytr=0,ztr=100;
	bool initOK=true;

	for( int fr = 1;tmp_frame; tmp_frame = cvQueryFrame(cap), fr++ )
	{
		stateSwitch->process(tmp_frame);
		if(fr%5==0 && stateSwitch->getActive()==TRACKING_STATE)
		{
			xtr=(int)(stateSwitch->getTarget()->x+(stateSwitch->getTarget()->width/2))*704/320;
			ytr=(int)(stateSwitch->getTarget()->y+(stateSwitch->getTarget()->height/2))*576/240;

			if(stateSwitch->getTarget()->width*stateSwitch->getTarget()->height<10000)
			{
				ztr=105;
			}
			else if(stateSwitch->getTarget()->width*stateSwitch->getTarget()->height>12000)
			{
				ztr=95;
			}
			else
			{
				ztr=100;
			}
			http->setAreaZoom(xtr,ytr,ztr);
			initOK=false;
		}
		else if(!initOK && stateSwitch->getActive()==BG_STATE)
		{
			http->goToInit();
			initOK=true;
		}
		cvShowImage("test_tracking", tmp_frame);
		//cvShowImage("BG_proj", stateSwitch->bgSub->bg_model->foreground);
		cvWriteFrame(MaVideo,tmp_frame);
	}
	cvReleaseVideoWriter(&MaVideo);
}

CvRect* control::vjFaceDetect(IplImage* frame, int cvSizeWidth, int cvSizeHeight)
{

   CvSeq * pFaceRectSeq;               // memory-access interface
   CvRect* faceRect = 0;
   pStorageface = cvCreateMemStorage(0);
   
    // detect faces in image
	pFaceRectSeq = cvHaarDetectObjects
		(frame, face_cascade_classifier, pStorageface,
		1.1,                       // increase search scale by 10% each pass
		3,                         // merge groups of three detections
		CV_HAAR_DO_CANNY_PRUNING,  // skip regions unlikely to contain a face
		cvSize(cvSizeWidth,cvSizeHeight));            // smallest size face to detect = 40x40


    const char * DISPLAY_WINDOW = "Haar Window";
	int i;
	
	// create a window to display detected faces
	cvNamedWindow(DISPLAY_WINDOW, CV_WINDOW_AUTOSIZE);

    //if one or more faces are detected, return the first one

    if(pFaceRectSeq && pFaceRectSeq->total){
		faceRect = (CvRect*) cvGetSeqElem(pFaceRectSeq, 0);
		CvPoint pt1 = { faceRect->x, faceRect->y };
		CvPoint pt2 = { faceRect->x + faceRect->width, faceRect->y + faceRect->height };
		cvRectangle(frame, pt1, pt2, CV_RGB(255,255,255), 3, 4, 0);
	}

	// display face detections
	cvShowImage(DISPLAY_WINDOW, frame);
	return faceRect;
}


void control::faceQulalityAssess(IplImage* frame, CvRect* rect)
{
	IplImage *face, *faceFrame, *blackWhiteImage;
	CvRect *faceRect, tmpRect;
	float face_resolution, face_brightness, face_sharpness, face_pose, face_exists, total_score;
	long centermassx, centermassy, centerfacey, centerfacex; 
	static int numoffaceFrame = 0, numofqulifiedFace = 0;
	float resolution_weight, sharpness_weight, pose_weight, brightness_weight;
	char fileName[60];
	ofstream outputFile("./record/output.txt", std::ios_base::app);


//	numofcamshiftFrame++;

	rect = &cvRect(rect->x-(0.1*rect->width), rect->y-(0.1*rect->width), rect->width*1.2, rect->height*1.2);
	if(rect->x<0) rect->x = 0;
	if(rect->y<0) rect->y = 0;
	if((rect->x+rect->width)>frame->width) rect->width = frame->width - rect->x;
	if((rect->y+rect->height)>frame->height) rect->height = frame->height - rect->y;

	cvSetImageROI(frame, *rect);
	faceFrame = cvCreateImage(cvSize(rect->width,  rect->height ), frame->depth, frame->nChannels);
	cvCopy(frame, faceFrame);
	cvResetImageROI(frame);
	cvRectangleR(frame, *rect, CV_RGB(255,255,255), 3, 4, 0);
	cvWriteFrame(camVidCamshiftTrack,frame);
	cvSetImageROI(frame, *rect);


	//is that really face?
	faceRect = vjFaceDetect(faceFrame, 70, 70);
	if(!faceRect) return;

	//		time_t tstartVideoFrame, tendVideoFrame, tstartCamShiftFrame, tendCamShiftFrame, tstartQuaAssess, tendQuaAssess;
//  double tSecond, tTotalQuaAssess, tTotalCamShiftFaceDect, tTotalVideoFrame;
	time(&tendCamShiftFrame);
	tSecond = difftime(tendCamShiftFrame,tstartCamShiftFrame);
	tTotalCamShiftFaceDect+=tSecond;
	time(&tstartQuaAssess);

	numoffaceFrame++;
	cvSetImageROI(faceFrame, *faceRect);
	face = cvCreateImage(cvSize(faceRect->width,  faceRect->height ), faceFrame->depth, faceFrame->nChannels);
	cvCopy(faceFrame, face);

	//Resolution
	face_resolution=abs(face->width * face->height);
	printf ("\nRes: %.2f \t", face_resolution);
	outputFile<<"\nRes: "<<face_resolution<<" \t"; 
				
	//Brightness
	face_brightness= knBrightness(face);
	printf("Bright: %.2f \t", face_brightness);
	outputFile<<"Bright: "<<face_brightness<<" \t"; 

	//Sharpness
	face_sharpness= knSharpness(face);
	printf("Sharp: %.2f \t", face_sharpness);
	outputFile<<"Sharp: "<<face_sharpness<<" \t"; 

	//face_pose
	blackWhiteImage=knSegmentation(face);
	knCenterOfMass(blackWhiteImage, centermassx, centermassy);
	centerfacex= face->width /2;
	centerfacey= face->height /2;
	face_pose = knDistance(centermassx,centermassy,centerfacex,centerfacey);
	printf("Pose: %.2f \t\n", face_pose);
	outputFile<<"Pose: "<<face_pose<<" \t\n"; 

	//normalization of quality parameters to select the qualified faces
	//face_resolution threshold: 200*178 = 0 to 310*275 = 1
	if(face_resolution<(180*158)) face_resolution = 0;
	else if(face_resolution>(310*275)) face_resolution = 1;
	else face_resolution = (1.0/((310*275)-(200*178)))*(face_resolution-(200*178));
	printf ("Res: %.2f \t", face_resolution);

	//face_brightness threshold: 80 = 0 to 150 = 1
	if(face_brightness<80) face_brightness = 0;
	else if(face_brightness>150) face_brightness = 1;
	else face_brightness = (1.0/(150-80))*(face_brightness-80);
	printf ("Bright: %.2f \t", face_brightness);

	//face_sharpness threshold: 30 = 0 to 105 = 1
	if(face_sharpness<30) face_sharpness = 0;
	else if(face_sharpness>105) face_sharpness = 1;
	else face_sharpness = (1.0/(105-30))*(face_sharpness-30);
	printf ("Sharp: %.2f \t", face_sharpness);

	//face_pose threshold: 95 = 0 to 30 = 1
	if(face_pose>95) face_pose = 0;
	else if(face_pose<30) face_pose = 1;
	else face_pose = (1.0/(95-30))*(95-face_pose);
	printf ("Pose: %.2f \t\n", face_pose);

	outputFile<<"NormRes: "<<face_resolution<<" \t"; 
	outputFile<<"NormBright: "<<face_brightness<<" \t"; 
	outputFile<<"NormSharp: "<<face_sharpness<<" \t"; 
	outputFile<<"NormPose: "<<face_pose<<" \t\n"; 

	//calculated weighted total score
	//pose = 0.9, sharpness = 1, brightness = 0.7, resolution = 0.5
	resolution_weight = 0.5; sharpness_weight = 1; pose_weight = 0.9; brightness_weight = 0.7;
	total_score = face_resolution*resolution_weight + face_brightness*brightness_weight + face_sharpness*sharpness_weight + face_pose*pose_weight;
	total_score = total_score/(resolution_weight + sharpness_weight + pose_weight + brightness_weight);
	if(face_resolution==0||face_brightness==0||face_sharpness==0||face_pose==0) total_score = 0;
	if(total_score>0.65) {
		numofqulifiedFace++;
//		sprintf(fileName, "./record/%d_%d_Res: %.2f_Pos: %.2f_Shrp: %.2f_Brght: %.2f.mpeg", numoffaceFrame, numofqulifiedFace, face_resolution, face_pose, face_sharpness, face_brightness);
		sprintf(fileName, "./record/%d_%d.mpeg", numoffaceFrame, numofqulifiedFace);
		cvSaveImage(strcat(fileName, ".jpeg"), face);	
	}
	sprintf(fileName, "./record/%d.mpeg", numoffaceFrame);
//	sprintf(fileName, "./record/%d_Res: %.2f_Pos: %.2f_Shrp: %.2f_Brght: %.2f.mpeg", numoffaceFrame, face_resolution, face_pose, face_sharpness, face_brightness);
	cvSaveImage(strcat(fileName, ".jpeg"), face);

	printf ("Total score: %.2f \t", total_score);
	printf ("VideoframeCount: %d \t", videoFrameCount);
	printf ("Camframe: %d \t", numofcamshiftFrame);
	printf ("Faces: %d \t", numoffaceFrame);
	printf ("Qualified faces: %d \t\n", numofqulifiedFace);

	outputFile<<"Total score: "<<total_score<<" \t"; 
	outputFile<<"VideoframeCount: "<<videoFrameCount<<" \t"; 
	outputFile<<"Camframe: "<<numofcamshiftFrame<<" \t"; 
	outputFile<<"Faces: "<<numoffaceFrame<<" \t"; 
	outputFile<<"Qualified faces: "<<numofqulifiedFace<<" \t\n"; 

//		time_t tstartVideoFrame, tendVideoFrame, tstartCamShiftFrame, tendCamShiftFrame, tstartQuaAssess, tendQuaAssess;
//  double tSecond, tTotalQuaAssess, tTotalCamShiftFaceDect, tTotalVideoFrame;
	time(&tendVideoFrame);
	time(&tendQuaAssess);
	tTotalVideoFrame = difftime(tendVideoFrame,tstartVideoFrame);
	tSecond = difftime(tendQuaAssess, tstartQuaAssess);
	tTotalQuaAssess +=tSecond;
	printf ("TimeVideoFrame: %.4f \t", tTotalVideoFrame);
	outputFile<<"TimeVideoFrame: "<<tTotalVideoFrame<<" \t"; 
	printf ("TimeCamshiftFaceDetect: %.4f \t", tTotalCamShiftFaceDect);
	outputFile<<"TimeCamshiftFaceDetect: "<<tTotalCamShiftFaceDect<<" \t"; 
	printf ("TimeQuaAssess: %.4f \t", tTotalQuaAssess);
	outputFile<<"TimeQuaAssess: "<<tTotalQuaAssess<<" \t\n"; 


	outputFile.close();
}

control::~control()
{
	cvReleaseCapture(&cap);
	cvDestroyAllWindows();
}


