
/*!
 *\class histo
 *\brief class for histogram calculation
 */


 /*!
 *Updated by Mohammad Ahsanul Haque (iamahsanul@gmail.com)
 *Aalborg University, Denmark
 */

 /*!Foundation reference:
 *\author Fayel <loic.fayel@ecole.ensicaen.fr>
 *\author Cochet <stephanie.cochet@ecole.ensicaen.fr>
 */

 
#ifndef __HISTO_H__
#define __HISTO_H__

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>

#include <string>

class histo
{
	private :
		CvHistogram*	hist;				/*!< histogram*/
		float		max_val,hranges_arr[2], *hranges;/*!< histogram calculation param*/
		int		hdims;				/*!< histogram calculation param*/
		bool		end;				/*!< boolean for end of calculation*/
	public :
		/*!
		 *\brief constructor
		 * build a new instance of histo
		 */
		histo();
		/*!
		 *\brief histogram calculation
		 *\param IplImage* : img of which histogram must be calculated
		 *\param IplImage* : img mask
		 *\param CvRect* : region of calculation of histogram
		 */
		void calcHist(IplImage* hue, IplImage* mask, CvRect* rect);
		/*!
		 *\brief stop histogram calculation
		 *\param bool : true if histogram is calculated,
		 * false otherwise
		 */
		void setEndCalc(bool b);
		/*!
		 *\brief get end of histogram calculation
		 *\return true if histogram is calculated,
		 * false otherwise
		 */
		bool endCalc();
		/*!
		 *\brief get histogram
		 *\return histogram
		 */
		CvHistogram* getHist();
};

#endif
