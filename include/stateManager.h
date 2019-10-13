
/*!
 *\class stateManager
 *\brief class for state management module
 * stateManager switch between BGsubtraction state (camera fixed) and Camshift tracking state (camera moved)
 * BGsubtraction to tracking : when BGsubtraction return a good target
 * Tracking to BGsubtraction : when the give target is lost
 */


 /*!
 *Updated by Mohammad Ahsanul Haque (iamahsanul@gmail.com)
 *Aalborg University, Denmark
 */

 /*!Foundation reference:
 *\author Fayel <loic.fayel@ecole.ensicaen.fr>
 *\author Cochet <stephanie.cochet@ecole.ensicaen.fr>
 */
 
#ifndef __STATEMANAGER_H__
#define __STATEMANAGER_H__

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>

#include <string>


#include "state.h"
#include "bgState.h"
#include "trackingState.h"

class stateManager
{
	private :
		IplImage*	tmp_frame;	/*!< frame to be processing*/
		state*		active;		/*!< current active state*/
		bgState*	bgSub;		/*!< BGsubtraction state*/
		trackingState*	track;		/*!< tracking state*/
		CvRect*		target;		/*!< current target*/
	public :
		/*!
		 *\brief constructor
		 * build a new instance of stateManager
		 */
		stateManager(IplImage* tmp_frame);
		/*!
		 *\brief initialise \c tmp_frame, \c bgSub and \c track with frame
		 *\param IplImage* : frame to be processing
		 */
		int init(IplImage* tmp_frame);
		/*!
		 *\brief switch state
		 * BGsubtraction to tracking : when BGsubtraction return a good target
		 * Tracking to BGsubtraction : when the give target is lost
		 */
		void changeState();
		/*!
		 *\brief save current target
		 * target is return by BGsubraction state
		 */
		void saveTarget();
		/*!
		 *\brief track target in the current frame
		 *\param IplImage* : frame to be processing
		 */
		void process(IplImage* tmp_frame);
		/*!
		 *\brief return the current target
		 *\return current target in a CvRect* structur
		 */
		CvRect* getTarget();
		/*!
		 *\brief return the current active state id
		 *\return current active state id
		 */
		int getActive();
};

#endif
