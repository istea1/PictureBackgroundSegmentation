#include "make_mask.h"

using namespace concurrency;
using namespace std;
using namespace cv;


Mat background_segmentation_by_knn(Mat img, vector<vector<int>> init_pixels, int k) {
    
    Mat im = img.clone();
    if (k > init_pixels.size()) {
        k = init_pixels.size();
    }
    int num_threads = 20;
    vector<vector<int>> distances_t(num_threads);
    for (int i = 0; i < num_threads; i++) {
        distances_t[i] = vector<int>(init_pixels.size());
    }
    parallel_for(int(0), int(im.rows), [&](int i) {
        vector<float> distances_with_init_pixels(init_pixels.size());
        for (int j = 0; j < im.cols; j++) {
            Vec3b now_color = im.at<Vec3b>(i, j);
            for (int d = 0; d < init_pixels.size(); d++) {
                float distance =
                    ((now_color[0] - init_pixels[d][3]) * (now_color[0] - init_pixels[d][3])) +
                    ((now_color[1] - init_pixels[d][4]) * (now_color[1] - init_pixels[d][4])) +
                    ((now_color[2] - init_pixels[d][5]) * (now_color[2] - init_pixels[d][5]));
                distances_with_init_pixels[d] = distance + 0.1 * init_pixels[d][2];
            }
            partial_sort(distances_with_init_pixels.begin(), distances_with_init_pixels.begin() + k, distances_with_init_pixels.end());
            int m[2] = { 0, 0 };
            for (int c = 0; c < k; c++) {
                m[int((distances_with_init_pixels[c] - int(distances_with_init_pixels[c])) * 10)] += 1;
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
