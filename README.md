# Quality-Aware-Facial-Video-from-Axis-PTZ-Camera
A C++ project that implements client server communication between a facial image quality assessment module and a PTZ camera. The camera is automatically controlled to find the face in an image frame and track the face over video frames. Quality assessment is done for each of the face detected in the video frames and good quality faces are selected.

# What implemented
- Client-server communication using POCO library
- Face detection using OPENCV
- Face tracking using CAMSHIFT
- Quality assessment on parameters like brightness, resolutions, sharpness, etc. 


# Prerquisites
- VC 2012
- OpenCV
- Axis PTZ914 Camera
- Axis IP utility and camera station server
- POCO Network Library (http://pocoproject.org/)

# How to run
- Download the repository in your computer. 
- Open the project in VC2012
- Connect AXIS PTZ914 camera (remember to put the right ip address of the camera in the code!)
- Build in VC
- Address the link or compile errors to match the approprite path and libraries
- You can see the example outpout in a folder with face quality matrices in 'output.txt' file


# Please cite
Haque, M. A., Nasrollahi, K., and Moeslund, T. B. (2013).Real-time acquisition of high quality face sequencesfrom  an  active  pan-tilt-zoom  camera.   In2013 10thIEEE International Conference on Advanced Videoand Signal Based Surveillance, pages 443–448
