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

 
#include "../include/httpClient.h"

httpClient::httpClient(const char* bla)
{
	PTZcookie=false;
	panInit=0;
	tiltInit=180;
	zoomInit=1;

	uri=new URI(bla);
	std::string path(uri->getPathAndQuery());
	if (path.empty()) path = "/";

	session=new HTTPClientSession(uri->getHost(), uri->getPort());

	response= new HTTPResponse();

	request=new HTTPRequest(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
	request->setHost(uri->getHost());
	HTTPBasicCredentials cred("root","MiviaLAB");
	cred.authenticate(*request);
	request->set("User-Agent", "Mozilla/5.0 (X11; U; Linux i686; fr; rv:1.9.2.3) Gecko/20100423 Ubuntu/10.04 (lucid) Firefox/3.6.3");
	request->set("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	request->set("Accept-Language","fr,en-us;q=0.7,en;q=0.3");
	request->set("Accept-Encoding","gzip,deflate");
	request->set("Accept-Charset","ISO-8859-1,utf-8;q=0.7,*;q=0.7");
	request->set("Connection","keep-alive");
	//request->set("Cookie","ptz_ctl_id=41881");

	sendRequest(path);
	if(response->getStatus()==200)
	{
		std::cout << "Camera -" << uri->getHost() << "- available" << std::endl;
	}
	else
	{
		std::cout << response->getStatus() << " " << response->getReason() << std::endl;
	}
}

void httpClient::sendRequest(std::map<std::string,std::string>* param)
{
	std::string req="axis-cgi/com/ptz.cgi?";
	std::map<std::string,std::string>::iterator idx_param;
	for(idx_param=param->begin();idx_param!=param->end();idx_param++)
	{
		req+=idx_param->first+"="+idx_param->second+"&";
	}
	sendRequest(req);
}

void httpClient::sendRequest(std::string req)
{
	try
	{
		request->setURI(req);

		session->sendRequest(*request);

		session->receiveResponse(*response);
	}
	catch (Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
	}
}

void httpClient::sendPTZRequest(std::string req)
{
	try
	{
		/*http request*/
		request->setURI("/axis-cgi/com/ptz.cgi?camera=1"+req);

		/*get ptz cookies and complete http request*/
		if(!PTZcookie)
		{
			cookies = new std::vector<HTTPCookie>;
			session->sendRequest(*request);
			session->receiveResponse(*response);
			response->getCookies(*cookies);
			PTZcookie=true;
			NameValueCollection nvc;
			std::vector<HTTPCookie>::iterator it = cookies->begin();
			while(it!=cookies->end())
			{
				nvc.add((*it).getName(), (*it).getValue());
				it++;
			}
			request->setCookies(nvc);
		}
		/*send full http request*/
		session->sendRequest(*request);
		session->receiveResponse(*response);
	}
	catch (Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
	}
}

void httpClient::goToInit()
{
	std::stringstream out;
	out << "&pan=" << panInit << "&tilt=" << tiltInit << "&zoom=" << zoomInit;
	std::string req=out.str();

	sendPTZRequest(req);

	if(response->getStatus()==204)
	{
		std::cout << "Move OK" << std::endl;
	}
	else
	{
		std::cout << response->getStatus() << " " << response->getReason() << std::endl;
	}
}

void httpClient::setInitPos(int p, int t, int z)
{
	panInit=p;
	tiltInit=t;
	zoomInit=z;
}

std::string httpClient::getInitPos()
{
	std::stringstream out;
	out << "(" << panInit << "," << tiltInit << "," << zoomInit << ")";
	std::string initPos=out.str();
	return initPos;
}

void httpClient::setPTZ(int x, int y, int z)
{

	std::stringstream out;
	out << "pan=" << x << "&tilt=" << y << "&zoom=" << z;
	std::string req="&"+out.str();

	sendPTZRequest(req);

	if(response->getStatus()==204)
	{
		std::cout << "Move OK - pan=" << x << ",tilt=" << y << ",zoom=" << z << std::endl;
	}
	else
	{
		std::cout << response->getStatus() << " " << response->getReason() << std::endl;
	}
}

void httpClient::setCenter(int x, int y)
{
	std::string s;
	std::stringstream out;
	out << x << "," << y;
	s = out.str();
	std::string req="&center="+s;

	sendPTZRequest(req);

	if(response->getStatus()==204)
	{
		std::cout << "Move OK - center : " << s << std::endl;
	}
	else
	{
		std::cout << response->getStatus() << " " << response->getReason() << std::endl;
	}
}

void httpClient::setAreaZoom(int x, int y, int zoom)
{
	std::string s;
	std::stringstream out;
	out << x << "," << y << "," << zoom;
	s = out.str();
	std::string req="&areazoom="+s;
	sendPTZRequest(req);
	if(response->getStatus()==204)
	{
		std::cout << "Move OK - areaZoom : " << s << std::endl;
	}
	else
	{
		std::cout << response->getStatus() << " " << response->getReason() << std::endl;
	}
}

void httpClient::setDirection(std::string dir)
{
	std::string req="&move="+dir;
	sendPTZRequest(req);
	if(response->getStatus()==204)
	{
		std::cout << "Move OK" << std::endl;
	}
	else
	{
		std::cout << response->getStatus() << " " << response->getReason() << std::endl;
	}
}

IplImage* httpClient::getJpg()
{
	IplImage* img = getJpg("320x240");
	return img;
}

IplImage* httpClient::getJpg(std::string resol)
{
	std::stringstream out;
	out << "http://"+uri->getHost()+"/axis-cgi/jpg/image.cgi?resolution="+resol;
	std::string fileName=out.str();
	IplImage* img = cvLoadImage(fileName.c_str());
	return img;
}

CvCapture* httpClient::getMpg()
{
	CvCapture* record = getMpg("320x240");
	return record;
}

CvCapture* httpClient::getMpg(std::string resol)
{
	std::stringstream out;
	out << "http://root:MiviaLAB@"+uri->getHost()+"/axis-cgi/mjpg/video.cgi?resolution="+resol;
	std::string fileName=out.str();
	CvCapture* record = cvCaptureFromFile(fileName.c_str());
	return record;
}
