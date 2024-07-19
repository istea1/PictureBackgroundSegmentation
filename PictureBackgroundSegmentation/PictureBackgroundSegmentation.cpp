// PictureBackgroundSegmentation.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "make_mask.h"
#include "getting_init_pixels.h"

using namespace concurrency;
using namespace std;
using namespace cv;

void save_init_pixels(Mat im);

Mat masking_image(Mat img, int k);

void testing();

Mat compare_mask_and_orig(Mat im, Mat mask);

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
    //setlocale(LC_ALL, "Russian");
    image = imread(args[1]);
    ////save_init_pixels(image);
    Mat image_mask = masking_image(image, 5);
    //testing();
    Mat comp = compare_mask_and_orig(image, image_mask);
    namedWindow("out", WINDOW_NORMAL);
    imwrite("comp.png", comp);
    waitKey(0);
    return 0;
}

void save_init_pixels(Mat im) {
    namedWindow("Window", WINDOW_NORMAL);
    setMouseCallback("Window", InitPixel);

    int q = 0;

    while (q != 113)
    {
        imshow("Window", im);
        q = waitKey(0);
    }
    destroyAllWindows();
    //send_init_pixels_to_file(init_pixels_global, 0);
}

Mat compare_mask_and_orig(Mat im, Mat mask) {
    Mat img = im.clone();
    for (int i = 0; i < im.rows; i++) {
        for (int j = 0; j < im.cols; j++) {
            if (mask.at<Vec3b>(i, j) == Vec3b(0, 0, 0)) {
                img.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            }
        }
    }
    return img;
}

Mat masking_image(Mat img, int k) {
    //vector<vector<int>> init_pixels = get_init_pixels_from_file();
    //init_pixels = sort_init_pixels(init_pixels, k);
    save_init_pixels(img);
    vector<vector<int>> init_pixels = init_pixels_global;
    int num_threads = 12;
    //vector<vector<int>> empty_vec = {};
    clock_t start = clock();
    Mat res = background_segmentation_by_knn(img, init_pixels, k, num_threads);
    //init_pixels_global = empty_vec;
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "This try, with k = " << k << " worked " << seconds << " seconds\n";
    return res;
}

void testing() {
    int i = 0;
    int k_list[] = { 1, 3, 5, 7, 9};
    string file_name;
    Mat res;
    image = imread("plastics_aligned.png");
    while (i < 5) {
        int k = k_list[i];
        int j = 0;
        float min_seconds = 9999999;
        float max_seconds = -9999999;
        float mid_seconds = 0;
        cout << "k = " << k << "\n";
        while (j < 5) {
            res = masking_image(image, k);
            file_name = to_string(k) + "_" + to_string(j + 1) + ".png";
            imwrite("version_avx_test//" + file_name, res);
            j += 1;
        }
        i += 1;
    }
}