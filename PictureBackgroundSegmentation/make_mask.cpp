#include "make_mask.h"

using namespace concurrency;
using namespace std;
using namespace cv;


Mat background_segmentation_by_knn(Mat img, vector<vector<int>> init_pixels, int k, int num_threads) {
    
    Mat im = img.clone();
    if (k > init_pixels.size()) {
        k = init_pixels.size();
    }
    
    vector<thread> threads(num_threads);
    vector<int> distance_t(num_threads);
    vector<vector<float>> distances_t(num_threads);
    for (int i = 0; i < num_threads; i++) {
        distance_t[i] = 0;
        distances_t[i] = vector<float>(init_pixels.size());
    }

    auto thread_func = [init_pixels, k, num_threads](Mat im, int thread_id, int distance, vector<float> distances, vector<int> m) {
        int i = im.rows / num_threads * thread_id;
        int i_fin = im.rows / num_threads * (thread_id + 1);
        int r, g, b, r_ip, g_ip, b_ip;
        for (; i < i_fin; i++) {
            for (int j = 0; j < im.cols; j++) {
                m = { 0, 0 };
                Vec3b now_color = im.at<Vec3b>(i, j);
                r = now_color[0];
                g = now_color[1];
                b = now_color[2];
                __m128i n_c = _mm_set_epi32(r, g, b, 0);
                for (int d = 0; d < init_pixels.size(); d++) {
                    r_ip = init_pixels[d][3];
                    g_ip = init_pixels[d][4];
                    b_ip = init_pixels[d][5];
                    __m128i i_c = _mm_set_epi32(r_ip, g_ip, b_ip, 0);
                    __m128i minuss = _mm_sub_epi32(n_c, i_c);
                    __m128i m_c = minuss;
                    __m128i for_sum = _mm_mul_epi32(minuss, m_c);
                    int* result = (int*)&for_sum;
                    distance =
                        result[0] +
                        result[1] +
                        result[2];
                    cout << distance << "\n";
                    distances[d] = distance + 0.1 * init_pixels[d][2];
                }
                partial_sort(distances.begin(), distances.begin() + k, distances.end());
                for (int c = 0; c < k; c++) {
                    m[distances[c] * 10 - int(distances[c]) * 10] += 1;
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

    for (int thread_id = 0; thread_id < num_threads; ++thread_id) {
        threads[thread_id] = thread(thread_func, im, thread_id, distance_t[thread_id], distances_t[thread_id], vector<int>(2));
    }
    for (int thread_id = 0; thread_id < num_threads; ++thread_id) {
        threads[thread_id].join();
    }

    return im;
}
