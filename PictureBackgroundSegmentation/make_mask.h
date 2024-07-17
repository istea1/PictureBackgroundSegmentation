#include <iostream>
#include <fstream>
#include "opencv2/opencv.hpp"
#include <time.h>
#include "string.h"
#include <opencv2/flann.hpp>
#include <windows.h>
#include <map>
#include <thread>
#include <algorithm>
#include <ppl.h>
#include <immintrin.h>

using namespace std;
using namespace cv;

Mat background_segmentation_by_knn(Mat img, vector<vector<int>> init_pixels, int k, int num_threads=12);

