/*!
 *\brief methods for tracking module
 */

 /*!
 *Updated by Mohammad Ahsanul Haque (iamahsanul@gmail.com)
 *Aalborg University, Denmark
 */

 /*!Foundation reference:
 *\author Fayel <loic.fayel@ecole.ensicaen.fr>
 *\author Cochet <stephanie.cochet@ecole.ensicaen.fr>
 */

 
 
#include "../include/trackingState.h"

trackingState::trackingState()
{
	id=TRACKING_STATE;
	name="Camshift";

	vmin = VMIN;
	vmax = VMAX;
	smin = SMIN;
	_vmin = vmin;
	_vmax = vmax;

	histogram = new histo();

	lost=LOST;
}

trackingState::trackingState(IplImage* tmp_frame)
{
	id=TRACKING_STATE;
	name="Camshift";

	vmin = VMIN;
	vmax = VMAX;
	smin = SMIN;
	_vmin = vmin;
	_vmax = vmax;

	histogram = new histo();
	init(tmp_frame);

	lost=LOST;
}

void trackingState::init(IplImage* tmp_frame)
{
    	imgBackProject = cvCreateImage( cvGetSize(tmp_frame), tmp_frame->depth, 1 );
	mask = cvCreateImage( cvGetSize(tmp_frame), tmp_frame->depth, 1 );
	hsv = cvCreateImage(cvGetSize(tmp_frame), tmp_frame->depth, 3 );
	hue = cvCreateImage(cvGetSize(tmp_frame), tmp_frame->depth, 1 );
}

int trackingState::process(IplImage* tmp_frame)
{

	//Start camshift algorythm
	cvCvtColor(tmp_frame,hsv,CV_BGR2HSV);
	_vmin = vmin;
	_vmax = vmax;
	//cvInRangeS( hsv, cvScalar(340,0,0,0),cvScalar(40,255,255,0),mask);
	//cvInRangeS(hsv,cvScalar(0,smin,MIN(_vmin,_vmax),0),cvScalar(180,256,MAX(_vmin,_vmax),0), mask );
	cvInRangeS( hsv, cvScalar(0,smin,MIN(_vmin,_vmax),0),cvScalar(180,256,MAX(_vmin,_vmax),0), mask );
	cvSplit(hsv,hue,0,0,0);

	if(!histogram->endCalc())
	{
		histogram->calcHist(hue,mask,target);
	}

 	cvCalcBackProject(&hue,imgBackProject,histogram->getHist());
	cvAnd(imgBackProject,mask,imgBackProject, 0 );
	//std::cout << "camshift window size: " << target->height << " " << target->width << std::endl;
	cvCamShift( imgBackProject, *target, cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ), &track_comp,&box);
	if(track_comp.rect.height>=0 || track_comp.rect.width>=0)
	{
		*target=track_comp.rect;
	}

	if( !tmp_frame->origin )
        box.angle = -box.angle;

	if(target->width>0 && target->height>0)
	{
		//draw target
		cvRectangle(tmp_frame, cvPoint(target->x,target->y),cvPoint(target->x+target->width,target->y+target->height),cvScalar(0,255,0),1,8,0);
		//cvRectangle(tmp_frame,cvPoint(10,10),cvPoint(200,200),cvScalar(200,200,255),1,8,0);
		histogram->setEndCalc(true);
	}	
	if(target->width*target->height<2000)
	{
		lost--;
		if(lost<1)
		{
			lost=LOST;
			return 1;
		}
	}
	return 0;
}

void trackingState::setTarget(CvRect* tr)
{
	target=tr;
}
