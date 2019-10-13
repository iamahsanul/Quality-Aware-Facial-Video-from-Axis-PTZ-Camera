/*!
*\brief methods for histogram calculation
*/

/*!
*Updated by Mohammad Ahsanul Haque (iamahsanul@gmail.com)
*Aalborg University, Denmark
*/

/*!Foundation reference:
*\author Fayel <loic.fayel@ecole.ensicaen.fr>
*\author Cochet <stephanie.cochet@ecole.ensicaen.fr>
*/



#include "../include/histo.h"
#include <iostream>

histo::histo()
{
	max_val = 0.f;
	hdims = 16;
	end=false;
	hranges_arr[0]= 0;
	hranges_arr[1]=180;
	hranges = hranges_arr;
    	hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges,1);
}

void histo::calcHist(IplImage* hue, IplImage* mask, CvRect* rect)
{
		cvSetImageROI(hue,*rect);
		cvSetImageROI(mask,*rect);

		cvCalcHist(&hue, hist, 0, mask);
		cvGetMinMaxHistValue(hist,0,&max_val,0,0);
		cvConvertScale(hist->bins,hist->bins,max_val ? 255. / max_val : 0.,0);

		cvResetImageROI(hue);
		cvResetImageROI(mask);

}

void histo::setEndCalc(bool b)
{
	end=b;
}

bool histo::endCalc()
{
	return end;
}

CvHistogram* histo::getHist()
{
	return hist;
}
