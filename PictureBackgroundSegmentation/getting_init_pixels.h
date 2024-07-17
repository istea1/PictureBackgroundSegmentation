#include <fstream>
#include <iostream>
#include "string.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

void send_init_pixels_to_file(vector<vector<int>> i_ps, int mode);

vector<vector<int>> get_init_pixels_from_file();

vector<vector<int>> sort_init_pixels(vector<vector<int>> i_ps, int k);
