/*!
 *\author Mohammad Ahsanul Haque (iamahsanul@gmail.com)
 *\brief main
 */

#include "../include/control.h"

int main()
{
	
	int menu=0;
	
	while(menu==0)
	{
		std::cout << "Ip camera : type 1" << std::endl;
		std::cout << "Video file : type 2" << std::endl;
		std::cin >> menu;
	}
	if(menu==1)
	{
		std::string ipCam="";
		std::cout << "Please specify the IP adress of the camera (d for default cam):" << std::endl;
		std::cin >> ipCam;
//		if(ipCam=="d") ipCam="172.28.247.53";
		if(ipCam=="d") ipCam="169.254.164.234";
		control trackObj("http://"+ipCam);
	}
	else if(menu==2)
	{
		std::string file="";
		while(file=="")
		{
			std::cout << "Please specify the video file name :" << std::endl;
			std::cin >> file;
		}
		control trackObj(file.c_str());
	}
	
	/*test du httpClient*/
	/*httpClient http("http://10.0.77.33");
	while(1)
	{
		std::cout << " x :\t";
		int x;
		std::cin >> x;
		std::cout << " y :\t";
		int y;
		std::cin >> y;
		http.setCenter(x,y);

		std::cout << "direction (right, left, up or down): ";
		std::string dir;
		std::cin >> dir;
		http.setDirection(dir);
	}*/
	return 0;
}
