/*!
 *\brief methods to manage differents states of the system
 */

 /*!
 *Updated by Mohammad Ahsanul Haque (iamahsanul@gmail.com)
 *Aalborg University, Denmark
 */

 /*!Foundation reference:
 *\author Fayel <loic.fayel@ecole.ensicaen.fr>
 *\author Cochet <stephanie.cochet@ecole.ensicaen.fr>
 */


#include "../include/stateManager.h"

stateManager::stateManager(IplImage* tmp_frame)
{
	init(tmp_frame);
}

int stateManager::init(IplImage* tmp_frame)
{
	bgSub=new bgState(tmp_frame);
	track=new trackingState(tmp_frame);
	active=bgSub;
	return 0;
}

void stateManager::changeState()
{
	std::cout << "state switched - active : ";
	if(active->getId()==BG_STATE)
	{
		saveTarget();
		track->setTarget(target);
		active = track;
		std::cout <<  active->getName() << "(id : " << active->getId() << ")" << std::endl;
	}
	else
	{
		active = bgSub;
		std::cout << active->getName() << "(id : " << active->getId() << ")" << std::endl;
	}
}

void stateManager::saveTarget()
{
	std::cout << "target saved" << std::endl;
	target=bgSub->getTarget();
}

CvRect* stateManager::getTarget()
{
	return target;
}

int stateManager::getActive()
{
	return active->getId();
}

void stateManager::process(IplImage* tmp_frame)
{
	if(active->process(tmp_frame)==1)
	{
		changeState();
	}
	if(active->getId()==BG_STATE)
	{
		cvWaitKey(2);
	}
	else
	{
		cvWaitKey(40);
	}
}
