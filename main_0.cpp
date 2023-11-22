
// g++ `pkg-config --cflags opencv4` main.cpp -o main `pkg-config --libs opencv4` 


#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std;

int main() {
    VideoCapture cap(0);

    if (!cap.isOpened()) cout << "cannot open camera";

    while (true) {
        Mat image;
        cap >> image;
        imshow("Display window", image);
        waitKey(1);
    }

    return 0;
}
