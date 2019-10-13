
/*!
 *\class control
 *\brief main class of tracking system
 */


 /*!
 *Author Mohammad Ahsanul Haque (iamahsanul@gmail.com)
 *Aalborg University, Denmark
 */


#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>

#include <string>

#include "stateManager.h"
#include "httpClient.h"
#include "FIQA.h"

class control
{
	private:
		std::string	ipCam;		/*!< Ip adresse of the ipCam*/
		httpClient*	http;		/*!< http client*/
		stateManager*	stateSwitch;	/*!< state manager*/
		char*		videoFileName;	/*!< video file name*/
		CvCapture*	cap;		/*!< video stream to be processing*/
		IplImage*	tmp_frame;	/*!< current frame*/
		bool		enableTrack;	/*!< enable tracking system*/
	public:
		/*!
		 *\brief constructor
		 * build a new instance of control class with an IpCam
		 *\param std::string : ip adresse or server name of the IpCam
		 */
		control(std::string ip);
		/*!
		 *\brief constructor
		 * build a new instance of control class with a video file name
		 *\param std::string : path of the video file
		 */
		control(const char* videoFileName);
		/*!
		 *\brief initialise \c cap
		 *\param CvCapture* : video to be processing
		 */
		void init(CvCapture* cap);
		/*!
		 *\brief run the trancking system
		 */
		void process();
		/*!
		 *\brief initially detecting face using viola and jones method
		 */
		CvRect* control::vjFaceDetect(IplImage* frame, int cvSizeWidth, int cvSizeHeight);		
		/*!
		 *\brief initially face quality assessment and saving qualified faces
		 */
		void control::faceQulalityAssess(IplImage* frame, CvRect* rect);
		/*!
		 *\brief destructor
		 */
		~control();
};

#endif
