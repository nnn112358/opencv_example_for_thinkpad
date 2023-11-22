# opencv_example_for_thinkpad

## 背景
Thinkpadの内蔵カメラをUbuntu環境にて、OpenCVを使ったC言語のプログラムからキャプチャしようとしたところ、ノイジーな画像が出力されました。

![Screenshot from 2023-11-22 15-24-39.jpg](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/73020/63eeb966-f8f2-e0f0-5cc6-0419a3e118da.jpeg)

``` 
環境:
Thinkpad T480s
Ubuntu 22.04.3 LTS(x64)
OpenCV ver 0.4.5
```

## OpenCVのプログラム

OpenCVのプログラムは以下のようにサンプルプログラムを使っていました。

```cv_main.cpp
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
```

OpenCVのプログラムは以下のようにOpenCV4のライブラリでビルドをして実行してみました。

```
$ sudo apt install g++ libopencv-dev
$ g++ cv_main.cpp -o cv_main `pkg-config --cflags opencv4` `pkg-config --libs opencv4`
$ ./cv_main
```

## 調査

カメラのデバイスファイル(/dev/video0)をlsコマンドで確認したところ、なんと、デバイスファイルが４つもあることがわかりました。

```
$ ls /dev/video*
/dev/video0  /dev/video1  /dev/video2  /dev/video3
```

v4l2-ctl をインストールし、カメラのデバイスファイルを列挙してみました。
/dev/video0と/dev/video1がIR Cameraのデバイスファイルで、/dev/video2と/dev/video3が通常のCameraのデバイスファイルであることがわかりました。
 
```
$ sudo apt install v4l-utils
$ v4l2-ctl --list-devices
Integrated IR Camera: Integrate (usb-0000:00:14.0-5):
	/dev/video0
	/dev/video1
	/dev/media0

Integrated Camera: Integrated C (usb-0000:00:14.0-8):
	/dev/video2
	/dev/video3
	/dev/media1
```

## 調査:/dev/video0

OpenCVはデフォルトではYUYV形式でカメラをオープンしようとするのですが、
/dev/video0をv4l2-ctlで確認すると、縦400x横400の画像だけを出力することがわかりました。

```
$ v4l2-ctl -d /dev/video0 --list-formats-ext
ioctl: VIDIOC_ENUM_FMT
	Type: Video Capture
	[0]: 'MJPG' (Motion-JPEG, compressed)
		Size: Discrete 640x480
			Interval: Discrete 0.017s (60.000 fps)
		Size: Discrete 320x240
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 160x120
			Interval: Discrete 0.033s (30.000 fps)
	[1]: 'YUYV' (YUYV 4:2:2)
		Size: Discrete 400x480
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 400x400
			Interval: Discrete 0.033s (30.000 fps)
```

## 調査:/dev/video1

/dev/video1をv4l2-ctlで確認すると、画像が出力しないことがわかりました。

```
$ v4l2-ctl -d /dev/video1 --list-formats-ext
ioctl: VIDIOC_ENUM_FMT
	Type: Video Capture

```
## 調査:/dev/video2

/dev/video2をv4l2-ctlで確認すると、通常のカメラが出力できることがわかりました。

```
$ $ v4l2-ctl -d /dev/video2 --list-formats-ext
ioctl: VIDIOC_ENUM_FMT
	Type: Video Capture

	[0]: 'MJPG' (Motion-JPEG, compressed)
		Size: Discrete 1280x720
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 320x180
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 320x240
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 352x288
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 424x240
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 640x360
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 640x480
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 848x480
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 960x540
			Interval: Discrete 0.033s (30.000 fps)
	[1]: 'YUYV' (YUYV 4:2:2)
		Size: Discrete 1280x720
			Interval: Discrete 0.100s (10.000 fps)
		Size: Discrete 320x180
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 320x240
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 352x288
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 424x240
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 640x360
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 640x480
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 848x480
			Interval: Discrete 0.050s (20.000 fps)
		Size: Discrete 960x540
			Interval: Discrete 0.067s (15.000 fps)
```

## 調査:/dev/video3

/dev/video3をv4l2-ctlで確認すると、画像が出力しないことがわかりました。

```
$ v4l2-ctl -d /dev/video3 --list-formats-ext
ioctl: VIDIOC_ENUM_FMT
	Type: Video Capture

```

## OpenCVの修正プログラム

/dev/video0からは400x400のIRカメラの画像、/dev/video2からはカメラのカラー画像
が出てくることがわかりました。OpenCVのVideoCaptureでデバイスファイルを指定することとt、VideoCaptureのset()関数でカメラにあわせてパラメータをセットするように、OpenCVのプログラムを修正しました。


```cv_main2.cpp
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

using namespace std;

int main() {
    Mat image_ir;
    Mat image_color;

    VideoCapture cap_ir("/dev/video0");
    //ここをVideoCapture cap_ir(0);としてもOK
    VideoCapture cap_color("/dev/video2");
    //ここをVideoCapture cap_color(2);としてもOK

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
```

OpenCVのプログラムは以下のようにビルドして、IRカメラの画像とカラーカメラの画像両方がとれることを確認しました。

```
$ g++ cv_main2.cpp -o cv_main `pkg-config --cflags opencv4` `pkg-config --libs opencv4`
$ ./cv_main
```

