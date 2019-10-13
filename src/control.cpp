
/*!
*\class control
*\brief main class of tracking control system
*/


/*!
*Author: Mohammad Ahsanul Haque (iamahsanul@gmail.com)
*Aalborg University, Denmark
*/


#include "../include/control.h"

control::control(std::string ip)
{
	/*initialisation des donnees membres
	* creation du httpclient, reccuperation du flux video
	* creation du stateManager
	*/
	ipCam=ip;
	http= new httpClient(ip.c_str());
	http->setInitPos(178,160,200);
	http->goToInit();
	cap=http->getMpg("320x240");
	init(cap);

	/*
	 * lancer le traitement de la video
	 */
	process();
}

control::control(const char* videoFileName)
{
	/*initialisation des donnees membres
	* creation du httpclient, reccuperation du flux video
	* creation du stateManager
	*/
	cap=cvCaptureFromFile(videoFileName);
	init(cap);

	/*
	 * lancer le traitement de la video
	 */
	process();
}

void control::init(CvCapture* cap)
{
	tmp_frame = cvQueryFrame(cap);
	if(!tmp_frame)
	{
		std::cout << "Bad video !" << std::endl;
	}
	stateSwitch = new stateManager(tmp_frame);
}

void control::process()
{
	tmp_frame = cvQueryFrame(cap);
	CvVideoWriter* MaVideo = cvCreateVideoWriter("./record/testTrack.mpeg",CV_FOURCC('M','P','E','G'),30,cvGetSize(tmp_frame),1);
	cvNamedWindow("test_tracking", 1);
	//cvNamedWindow("BG_proj", 1);
	//CvRect bla;
	int xtr=0,ytr=0,ztr=100;
	bool initOK=true;

	for( int fr = 1;tmp_frame; tmp_frame = cvQueryFrame(cap), fr++ )
	{
		stateSwitch->process(tmp_frame);
		if(fr%5==0 && stateSwitch->getActive()==TRACKING_STATE)
		{
			xtr=(int)(stateSwitch->getTarget()->x+(stateSwitch->getTarget()->width/2))*704/320;
			ytr=(int)(stateSwitch->getTarget()->y+(stateSwitch->getTarget()->height/2))*576/240;

			if(stateSwitch->getTarget()->width*stateSwitch->getTarget()->height<10000)
			{
				ztr=105;
			}
			else if(stateSwitch->getTarget()->width*stateSwitch->getTarget()->height>12000)
			{
				ztr=95;
			}
			else
			{
				ztr=100;
			}
			http->setAreaZoom(xtr,ytr,ztr);
			initOK=false;
		}
		else if(!initOK && stateSwitch->getActive()==BG_STATE)
		{
			http->goToInit();
			initOK=true;
		}
		cvShowImage("test_tracking", tmp_frame);
		//cvShowImage("BG_proj", stateSwitch->bgSub->bg_model->foreground);
		cvWriteFrame(MaVideo,tmp_frame);
	}
	cvReleaseVideoWriter(&MaVideo);
}

control::~control()
{
	cvReleaseCapture(&cap);
	cvDestroyAllWindows();
}
