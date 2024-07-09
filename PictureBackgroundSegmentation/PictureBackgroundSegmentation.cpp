// PictureBackgroundSegmentation.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

//struct InitPixel
//{
//	vector<int> coords;
//	string _class;
//	vector<int> color;
//};

Mat background_segmentation_by_knn(Mat im, vector<vector<int>> init_pixels, int k);

Mat image;

vector<vector<int>> init_pixels;

// function which will be called on mouse input
void InitPixel(int action, int x, int y, int flags, void* userdata)
{
    vector<int> init_pixel;

    // Mark the top left corner when left mouse button is pressed
    if (action == EVENT_LBUTTONDOWN)
    {
        init_pixel.push_back(x);
        init_pixel.push_back(y);
        init_pixel.push_back(0);
        init_pixel.push_back(image.at<Vec3b>(y, x)[0]);
        init_pixel.push_back(image.at<Vec3b>(y, x)[1]);
        init_pixel.push_back(image.at<Vec3b>(y, x)[2]);
        init_pixels.push_back(init_pixel);
        //image.at<Vec3b>(y, x) = (0, 0, 0);
        //imshow("Window", image);
    }
    // When left mouse button is released, mark bottom right corner
    else if (action == EVENT_RBUTTONDOWN)
    {
        init_pixel.push_back(x);
        init_pixel.push_back(y);
        init_pixel.push_back(255);
        init_pixel.push_back(image.at<Vec3b>(y, x)[0]);
        init_pixel.push_back(image.at<Vec3b>(y, x)[1]);
        init_pixel.push_back(image.at<Vec3b>(y, x)[2]);
        init_pixels.push_back(init_pixel);
        //image.at<Vec3b>(y, x) = (0, 0, 0);
        //imshow("Window", image);
    }

}

// Main function
int main(int argc, char** args)
{
    image = imread(args[1]);
    int k = 5;
    namedWindow("Window");
    // highgui function called when mouse events occur
    setMouseCallback("Window", InitPixel);

    int q = 0;
    // loop until q character is pressed
    while (q != 113)
    {
        imshow("Window", image);
        q = waitKey(0);
    }
    destroyAllWindows();
    image = background_segmentation_by_knn(image, init_pixels, k);
    imshow("res", image);
    waitKey(0);
    return 0;
}

Mat background_segmentation_by_knn(Mat im, vector<vector<int>> init_pixels, int k) {
    k = std::min(k, int(init_pixels.size()));
	for (int i = 0; i < im.rows; i++) {
		for (int j = 0; j < im.cols; j++) {
			Vec3b now_color = im.at<Vec3b>(i, j);
			vector<vector<int>> distances_with_init_pixels;
			for (int d = 0; d < init_pixels.size(); d++) {
				int distance = pow((now_color[0] - init_pixels[d][3]), 2) + pow((now_color[1] - init_pixels[d][4]), 2) + pow((now_color[2] - init_pixels[d][5]), 2);
				vector<int> distance_and_class = {distance, init_pixels[d][2]};
				distances_with_init_pixels.push_back(distance_and_class);
			}
			partial_sort(distances_with_init_pixels.begin(), distances_with_init_pixels.begin() + k, distances_with_init_pixels.end());
			int m[256];
			m[0] = 0;
			m[255] = 0;
			for (int c = 0; c < k; c++) {
				m[distances_with_init_pixels[c][1]] += 1;
			}
			if (m[0] > m[255]) {
				im.at<Vec3b>(i, j) = (0, 0, 0);
			}
			else {
				im.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			}
		}
	}
	return im;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
