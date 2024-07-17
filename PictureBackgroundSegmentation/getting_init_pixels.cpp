#include "getting_init_pixels.h"

void send_init_pixels_to_file(vector<vector<int>> i_ps, int mode) {
    ofstream o_file;
    if (mode == 0) {
        o_file.open("init_pixels.txt", ios::app);
    }
    else {
        o_file.open("init_pixels_1_time.txt");
    }
    
    for (int i = 0; i < i_ps.size(); i++) {
        vector<int> i_p = i_ps[i];
        string st_info = "";
        for (int j = 0; j < i_p.size(); j++) {
            st_info += to_string(i_p[j]) + " ";
        }
        o_file << st_info + "\n";
    }
}

vector<vector<int>> get_init_pixels_from_file() {
    ifstream i_file;
    i_file.open("init_pixels_1_time.txt");
    string st;
    vector<vector<int>> i_ps;
    int j_0 = 0;
    while (getline(i_file, st)) {
        vector<int> now_vec(6);
        i_ps.push_back(now_vec);
        int now_numb = 0;
        int j_1 = 0;
        for (int i = 0; i < st.size(); i++) {
            if (st[i] == ' ') {
                i_ps[j_0][j_1] = now_numb;
                now_numb = 0;
                j_1 += 1;
            }
            else {
                now_numb = now_numb * 10 + int(st[i]) - 48;
            }
        }
        j_0 += 1;
    }

    return i_ps;
}

vector<vector<int>> sort_init_pixels(vector<vector<int>> i_ps, int k) {
    int bg_p = 0;
    int obj_p = 0;
    vector<vector<int>> i_ps_res(size_t(k * 5));
    random_shuffle(i_ps.begin(), i_ps.end());
    int i = 0;
    int j = 0;
    while (i < i_ps.size()) {
        if (i_ps[i][2] == 0) {
            if (bg_p < k * 2) {
                bg_p += 1;
                i_ps_res[j] = i_ps[i];
                j += 1;
            }
        }
        if (i_ps[i][2] == 1) {
            if (obj_p < k * 3) {
                obj_p += 1;
                i_ps_res[j] = i_ps[i];
                j += 1;
            }
        }
        if (obj_p >= k * 3 and bg_p >= k * 2) {
            i = i_ps.size();
        }
        i+=1;
    }
    return i_ps_res;
}