// PictureBackgroundSegmentation.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "make_mask.h"

using namespace concurrency;
using namespace std;
using namespace cv;

Mat masking_image(Mat img, int k);

void testing();

Mat image;

vector<vector<int>> init_pixels_global;

void InitPixel(int action, int x, int y, int flags, void* userdata)
{
    vector<int> init_pixel;

    if (action == EVENT_LBUTTONDOWN)
    {
        init_pixel.push_back(x);
        init_pixel.push_back(y);
        init_pixel.push_back(0);
        init_pixel.push_back(image.at<Vec3b>(y, x)[0]);
        init_pixel.push_back(image.at<Vec3b>(y, x)[1]);
        init_pixel.push_back(image.at<Vec3b>(y, x)[2]);
        init_pixels_global.push_back(init_pixel);
        cout << "coords: " << x << " " << y << ", class: " << init_pixel[2] << ", color: " << init_pixel[3] << " " << init_pixel[4] << " " << init_pixel[5] << "\n";
    }
    else if (action == EVENT_RBUTTONDOWN)
    {
        init_pixel.push_back(x);
        init_pixel.push_back(y);
        init_pixel.push_back(1);
        init_pixel.push_back(image.at<Vec3b>(y, x)[0]);
        init_pixel.push_back(image.at<Vec3b>(y, x)[1]);
        init_pixel.push_back(image.at<Vec3b>(y, x)[2]);
        init_pixels_global.push_back(init_pixel);
        cout << "coords: " << x << " " << y << ", class: " << init_pixel[2] << ", color: " << init_pixel[3] << " " << init_pixel[4] << " " << init_pixel[5] << "\n";

    }
}

int main(int argc, char** args)
{
    image = imread(args[1]);
    image = masking_image(image, 3);
    //testing();
    namedWindow("out", WINDOW_NORMAL);
    imshow("out", image);
    waitKey(0);
    return 0;
}

Mat masking_image(Mat img, int k) {
    namedWindow("Window", WINDOW_NORMAL);
    setMouseCallback("Window", InitPixel);

    int q = 0;

    vector<vector<int>> empty_vec = {};

    while (q != 113)
    {
        imshow("Window", img);
        q = waitKey(0);
    }
    destroyAllWindows();
    clock_t start = clock();
    Mat res = background_segmentation_by_knn(img, init_pixels_global, k);
    init_pixels_global = empty_vec;
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "This try, with k = " << k << " worked " << seconds << " seconds\n";
    return res;
}

void testing() {
    int i = 0;
    int k_list[] = { 1, 3, 5, 11, 23 };
    image = imread("plastics_aligned.png");
    while (i < 5) {
        int k = k_list[i];
        int j = 0;
        float min_seconds = 9999999;
        float max_seconds = -9999999;
        float mid_seconds = 0;
        cout << "k = " << k << "\n";
        while (j < 1) {
            Mat res = masking_image(image, k);
            string file_name = "res_k_=_" + to_string(k) + "_try_=_" + to_string(j + 1) + ".png";
            imwrite("version_01_test//" + file_name, res);
            j += 1;
        }
        i += 1;
    }
}