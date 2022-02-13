#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>


using namespace cv;
using namespace std;

struct Images{
    string label;
    int r_value;
    int g_value;
    int b_value;

    Images(string tlabel = {" "}, int tr_value = 0, int tg_value = 0, int tb_value = 0) : label(tlabel), r_value(tr_value), g_value(tg_value), b_value(tb_value) {};
    ~Images() {};

    friend ostream& operator<<(ostream& out, const Images& r);
};

ostream& operator<<(ostream& out, const Images& r) {
    out << "{ red " << r.r_value << " green: " << r.g_value << " blue: " << r.b_value << " }" << endl;
    return out;
}
template <typename T>
void t_print(vector<T>& v) {
    if (v.empty()) return;
    for (T& i : v) cout << i << endl;
}

void get_rgb(const Mat& rgb, vector<Images>& data) {
    Images img;
    for (int row = 0; row < rgb.rows; ++row) {
        for (int col = 0; col < rgb.cols; ++col) {
            Vec3b bgrpixel = rgb.at<Vec3b>(row, col);

            img.b_value = bgrpixel[0];
            img.g_value = bgrpixel[1];
            img.r_value = bgrpixel[2];
            data.push_back(img);
            
        }


    }
}

int main(int argc, char** argv)
{
    std::cout << "starting the opencv basic.cpp...." << std::endl;

    if (argc != 2) {
        cout << "Error in arguments..\n";
        exit(-1);
    }

    std::string dir(argv[1]);
    vector<String> filenames;
    glob(dir, filenames);
    vector<Mat> images;
    size_t count = filenames.size(); 
    for (size_t i = 0; i < count; i++) {
        images.push_back(imread(filenames[i]));
    }
        
    vector<Images> data;
    for (int i = 0; i < images.size(); i++) {
        get_rgb(images[i], ref(data));
    }
    
    t_print(data);

    
    return 0;
}
