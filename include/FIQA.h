// Real Time Face Quality Assessment Using the features of the face and facial components

/*!
*Author Mohammad Ahsanul Haque (iamahsanul@gmail.com)
*Aalborg University, Denmark
*/

/*!Foundation reference:
*\author Kamal Nasrollahi <kn@create.aau.dk> */


#include "stdafx.h"

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


// Function to detect and draw any faces that is present in an image
CvSeq* knDetectingFaces( IplImage* img );


float knBrightness(IplImage* img);

float knSharpness (IplImage* img);


IplImage*  knSegmentation(IplImage* img);

void knCenterOfMass (IplImage* bw, long& cmx, long& cmy);

float knDistance(float cmx, float cmy,float cwx,float cwy);

CvSeq* knDetectingEyes( IplImage* img );


// Function to detect and draw any moth that is present in a face image
CvSeq* knDetectingMouth( IplImage* img );


void knMax(float a[], int num_elements, float* max, int* maxIndex);

void knMin(float a[], int num_elements, float* min, int* minIndex);

void knShowManyImages(char* title, int nArgs, ...);

void knShowFaceLog(char* title, int nArgs, ...) ;

// Main function, defines the entry point for the program.
int mainFunctionOfKamal( int argc, char** argv );