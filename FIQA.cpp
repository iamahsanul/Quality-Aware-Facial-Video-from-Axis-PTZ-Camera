// Real Time Face Quality Assessment Using the features of the face and facial components

/*!
*Author Mohammad Ahsanul Haque (iamahsanul@gmail.com)
*Aalborg University, Denmark
*/

/*!Foundation reference:
*\author Kamal Nasrollahi <kn@create.aau.dk> */


#include "stdafx.h"

// OpenCV Sample Application: facedetect.c

// Include header files
#include <opencv/cv.h>
#include <opencv/highgui.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>


	//max number of frames for local scoring
	 int const max_duration=40;


	// Create memory for calculations
	static CvMemStorage* storage = 0;

	// Create a new Haar classifier
	static CvHaarClassifierCascade* cascade = 0;

	// Create a string that contains the cascade name
	const char* face_cascade_name =
    "C:/Program Files/OpenCV/data/haarcascades/haarcascade_frontalface_alt.xml";

	// Create a new Haar classifier
    CvHaarClassifierCascade* face_cascade =(CvHaarClassifierCascade*)cvLoad( face_cascade_name, 0, 0, 0 );;

	// Create a string that contains the exact cascade name-Eyes
	const char* eyes_cascade_name =
    "C:/Program Files/OpenCV/data/haarcascades/haarcascade_eye.xml";


	// Create memory for calculations_Eyes
    CvMemStorage* e_storage = 0;

    // Create a new Haar classifier eyes
    CvHaarClassifierCascade* eyes_cascade = (CvHaarClassifierCascade*)cvLoad( eyes_cascade_name, 0, 0, 0 );;


	// Create a string that contains the exact cascade name-mouths
	const char* mouth_cascade_name =
    "C:/Program Files/OpenCV/data/haarcascades/haarcascade_mcs_mouth.xml";
	
	// Create memory for calculations
    CvMemStorage* m_storage = 0;

    // Create a new Haar classifier
    CvHaarClassifierCascade* mouth_cascade =(CvHaarClassifierCascade*)cvLoad( mouth_cascade_name, 0, 0, 0 );;
	
	




long bwarea=1;
// Function to detect and draw any faces that is present in an image
CvSeq* knDetectingFaces( IplImage* img )
{
    int scale = 1;

    // Allocate the memory storage
    storage = cvCreateMemStorage(0);

    // Clear the memory storage which was used before
    cvClearMemStorage( storage );

    // Find whether the cascade is loaded, to find the faces. If yes, then:
    if( face_cascade )
    {
        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        return (cvHaarDetectObjects( img, face_cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(40, 40) ));
    }
}


float knBrightness(IplImage* img){
	IplImage* ycbcr = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
	cvCvtColor(img, ycbcr, CV_RGB2YCrCb);
	long sum=0;
	for( int y=0; y<ycbcr->height; y++ ) {
		uchar* ptr = (uchar*) (
		ycbcr->imageData + y * ycbcr->widthStep
		);
		for( int x=0; x<ycbcr->width; x++ ) {
			sum=sum+ptr[3*x+1];
		}
	}
	float face_brightness=sum/ ((ycbcr->width)*(ycbcr->height));
	cvReleaseImage(&ycbcr);
	return abs(face_brightness);
}

float knSharpness (IplImage* img){
	IplImage* lpimg = cvCreateImage(cvGetSize(img), img->depth , img->nChannels );
	float a[] = { 1/9,  1/9, 1/9,
                  1/9,  1/9, 1/9,
                  1/9,  1/9, 1/9};
	CvMat lp = cvMat(3,3,CV_32FC1, a);
	cvFilter2D(img, lpimg,&lp, cvPoint(2,2));
	cvAbsDiff(img, lpimg, img);
	cvAbs(img, img);
	cvReleaseImage(&lpimg);
	if (img->nChannels ==3) 
		return abs((cvAvg(img).val[0] +cvAvg(img).val[1]+cvAvg(img).val[2])/3);
	else
		return abs((cvAvg(img)).val[0]);
}


IplImage*  knSegmentation(IplImage* img){
	IplImage* bw;
	bwarea=1;
	bw = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	if (img->nChannels==3)
		cvCvtColor(img, bw, CV_RGB2GRAY);
	else
		cvCopy(img, bw);

	long thrsh=5, bckg=0, frg=0;
	int nbckg=1, nfrg=1;
	float avgfr=10,avgbk=10;
	
	while (thrsh <=((avgfr+avgbk)/2)){
		nbckg=1; nfrg=1;
		bckg=0; frg=0;
		for( int y=0; y<bw->height; y++ ) {
			uchar* ptr = (uchar*) (
			bw->imageData + y * bw->widthStep
			);
			for( int x=0; x<bw->width; x++ ) {
					if (ptr[x]>= thrsh){
						frg=frg+ptr[x];
						nfrg=nfrg+1;
					}
					else{
						bckg=bckg+ptr[x];
						nbckg=nbckg+1;
					}
			}
		}
		avgfr= frg / nfrg;
		avgbk= bckg / nbckg;
		thrsh=thrsh+1;
	}
	for( int y=0; y<bw->height; y++ ) {
		uchar* ptr = (uchar*) (
		bw->imageData + y * bw->widthStep
		);
		for( int x=0; x<bw->width; x++ ) {
			if (ptr[x]<= thrsh)
				ptr[x]=0;
			else{
				ptr[x]=255;
				bwarea =bwarea+1; // number of ON pixels in the binary image
			}
		}
	}
	return bw;
} 

void knCenterOfMass (IplImage* bw, long& cmx, long& cmy){
	long xmean =0, ymean = 0;
	for( int y=0; y<bw->height  ; y++ ) {
		uchar* ptr = (uchar*) (
		bw->imageData + y * bw->widthStep
		);
		for( int x=0; x<bw->width; x++ ) {
		     ymean = ymean + x*ptr[x];
		     xmean = xmean + y*ptr[x];
		}
	}
	cmx = abs((long)(xmean/(bwarea*255)));// 255 is because bwarea is just the number of ON pixels 
	cmy = abs((long)(ymean/(bwarea*255)));

	return ;
}

float knDistance(float cmx, float cmy,float cwx,float cwy){
	return abs(sqrt((cmx-cwx)*(cmx-cwx)+(cmy-cwy)*(cmy-cwy)));
}

CvSeq* knDetectingEyes( IplImage* img ){
    // Allocate the memory storage
    e_storage = cvCreateMemStorage(0);

    // Clear the memory storage which was used before
    cvClearMemStorage( e_storage );

    // Find whether the cascade is loaded, to find the faces. If yes, then:
    if( eyes_cascade )
    {
        // There can be more than one eyes in a face image. So create a growable sequence of eyes.
        // Detect the objects and store them in the sequence
		return (cvHaarDetectObjects( img, eyes_cascade, e_storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(20, 20)));
     }
}


// Function to detect and draw any moth that is present in a face image
CvSeq* knDetectingMouth( IplImage* img ){
    // Allocate the memory storage
    m_storage = cvCreateMemStorage(0);


    // Clear the memory storage which was used before
    cvClearMemStorage( m_storage );

    // Find whether the cascade is loaded, to find the faces. If yes, then:
    if( mouth_cascade )
    {
        // There can be more than one eyes in a face image. So create a growable sequence of eyes.
        // Detect the objects and store them in the sequence
		return (cvHaarDetectObjects( img, mouth_cascade, m_storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(20, 20)));
     }
}


void knMax(float a[], int num_elements, float* max, int* maxIndex)
{
   int i;
   *max=-32000;
   for (i=0; i<num_elements; i++)
   {
	 if (a[i]>(*max))
	 {
	    *max=a[i];
		*maxIndex= i;
	 }
   }
}

void knMin(float a[], int num_elements, float* min, int* minIndex)
{
   int i;
   *min=32000;
   for (i=0; i<num_elements; i++)
   {
	 if (a[i]<(*min))
	 {
	    *min=a[i];
		*minIndex=i;
	 }
   }
}

void knShowManyImages(char* title, int nArgs, ...) {

    // img - Used for getting the arguments 
    IplImage *img;

    // DispImage - the image in which input images are to be copied
    IplImage *DispImage;

    int size;
    int i;
    int m, n;
    int x, y;

    // w - Maximum number of images in a row 
    // h - Maximum number of images in a column 
    int w, h;

    // scale - How much we have to resize the image
    float scale;
    int max;

    // If the number of arguments is lesser than 0 or greater than 12
    // return without displaying 
 
	if(nArgs <= 0) {
        printf("Number of arguments too small....\n");
        return;
    }
    else if(nArgs > 20) {
        printf("Number of arguments too large....\n");
        return;
    }
    // Determine the size of the image, 
    // and the number of rows/cols 
    // from number of arguments 
/*    else if (nArgs == 1) {
        w = h = 1;
        size = 150;
    }
    else if (nArgs == 2) {
        w = 1; h = 2;
        size = 150;
    }
    else if (nArgs == 3 || nArgs == 4) {
        w = 1; h = 4;
        size = 150;
    }
    else if (nArgs == 5 || nArgs == 6) {
        w = 1; h = 6;
        size = 100;
    }
    else if (nArgs == 7 || nArgs == 8) {
        w = 1; h = 8;
        size = 100;
    }
    else {
        w = 1; h = 10;
        size = 75;
    }

*/
	else{
		w=1; h=8;
		size= 100;
	}

    // Create a new 3 channel image
    DispImage = cvCreateImage( cvSize(100 + size*w, 60 + size*h), 8, 3 );

    // Used to get the arguments passed
    va_list args;
    va_start(args, nArgs);

    // Loop for nArgs number of arguments
    for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size)) {

        // Get the Pointer to the IplImage
        img = va_arg(args, IplImage*);

        // Check whether it is NULL or not
        // If it is NULL, release the image, and return
        if(img == 0) {
            printf("Invalid arguments");
            cvReleaseImage(&DispImage);
            return;
        }

        // Find the width and height of the image
        x = img->width;
        y = img->height;

        // Find whether height or width is greater in order to resize the image
        max = (x > y)? x: y;

        // Find the scaling factor to resize the image
        scale = (float) ( (float) max / size );

        // Used to Align the images
        if( i % w == 0 && m!= 20) {
            m = 20;
            n+= 20 + size;
        }

        // Set the image ROI to display the current image
        cvSetImageROI(DispImage, cvRect(m, n, (int)( x/scale ), (int)( y/scale )));

        // Resize the input image and copy the it to the Single Big Image
        cvResize(img, DispImage);

        // Reset the ROI in order to display the next image
        cvResetImageROI(DispImage);
    }

    // Create a new window, and show the Single Big Image
    cvNamedWindow( title, 1 );
    cvShowImage( title, DispImage);

    //cvWaitKey();
    //cvDestroyWindow(title);

    // End the number of arguments
    va_end(args);

    // Release the Image Memory
    cvReleaseImage(&DispImage);
}

void knShowFaceLog(char* title, int nArgs, ...) {

    // img - Used for getting the arguments 
    IplImage *img;

    // DispImage - the image in which input images are to be copied
    IplImage *DispImage;

    int size;
    int i;
    int m, n;
    int x, y;

    // w - Maximum number of images in a row 
    // h - Maximum number of images in a column 
    int w, h;

    // scale - How much we have to resize the image
    float scale;
    int max;

    // If the number of arguments is lesser than 0 or greater than 12
    // return without displaying 
 
	if(nArgs <= 0) {
        printf("Number of arguments too small....\n");
        return;
    }
    else if(nArgs > 50) {
        printf("Number of arguments too large....\n");
        return;
    }
    // Determine the size of the image, 
    // and the number of rows/cols 
    // from number of arguments 
	else{
		w=75; h=0.35;
		size= 20;
	}

    // Create a new 3 channel image
    DispImage = cvCreateImage( cvSize(100 + size*w, 60 + size*h), 8, 3 );

    // Used to get the arguments passed
    va_list args;
    va_start(args, nArgs);

    // Loop for nArgs number of arguments
    for (i = 0, m = 5, n = 5; i < nArgs; i++, m += (5 + size)) {

        // Get the Pointer to the IplImage
        img = va_arg(args, IplImage*);

        // Check whether it is NULL or not
        // If it is NULL, release the image, and return
        if(img == 0) {
            printf("Invalid arguments");
            cvReleaseImage(&DispImage);
            return;
        }

        // Find the width and height of the image
        x = img->width;
        y = img->height;

        // Find whether height or width is greater in order to resize the image
        max = (x > y)? x: y;

        // Find the scaling factor to resize the image
        scale = (float) ( (float) max / size );

        // Used to Align the images
        if( i % w == 0 && m!= 5) {
            m = 5;
            n+= 5 + size;
        }

        // Set the image ROI to display the current image
        cvSetImageROI(DispImage, cvRect(m, n, (int)( x/scale ), (int)( y/scale )));

        // Resize the input image and copy the it to the Single Big Image
        cvResize(img, DispImage);

        // Reset the ROI in order to display the next image
        cvResetImageROI(DispImage);
    }

    // Create a new window, and show the Single Big Image
    cvNamedWindow( title, 1 );
    cvShowImage( title, DispImage);

    //cvWaitKey();
    //cvDestroyWindow(title);

    // End the number of arguments
    va_end(args);

    // Release the Image Memory
    cvReleaseImage(&DispImage);
}


// Main function, defines the entry point for the program.
int mainFunctionOfKamal( int argc, char** argv )
{
	IplImage *frame, *face, *bw, *half, *eye, *mouth, *b1, *b2, *b3, *b4, *b5, *b6, *b7, *xbe, *b[50], *bf[50];
    // Create two points to represent the face locations
	CvPoint pt1, pt2;
	int i,j, scale=1, c=0, numbest;
	CvSeq *faces, *eyes, *mouths;
	CvRect *r, *rec; //r is the Rec for eyes and rec is the Rec for eyes and mouth
	CvRect eaop;//Eye boundary after Opening
	CvRect maop;//Mouth boundary after Opening
	long cmx=0, cmy=0, cwx=0, cwy=0, cmlex=0, cmley=0, cmrex=0, cmrey=0;
	
	int number_of_faces=1, number_of_bests=0, number_of_faces_in_log=0;

	float face_resolution[max_duration], face_resolution_max=1;
	float face_brightness[max_duration], face_sharpness[max_duration], face_yaw[max_duration], face_roll[max_duration],
	      leye_openness[max_duration],reye_openness[max_duration], leye_yaw[max_duration],
		  reye_yaw[max_duration], mouth_openness[max_duration], mouthness[max_duration];
	
	IplConvKernel* m_pSE = cvCreateStructuringElementEx( 3, 3, 1, 1,CV_SHAPE_RECT, NULL );
	
	float face_brightness_max, face_sharpness_max, face_yaw_min, face_roll_min, leye_openness_max,
		  reye_openness_max, leye_yaw_min, reye_yaw_min, mouth_openness_min, mouthness_max;

	
	char buf[10];
	
	float scores[max_duration], bestScore;
	
	int maxIndex=0, minIndex=0;

	//scores initialization
	for (int k=0; k<max_duration-1; k++){
		face_resolution[k]=0;
		face_brightness[k]= 0;
		face_sharpness[k]= 0;
		face_yaw[k]=0;
		face_roll[k]=0;
		leye_openness[k]= 0;
		reye_openness[k]=0;
		leye_yaw[k]=0;
		reye_yaw[k]=0;
		mouth_openness[k]=0;
		mouthness[k]=0;
		scores[k]=0;
	}

	// Check whether the cascade has loaded successfully. Else report and error and quit
    if( !face_cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier face_cascade\n" );
        return 0;
    }
	
	// Check whether the cascade has loaded successfully. Else report and error and quit
    if( !eyes_cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier eyes_cascade\n" );
        return 0;
    }

	// Check whether the cascade has loaded successfully. Else report and error and quit
    if( !mouth_cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier mouth_cascade\n" );
        return 0;
    }
		
	//printf("Resolution \t Brightness \t Sharpness \t face_yaw \t face_roll \t \n");
	cvNamedWindow( "Sequence", 1 );
	cvNamedWindow("Face", 1);
	cvNamedWindow("Segmented", 1);
	cvNamedWindow("Local Key-Frames", 1);
	cvNamedWindow("Face-Log", 1);
	
	//CvCapture* capture = cvCreateCameraCapture(0);
	//CvCapture* capture = cvCreateFileCapture( "D:/Data/a11.avi");
	//CvCapture* capture = cvCreateFileCapture("D:/My works/face data/data1/video 5re.AVI");
//	CvCapture* capture = cvCreateFileCapture("C:\Users\admt\Desktop\VC Practice\Face Detection\FaceLogWithQualityAssess_Kamal/03-2.avi");
	CvCapture* capture = cvCreateFileCapture("03-2.avi");
	//CvCapture* capture = cvCreateFileCapture( "D:/My works/2010 EURASIP IS/data/Videos/10MyVideo.avi");
	//CvCapture* capture = cvCreateFileCapture( "D:/My works/2010 IJIG/data/res Video 3.wmv.avi");

	
	while(1) {
		frame = cvQueryFrame( capture );
		//frame= cvLoadImage("D:/My works/IEEE/cam/3.jpg");
		
		if (!frame) return 0;
		else{
			//*************** video2
			cvSaveImage("temp.jpeg", frame);
			frame = cvLoadImage("temp.jpeg");

			faces=knDetectingFaces(frame);
			// Loop the number of faces found.
			//Just for one face, for more remove the following for comment 
			for( i = 0; i < (faces ? faces->total : 0); i++ )
			{

				// Create a new rectangle for drawing the face
				r = (CvRect*)cvGetSeqElem( faces, i );

				// Find the dimensions of the face,and scale it if necessary
				pt1.x = r->x*scale;
				pt2.x = (r->x+r->width)*scale;
				pt1.y = r->y*scale;
				pt2.y = (r->y+r->height)*scale;

				// Draw the rectangle in the input image
				cvRectangle( frame, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );

				//Go for the features of each face
				cvSetImageROI(frame, *r);
				face = cvCreateImage(cvSize(r->width,  r->height ),frame->depth, frame->nChannels  );
				cvCopy(frame, face);
				
			    cvSaveImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"), face);
			

				//Resolution
				face_resolution[number_of_faces]=abs(face->width * face->height);
				printf ("%.2f \t", face_resolution[number_of_faces]);
				
				//Brightness
				face_brightness[number_of_faces]= knBrightness(face);
				printf("%.2f \t", face_brightness[number_of_faces]);

				//Sharpness
				face_sharpness[number_of_faces]= knSharpness(face);
				printf("%.2f \t", face_sharpness[number_of_faces]);

				
				bw=knSegmentation(face);
				cvShowImage("Segmented", bw);


				//face_yaw
				knCenterOfMass(bw, cmx, cmy);
				cwx= face->width /2;
				cwy= face->height /2;
				face_yaw[number_of_faces]= knDistance(cmx,cmy,cwx,cwy);
				printf("%.2f \t", face_yaw[number_of_faces]);
				
	
				//Eyes
				cvSetImageROI(face, cvRect(0, 0, face->width, face->height/2));
				half = cvCreateImage(cvSize(face->roi->width,  face->roi->height ),face->depth , face->nChannels );
				cvCopy(face, half);
				//cvWaitKey(1);

			
				eyes=knDetectingEyes(half);
				// Loop the number of eyes found.
				//for( j = 0; j < (eyes ? eyes->total : 0); j++ )
				if ((eyes->total)>0){
					for( j = 0; j <2; j++ )
					{ 
						// Create a new rectangle for drawing the eyes
						rec = (CvRect*)cvGetSeqElem( eyes, j );
						
						// Find the dimensions of the eyes,and scale it if necessary
						pt1.x = rec->x*scale;
						pt2.x = (rec->x+rec->width)*scale;
						pt1.y = rec->y*scale;
						pt2.y = (rec->y+rec->height)*scale;
						
						// Draw the rectangle on the eyes image
						//cvRectangle( frame, pt1, pt2, CV_RGB(0,255,0), 3, 8, 0 );
						cvSetImageROI(half, *rec);
						eye = cvCreateImage(cvSize(rec->width,  rec->height ),half->depth, half->nChannels  );
						cvCopy(half, eye);
						cvResetImageROI(half);
						if (j==0){ // left eye
							bw=knSegmentation(eye);
							knCenterOfMass(bw, cmlex, cmley);
							leye_yaw[number_of_faces]=knDistance(cmlex,cmley,eye->width/2,eye->height/2);
							// Left Eye's Yaw
							printf("%.2f \t", leye_yaw[number_of_faces]);
							
							//cvWaitKey(1);

							// Left Eye's Openness
							cvMorphologyEx(bw, bw, NULL,m_pSE, CV_MOP_OPEN, 1);
							eaop= cvBoundingRect(bw, 1);
							leye_openness[number_of_faces]=eaop.height /eaop.width;
							printf("%.2f \t",  leye_openness[number_of_faces]);
						}
						if (j==1){ // right eye
							bw=knSegmentation(eye);
							knCenterOfMass(bw, cmrex, cmrey);
							reye_yaw[number_of_faces]=knDistance(cmrex,cmrey,eye->width/2,eye->height/2);
							//Right Eyes Yaw
							printf("%.2f \t", reye_yaw);
							//cvShowImage("dst", bw);							
							//Right Eye's Openness
							cvMorphologyEx(bw, bw, NULL,m_pSE, CV_MOP_OPEN, 1);
							eaop= cvBoundingRect(bw, 1);
							reye_openness[number_of_faces]= eaop.height /eaop.width ;
							printf("%.2f \t", reye_openness[number_of_faces]);
							if ((cmrex-cmlex)==0)
								face_roll[number_of_faces]= cmrey-cmley; //
							else
								face_roll[number_of_faces]= (cmrey-cmley)/(cmrex-cmlex);
						}
						//it should be here coz if the second eye can not be found there should be some value for this feature
						printf("%.2f \t", face_roll);
					}
					if (eye) cvReleaseImage(&eye);
				}else{
					leye_yaw[number_of_faces]=0;
					leye_openness[number_of_faces]=0;
					reye_yaw[number_of_faces]=0;
					reye_openness[number_of_faces]=0;
					face_roll[number_of_faces]=0;
				}
				cvResetImageROI(face);
				//End of eyes
				cvReleaseMemStorage(&eyes->storage);
				cvReleaseImage(&half);
				
				//mouths	
				
				cvSetImageROI(face, cvRect(0, face->height/2 , face->width, face->height));
				half = cvCreateImage(cvSize(face->roi->width,  face->roi->height ),face->depth , face->nChannels );
				cvCopy(face, half);
				//cvShowImage( "dst", half );
				//cvWaitKey(1);
				
				mouths=knDetectingMouth(half);
				// Loop the number of mouths found.
				//for( j = 0; j < (mouths ? mouths->total : 0); j++ )
				if ((mouths->total)>0){
					for( j = 0; j <1; j++ )
					{
						// Create a new rectangle for drawing the face
						rec = (CvRect*)cvGetSeqElem( mouths, j );

						// Find the dimensions of the mouths,and scale it if necessary
						pt1.x = rec->x*scale;
						pt2.x = (rec->x+rec->width)*scale;
						pt1.y = (rec->y+half->height )*scale;
						pt2.y = (rec->y+rec->height+half->height )*scale;

						// Draw the rectangle on the founded mouths(s)
						//cvRectangle( frame, pt1, pt2, CV_RGB(0,0,255), 3, 8, 0 );
						
						//mouth openness
						cvSetImageROI(half, *rec);
						mouth = cvCreateImage(cvSize(rec->width,  rec->height ),half->depth, half->nChannels  );
						cvCopy(half, mouth);
						cvResetImageROI(half);
						bw=knSegmentation(mouth);
						
						//mouthness
						mouthness[number_of_faces]=bwarea/(bw->width*bw->height);
						printf("%.2f \t", mouthness[number_of_faces]);

					    cvMorphologyEx(bw, bw, NULL,m_pSE, CV_MOP_CLOSE, 1);
						maop= cvBoundingRect(bw, 1);
						mouth_openness[number_of_faces]= maop.height /maop.width;
						printf("%.2f \t", mouth_openness[number_of_faces]);
					}
					if (mouth) cvReleaseImage(&mouth);
				}else{
					mouthness[number_of_faces]=0;
					mouth_openness[number_of_faces]=0;
				}
				cvReleaseMemStorage(&mouths->storage);
				//End of mouths

				cvResetImageROI(face);
				cvResetImageROI(frame);
				cvShowImage("Face", face);
				
				if (number_of_faces==1){
								bf[1]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));
								knShowFaceLog("Face-Log",1,bf[1]);
							}else if (number_of_faces==2){
								bf[2]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));
								knShowFaceLog("Face-Log",2,bf[1], bf[2]);
							}else if (number_of_faces==3){
								bf[3]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",3, bf[1], bf[2], bf[3]);
							}else if (number_of_faces==4){
								bf[4]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",4, bf[1], bf[2], bf[3], bf[4]);
							}else if (number_of_faces==5){
								bf[5]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",5, bf[1], bf[2], bf[3], bf[4], bf[5]);
							}else if (number_of_faces==6){
								bf[6]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",6, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6]);
							}else if (number_of_faces==7){
								bf[7]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",7, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7]);
							}else if (number_of_faces==8){
								bf[8]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",8, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8]);
							}else if (number_of_faces==9){
								bf[9]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",9, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9]);
							}else if (number_of_faces==10){
								bf[10]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",10, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9], bf[10]);
							}else if (number_of_faces==11){
								bf[11]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",11, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9], bf[10], bf[11]);
							}else if (number_of_faces==12){
								bf[12]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",12, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9], bf[10], bf[11], bf[12]);
							}else if (number_of_faces==13){
								bf[13]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",13, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14]);
							}else if (number_of_faces==14){
								bf[14]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",14, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15]);
							}else if (number_of_faces==15){
								bf[15]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",15, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15]);
							}else if (number_of_faces==16){
								bf[16]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",16, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16]);
							}else if (number_of_faces==17){
								bf[17]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",17, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17]);
							}else if (number_of_faces==18){
								bf[18]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",18, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18]);
							}else if (number_of_faces==19){
								bf[19]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",19, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19]);
							}else if (number_of_faces==20){
								bf[20]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",20, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20]);
							}else if (number_of_faces==21){
								bf[21]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",21, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21]);
							}else if (number_of_faces==22){
								bf[22]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",22, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22]);
							}else if (number_of_faces==23){
								bf[23]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",23, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23]);
							}else if (number_of_faces==24){
								bf[24]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",24, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24]);
							}else if (number_of_faces==25){
								bf[25]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",25, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25]);
							}else if (number_of_faces==26){
								bf[26]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",26, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26]);
							}else if (number_of_faces==27){
								bf[27]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",27, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27]);
							}else if (number_of_faces==28){
								bf[28]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",28, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28]);
							}else if (number_of_faces==29){
								bf[29]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",29, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29]);
							}else if (number_of_faces==30){
								bf[30]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",30, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30]);
							}else if (number_of_faces==31){
								bf[31]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",31, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31]);
							}else if (number_of_faces==32){
								bf[32]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",32, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32]);
							}else if (number_of_faces==33){
								bf[33]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",33, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33]);
							}else if (number_of_faces==34){
								bf[34]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",34, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34]);
							}else if (number_of_faces==35){
								bf[35]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",35, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35]);
							}else if (number_of_faces==36){
								bf[36]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",36, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36]);
							}else if (number_of_faces==37){
								bf[37]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",37, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37]);
							}else if (number_of_faces==38){
								bf[38]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",38, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38]);
							}else if (number_of_faces==39){
								bf[39]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",39, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38], bf[39]);
							}else if (number_of_faces==40){
								bf[40]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",40, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38], bf[39], bf[40]);
							}else if (number_of_faces==41){
								bf[41]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",41, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38], bf[39], bf[40], bf[41]);
							}else if (number_of_faces==42){
								bf[42]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",42, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38], bf[39], bf[40], bf[41], bf[42]);
							}else if (number_of_faces==43){
								bf[43]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",43, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38], bf[39], bf[40], bf[41], bf[42], bf[43]);
							}else if (number_of_faces==44){
								bf[44]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",44, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38], bf[39], bf[40], bf[41], bf[42], bf[43], bf[44]);
							}else if (number_of_faces==45){
								bf[45]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",45, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38], bf[39], bf[40], bf[41], bf[42], bf[43], bf[44], bf[45]);
							}else if (number_of_faces==46){
								bf[46]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",46, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38], bf[39], bf[40], bf[41], bf[42], bf[43], bf[44], bf[45], bf[46]);
							}else if (number_of_faces==47){
								bf[47]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",47, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38], bf[39], bf[40], bf[41], bf[42], bf[43], bf[44], bf[45], bf[46], bf[47]);
							}else if (number_of_faces==48){
								bf[48]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",48, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38], bf[39], bf[40], bf[41], bf[42], bf[43], bf[44], bf[45], bf[46], bf[47],bf[48]);
							}else if (number_of_faces==49){
								bf[49]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",49, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38], bf[39], bf[40], bf[41], bf[42], bf[43], bf[44], bf[45], bf[46], bf[47],bf[48], bf[49]);
							}else if (number_of_faces==50){
								bf[50]=cvLoadImage(strcat(itoa(number_of_faces, buf, 10), ".jpeg"));				
								knShowFaceLog("Face-Log",50, bf[1], bf[2], bf[3], bf[4], bf[5], bf[6], bf[7], bf[8], bf[9],bf[10], bf[11], bf[12], bf[13], bf[14], bf[15], bf[16], bf[17], bf[18], bf[19], bf[20], bf[21], bf[22],bf[23], bf[24], bf[25], bf[26], bf[27], bf[28], bf[29], bf[30], bf[31], bf[32], bf[33], bf[34],bf[35], bf[36], bf[37], bf[38], bf[39], bf[40], bf[41], bf[42], bf[43], bf[44], bf[45], bf[46], bf[47],bf[48], bf[49], bf[50]);
							}

				
				if (number_of_faces==max_duration-2){
					knMax(face_resolution, max_duration, &face_resolution_max, &maxIndex);
					knMax(face_brightness, max_duration, &face_brightness_max, &maxIndex);
					knMax(face_sharpness, max_duration,&face_sharpness_max, &maxIndex);
					knMin(face_yaw, max_duration, &face_yaw_min, &minIndex);
					knMin(face_roll, max_duration, &face_roll_min, &minIndex);
					knMax(leye_openness, max_duration,&leye_openness_max,&maxIndex);
					knMax(leye_openness, max_duration, &leye_openness_max, &maxIndex);
					knMax(leye_openness, max_duration, &leye_openness_max, &maxIndex);
					knMax(reye_openness, max_duration, &reye_openness_max, &maxIndex);
					knMin(leye_yaw, max_duration, &leye_yaw_min, &minIndex);
					knMin(reye_yaw, max_duration, &reye_yaw_min, &minIndex);
					knMin(mouth_openness, max_duration, &mouth_openness_min, &minIndex);
					knMax(mouthness, max_duration, &mouthness_max, &maxIndex);
					for (int k=0; k<number_of_faces; k++){
						face_resolution[k]=face_resolution[k]/face_resolution_max;
						face_brightness[k]= face_brightness[k]/face_brightness_max;
						face_sharpness[k]= face_sharpness[k]/face_sharpness_max;
						face_yaw[k]=face_yaw_min/face_yaw[k];
						face_roll[k]=face_roll_min/face_roll[k];
						leye_openness[k]= leye_openness[k]/leye_openness_max;
						reye_openness[k]=reye_openness[k]/reye_openness_max;
						leye_yaw[k]=leye_yaw_min/leye_yaw[k];
						reye_yaw[k]=reye_yaw_min/reye_yaw[k];
						mouth_openness[k]= mouth_openness_min/mouth_openness[k];
						mouthness[k]=mouthness[k]/mouthness_max;
						scores[k]=(face_resolution[k]+face_brightness[k]+face_sharpness[k]+face_yaw[k]+face_roll[k]+
						leye_openness[k]+reye_openness[k]+leye_yaw[k]+reye_yaw[k]+mouth_openness[k]+mouthness[k])/11;
					}

					knMax(scores, max_duration, &bestScore, &maxIndex);
					++number_of_bests;
/*		
					if (number_of_bests>=1){
						char bname[100]="";
						for (numbest=1; numbest<=number_of_bests; numbest++ ){
							strcat(bname, "b[");
							strcat(bname, itoa(numbest, buf, 10));
							strcat(bname, "]");
							if (numbest<=number_of_bests){
								strcat(bname, ", ");
								b[numbest]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));
							}else{
							b[numbest]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));
							}
						}								
					knShowManyImages("Local Key-Frames", numbest, bname);
					}
*/
					if (number_of_bests==1){
						b[1]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));
						knShowManyImages("Local Key-Frames",1,b[1]);
					}else if (number_of_bests==2){
						b[2]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));
						knShowManyImages("Local Key-Frames",2,b[1], b[2]);
					}else if (number_of_bests==3){
						b[3]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",3, b[1], b[2], b[3]);
					}else if (number_of_bests==4){
						b[4]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",4, b[1], b[2], b[3], b[4]);
					}else if (number_of_bests==5){
						b[5]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",5, b[1], b[2], b[3], b[4], b[5]);
					}else if (number_of_bests==6){
						b[6]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",6, b[1], b[2], b[3], b[4], b[5], b[6]);
					}else if (number_of_bests==7){
						b[7]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",7, b[1], b[2], b[3], b[4], b[5], b[6], b[7]);
					}
					/*
					else if (number_of_bests==8){
						b[8]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",8, b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8]);
					}else if (number_of_bests==9){
						b[9]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",9, b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9]);
					}else if (number_of_bests==10){
						b[10]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",10, b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9], b[10]);
					}else if (number_of_bests==11){
						b[11]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",11, b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9],b[10], b[11]);
					}else if (number_of_bests==12){
						b[12]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",12, b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9],b[10], b[11], b[12]);
					}else if (number_of_bests==13){
						b[13]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",13, b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9],b[10], b[11], b[12], b[13]);
					}else if (number_of_bests==14){
						b[14]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",14, b[1], b[2], b[3], b[4], b[5], b[6], b[7],  b[8], b[9],b[10], b[11], b[12], b[13], b[14]);
					}else if (number_of_bests==15){
						b[15]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",15, b[1], b[2], b[3], b[4], b[5], b[6], b[7],  b[8], b[9],b[10], b[11], b[12], b[13], b[14],b[15]);
					}else if (number_of_bests==16){
						b[16]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",16, b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9],b[10], b[11], b[12], b[13], b[14],b[15], b[16]);
					}else if (number_of_bests==17){
						b[17]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",17, b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9],b[10], b[11], b[12], b[13], b[14],b[15], b[16], b[17]);
					}else if (number_of_bests==18){
						b[18]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",18, b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9],b[10], b[11], b[12], b[13], b[14],b[15], b[16], b[17], b[18]);
					}else if (number_of_bests==19){
						b[19]=cvLoadImage(strcat(itoa(maxIndex, buf, 10), ".jpeg"));				
						knShowManyImages("Local Key-Frames",19, b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9],b[10], b[11], b[12], b[13], b[14],b[15], b[16], b[17], b[18], b[19]);
					}*/
					if (face) cvReleaseImage(&face);
					if (bw) cvReleaseImage(&bw);
					if (half) cvReleaseImage(&half);
					number_of_faces=0;
				}
				//
				printf("\n");
				++number_of_faces;
				++number_of_faces_in_log;	
			}
			
			cvShowImage( "Sequence", frame );
			cvReleaseImage(&frame);
			cvReleaseMemStorage(&faces->storage);
			c=cvWaitKey(10);
			if( c == 27 ) //return 0;
			{

				if( c == 27 ) return 0;
			}
		}

	}//end of while for the video
	
}

