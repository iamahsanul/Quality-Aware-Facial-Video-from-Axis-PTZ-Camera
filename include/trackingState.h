/*!
*\class trackingState
*\brief class for tracking module
* tracking use camshift algorythm
* state active when camera is moving
* this module is use to track a given target
*/

/*!
 *Updated by Mohammad Ahsanul Haque (iamahsanul@gmail.com)
 *Aalborg University, Denmark
 */

/*!Foundation reference: 
 *\author Fayel <loic.fayel@ecole.ensicaen.fr>
 *\author Cochet <stephanie.cochet@ecole.ensicaen.fr>
 */

#ifndef __TRACKINGSTATE_H__
#define __TRACKINGSTATE_H__

#include "state.h"
#include "histo.h"

#define VMIN 120
#define VMAX 256
#define SMIN 30
#define LOST 40

class trackingState: public state
{
	private :
		CvRect*		target;					/*!< Rectangle containing the target to track*/
		CvConnectedComp	track_comp;				/*!< result of camshift algo*/
		CvBox2D		box;					/*!< camshift param*/
		IplImage	*mask, *hsv, *hue, *imgBackProject;	/*!< img for camshift algo and histogramme calculation*/
		int		vmin, vmax, smin, _vmin, _vmax;		/*!< camshift param*/
		histo*		histogram;				/*!< histogram*/
		int		lost;					/*!< if target is lost*/
	public:
		/*!
		 *\brief constructor
		 * build a new instance of trackingState class
		 * without initialised \c imgBackProject, \c hsv, \c mask and \c hue
		 */
		trackingState();
		/*!
		 *\brief constructor
		 * build a new instance of trackingState class
		 */
		trackingState(IplImage* tmp_frame);
		/*!
		 *\brief initialise \c imgBackProject, \c hsv, \c mask and \c hue with frame size and depth
		 *\param IplImage* : frame to be processing
		 */
		void init(IplImage* tmp_frame);
		/*!
		 *\brief search for the target in the frame
		 *\param IplImage* : frame to be processing
		 *\return 1 if a target is lost more than 30frames
		 * 0 otherwise
		 */
		int process(IplImage* tmp_frame);
		/*!
		 *\brief initialise the target to track
		 *\param: CvRect* rectangle containing the target to track
		 */
		void setTarget(CvRect* tr);
};

#endif
