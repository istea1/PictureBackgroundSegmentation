#include "make_mask.h"

using namespace concurrency;
using namespace std;
using namespace cv;


Mat background_segmentation_by_knn(Mat img, vector<vector<int>> init_pixels, int k) {
    
    Mat im = img.clone();
    if (k > init_pixels.size()) {
        k = init_pixels.size();
    }
    int num_threads = 12;
    int size_cbd = 256 * 256 * 256;
    vector<thread> threads(num_threads);
    vector<int> distance_t = vector<int>(size_cbd);
    vector<vector<int>> classes_by_distances_t(num_threads);
    vector<vector<int>> distances_t(num_threads);
    for (int i = 0; i < num_threads; i++) {
        distance_t[i] = 0;
        classes_by_distances_t[i] = vector<int>(size_cbd);
        distances_t[i] = vector<int>(init_pixels.size());
    }

    auto thread_func = [init_pixels, k, num_threads](Mat im, int thread_id, int distance, vector<int>classes_by_distances, vector<int> distances) {
        int i = im.rows / num_threads * thread_id;
        int i_fin = im.rows / num_threads * (thread_id + 1);
        //cout << i << " " << i_fin << "\n";
        for (; i < i_fin; i++) {
            for (int j = 0; j < im.cols; j++) {
                Vec3b now_color = im.at<Vec3b>(i, j);
                for (int d = 0; d < init_pixels.size(); d++) {
                    distance =
                        ((now_color[0] - init_pixels[d][3]) * (now_color[0] - init_pixels[d][3])) +
                        ((now_color[1] - init_pixels[d][4]) * (now_color[1] - init_pixels[d][4])) +
                        ((now_color[2] - init_pixels[d][5]) * (now_color[2] - init_pixels[d][5]));
                    classes_by_distances[distance] = init_pixels[d][2];
                    distances[d] = distance;
                }
                partial_sort(distances.begin(), distances.begin() + k, distances.end());
                int m[2] = { 0, 0 };
                for (int c = 0; c < k; c++) {
                    m[classes_by_distances[distances[c]]] += 1;
                }
                if (m[0] > m[1]) {
                    im.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
                }
                else {
                    im.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
                }
            }
        }
    };

    for (int thread_id = 0;  thread_id < num_threads; ++thread_id) {
        threads[thread_id] = thread(thread_func, im, thread_id, distance_t[thread_id], classes_by_distances_t[thread_id], distances_t[thread_id]);
        int i = im.rows / num_threads * thread_id;
        int i_fin = im.rows / num_threads * (thread_id + 1);
        cout << i << " " << i_fin << "\n";
    }
    for (int thread_id = 0;  thread_id < num_threads; ++thread_id) {
        threads[thread_id].join();
    }

    return im;
}
