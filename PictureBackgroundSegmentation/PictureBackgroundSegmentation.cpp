// PictureBackgroundSegmentation.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "opencv2/opencv.hpp"
#include <time.h>
#include "string.h"
#include <opencv2/flann.hpp>
#include <windows.h>
#include <ppl.h>

using namespace concurrency;
using namespace std;
using namespace cv;

//struct InitPixel
//{
//	vector<int> coords;
//	string _class;
//	vector<int> color;
//};

Mat background_segmentation_by_knn(Mat img, vector<vector<int>> init_pixels, int k);

Mat masking_image(Mat img, int k);

int testing_();

Mat image;

vector<vector<int>> init_pixels;

int k_list[] = {1, 3, 5, 11, 23};

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
        init_pixels.push_back(init_pixel);
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
        init_pixels.push_back(init_pixel);
        cout << "coords: " << x << " " << y << ", class: " << init_pixel[2] << ", color: " << init_pixel[3] << " " << init_pixel[4] << " " << init_pixel[5] << "\n";

    }
}

int main(int argc, char** args)
{
    image = imread("plastics_aligned.png");
    //Mat image_mask;
    image = masking_image(image, 1);
    namedWindow("out", WINDOW_NORMAL);
    //imshow("out", image);*/
    //testing_();
    
    imshow("out", image);
    waitKey(0);
    return 0;
}

Mat background_segmentation_by_knn(Mat img, vector<vector<int>> init_pixels, int k) {
    Mat im = img.clone();
    if (k > init_pixels.size()) {
        k = int(init_pixels.size());
    }
    
    parallel_for(size_t(0), size_t(im.rows), [&](size_t i) {
        for (int j = 0; j < im.cols; j++) {
            Vec3b now_color = im.at<Vec3b>(i, j);
            vector<vector<int>> distances_with_init_pixels;
            for (int d = 0; d < init_pixels.size(); d++) {
                //int distance = ((now_color[0] - init_pixels[d][3]) * (now_color[0] - init_pixels[d][3])) + ((now_color[1] - init_pixels[d][4]) * (now_color[1] - init_pixels[d][4])) + ((now_color[2] - init_pixels[d][5]) * (now_color[2] - init_pixels[d][5]));
                //vector<int> distance_and_class = {distance, init_pixels[d][2]};
                distances_with_init_pixels.push_back({ ((now_color[0] - init_pixels[d][3]) * (now_color[0] - init_pixels[d][3])) + ((now_color[1] - init_pixels[d][4]) * (now_color[1] - init_pixels[d][4])) + ((now_color[2] - init_pixels[d][5]) * (now_color[2] - init_pixels[d][5])), init_pixels[d][2] });
            }
            partial_sort(distances_with_init_pixels.begin(), distances_with_init_pixels.begin() + k, distances_with_init_pixels.end());
            int m[2] = { 0, 0 };
            for (int c = 0; c < k; c++) {
                m[distances_with_init_pixels[c][1]] += 1;
            }
            if (m[0] > m[1]) {
                im.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            }
            else {
                im.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
            }
        }
    });
	return im;
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
    Mat res = background_segmentation_by_knn(img, init_pixels, k);
    init_pixels = empty_vec;
    clock_t end = clock();
    while (init_pixels.size() > 0) {
        init_pixels.pop_back();
    }
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "This try, with k = " << k << " worked " << seconds << " seconds\n";
    return res;
}

int testing_() {
    int i = 0;
    image = imread("plastics_aligned.png");
    while (i < 5) {
        int k = k_list[i];
        int j = 0;
        float min_seconds = 9999999;
        float max_seconds = -9999999;
        float mid_seconds = 0;
        cout << "k = " << k << "\n";
        while (j < 5) {
            Mat res = masking_image(image, k);
            string file_name = "res_k_=_" + to_string(k) + "_try_=_" + to_string(j + 1) + ".png";
            imwrite("version_01_test//" + file_name, res);
            j += 1;
        }
        i += 1;
    }
}