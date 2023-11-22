
// g++ `pkg-config --cflags opencv4` main.cpp -o main `pkg-config --libs
// opencv4` (base) nnn@thinkpad-t480s:~/Desktop/opencv$ ./main
// g++ `pkg-config --cflags opencv4` main.cpp -o main `pkg-config --libs opencv4` 


#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

using namespace std;

int main() {
    Mat image_ir;
    Mat image_color;

    VideoCapture cap_ir(0);
    VideoCapture cap_color(2);

    if (!cap_ir.isOpened()) cout << "cannot open camera";
    if (!cap_color.isOpened()) cout << "cannot open camera";

    if (!cap_ir.set(cv::CAP_PROP_CONVERT_RGB, 0))
        cout << "camera set convert error" << endl;
    if (!cap_ir.set(cv::CAP_PROP_FRAME_WIDTH, 400))
        cout << "camera set width error" << endl;
    if (!cap_ir.set(cv::CAP_PROP_FRAME_HEIGHT, 400))
        cout << "camera set height error" << endl;

    if (!cap_color.set(cv::CAP_PROP_FRAME_WIDTH, 640))
        cout << "camera set width error" << endl;
    if (!cap_color.set(cv::CAP_PROP_FRAME_HEIGHT, 480))
        cout << "camera set height error" << endl;

    while (true) {
        cap_ir >> image_ir;
        imshow("cap_ir", image_ir);

        cap_color >> image_color;
        imshow("cap_color", image_color);
        waitKey(1);
    }

    return 0;
}
