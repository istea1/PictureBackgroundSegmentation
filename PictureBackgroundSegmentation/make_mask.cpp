#include "make_mask.h"

using namespace concurrency;
using namespace std;
using namespace cv;


Mat background_segmentation_by_knn(Mat img, vector<vector<int>> init_pixels, int k) {
    
    Mat im = img.clone();
    if (k > init_pixels.size()) {
        k = init_pixels.size();
    }
    /*int num_threads = 6000;
    vector<vector<float>> distances_t(num_threads);
    for (int i = 0; i < num_threads; i++) {
        distances_t[i] = vector<float>(init_pixels.size());
    }*/
    vector<double> distances(init_pixels.size());
    double distance;
    for (int i = 0; i < im.rows; i++) {
        for (int j = 0; j < im.cols; j++) {
            Vec3b now_color = im.at<Vec3b>(i, j);
            for (int d = 0; d < init_pixels.size(); d++) {
                distance =
                    ((now_color[0] - init_pixels[d][3]) * (now_color[0] - init_pixels[d][3])) + 
                    ((now_color[1] - init_pixels[d][4]) * (now_color[1] - init_pixels[d][4])) + 
                    ((now_color[2] - init_pixels[d][5]) * (now_color[2] - init_pixels[d][5]));
                distances[d] = distance + (0.1 * init_pixels[d][2]);
            }
            partial_sort(distances.begin(), distances.begin() + k, distances.end());
            int m[2] = { 0, 0 };
            for (int c = 0; c < k; c++) {
                m[int((distances[c] - int(distances[c])) * 10)] += 1;
            }
            if (m[0] > m[1]) {
                im.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            }
            else {
                im.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
            }
        }
    }

    return im;
}
