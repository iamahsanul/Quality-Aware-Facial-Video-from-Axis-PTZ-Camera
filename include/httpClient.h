
/*!
 *\class httpClient
 *\brief class for communication with IpCam module
 * Camera used is : 218-PTZ AXIS Ip Cam 
 * communication with camera use : 
 * - Axis Vapix http API version 2.0 (cf. http://www.axis.com/techsup/cam_servers/dev/cam_http_api_2.php)
 * - poco::net library (cf. http://pocoproject.org/)
 */


 /*!
 *Updated by Mohammad Ahsanul Haque (iamahsanul@gmail.com)
 *Aalborg University, Denmark
 */

 /*!Foundation reference:
 *\author Fayel <loic.fayel@ecole.ensicaen.fr>
 *\author Cochet <stephanie.cochet@ecole.ensicaen.fr>
 */


#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPCookie.h>
#include <Poco/Net/NameValueCollection.h>
#include "Poco/Net/HTTPBasicCredentials.h"
#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

#include <map>
#include <string>
#include <iostream>
#include <fstream>

#define OK 1
#define ERR 0

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPBasicCredentials;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPCookie;
using Poco::Net::NameValueCollection;
using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::Exception;

class httpClient
{
	private :
		URI* uri;			/*!< initial uri of the ipCam (ex: \c "http://<ipcam>" or \c "http://<serverName>")*/
		HTTPClientSession* session;	/*!< http session*/
		HTTPRequest* request;		/*!< http request*/
		HTTPResponse* response;		/*!< http response*/
		bool PTZcookie;			/*!< presence of ptz cookie*/
		std::vector<HTTPCookie>* cookies;/*!< vector contining cookies for IpCam ptz control*/
		NameValueCollection* nvc;	/*!< structure to setting httpRequest cookies for IpCam ptz control*/
		int panInit,tiltInit,zoomInit;	/*!< priviligied position of the IpCam*/
 	public:
		/*!
		 *\brief constructor
		 * build a new instance of httpClient with initial uri of the ipCam 
		 * (ex: \c "http://<ipcam>" or \c "http://<serverName>")
		 * class will send to it http request
		 * the Ip adresse is set only in the constructor
		 */
		httpClient(const char* bla);
		/*!
		 *\brief send http Request to the IpCam with parameters
		 *\param std::map<std::string,std::string>* : parameters to send
		 */
		void sendRequest(std::map<std::string,std::string>* param);
		/*!
		 *\brief send http Request to the IpCam
		 *\param std::string : path and query of the http request (ex :"/axis-cgi/view/param.cgi?action=list")
		 */
		void sendRequest(std::string req);
		/*!
		 *\brief send http Request to the IpCam in order to control PTZ
		 *\param std::string : query of the http request (ex :"&pan=180&tilt=180&zoom=100")
		 */
		void sendPTZRequest(std::string req);
		/*!
		 *\brief move ipCam to priviligied position
		 */
		void goToInit();
		/*!
		 *\brief set ipCam to priviligied position
		 *\param int : absolute pan (-180 to 180)
		 *\param int : absolute tilt (-180 to 180)
		 *\param int : absolute zoom (1 to 9999)
		 */
		void setInitPos(int p, int t, int z);
		/*!
		 *\brief get ipCam priviligied position
		 *\param std::string : ipCam priviligied position (pan,tilt,zoom)
		 */
		std::string getInitPos();
		/*!
		 *\brief set ipCam PTZ parameters
		 *\param int : absolute pan (-180 to 180)
		 *\param int : absolute tilt (-180 to 180)
		 *\param int : absolute zoom (1 to 9999)
		 */
		void setPTZ(int x, int y, int z);
		/*!
		 *\brief center IpCam in a given point
		 *\param int : x
		 *\param int : y
		 */
		void setCenter(int x, int y);
		/*!
		 *\brief set ipCam areazoom
		 *\param int : x center
		 *\param int : y center
		 *\param int : zoom (zoom if >100, unzoom if <100, nothing otherwise)
		 */
		void setAreaZoom(int x, int y, int zoom);
		/*!
		 *\brief move ipCam to given direction
		 *\param std::string : direction (up,down,right,left)
		 */
		void setDirection(std::string dir);
		/*!
		 *\brief return the current frame
		 * (JPEG format with default resolution "320x240")
		 *\return current frame (JPEG format) in a IplImage*
		 */
		IplImage* getJpg();
		/*!
		 *\brief return the current frame
		 * (JPEG format with given resolution)
		 *\param std::string : resolution
		 * (possible value : 1280x1024, 1280x960, 1280x720, 768x576, 4CIF, 704x576, 704x480, VGA, 640x480, 640x360, 2CIFEXP, 2CIF, 704x288, 704x240, 480x360, CIF, 384x288, 352x288, 352x240, 320x240, 240x180, QCIF, 192x144, 176x144, 176x120, 160x120)
		 *\return current frame (JPEG format) in a IplImage*
		 */
		IplImage* getJpg(std::string resol);
		/*!
		 *\brief return the current video stream
		 * (MJPG format with default resolution "320x240")
		 *\return current frame (MJPG format) in a CvCapture*
		 */
		CvCapture* getMpg();
		/*!
		 *\brief return the current video stream
		 * (MJPG format with given resolution)
		 *\param std::string : resolution
		 * (possible value : 1280x1024, 1280x960, 1280x720, 768x576, 4CIF, 704x576, 704x480, VGA, 640x480, 640x360, 2CIFEXP, 2CIF, 704x288, 704x240, 480x360, CIF, 384x288, 352x288, 352x240, 320x240, 240x180, QCIF, 192x144, 176x144, 176x120, 160x120)
		 *\return current frame (MJPG format) in a CvCapture*
		 */
		CvCapture* getMpg(std::string resol);
};

#endif
