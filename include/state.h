
/*!
 *\class state
 *\brief mother class for generic state
 */


 /*!
 *Updated by Mohammad Ahsanul Haque (iamahsanul@gmail.com)
 *Aalborg University, Denmark
 */

 /*!Foundation reference:
 *\author Fayel <loic.fayel@ecole.ensicaen.fr>
 *\author Cochet <stephanie.cochet@ecole.ensicaen.fr>
 */

#ifndef __STATE_H__
#define __STATE_H__

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>

#include <iostream>
#include <string>

#define TRACKING_STATE -1
#define BG_STATE 1

class state
{
	public :
		int id;			/*!< state id*/
		std::string name;	/*!< sate name*/

	public:
		/*!
		 *\brief virtual method for proccessing frame
		 *@param IplImage* : frame to process
		 *\return interger depending on result of algo use ion each state
		 */
		virtual int process(IplImage* tmp_frame)=0;
		/*!
		 *\brief get the id of the state
		 *\return id of the state
		 */
		int getId();
		/*!
		 *\brief get the name of the state
		 *\return name of the state
		 */
		std::string getName();
};

#endif

