// Very Useful website
// http://derekmolloy.ie/beaglebone-images-video-and-opencv/
g++  `pkg-config --cflags opencv` `pkg-config --libs opencv` boneCV.cpp -o boneCV
mjpg_streamer -i "input_uvc.so" -o "output_http.so -p 8090"
