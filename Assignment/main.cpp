#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


struct Image
{
    Mat training_image;
    string label;

    //Constructor
    Image(){}

    friend ostream& operator<<(ostream& out, const Image& t);

    //Destructor
    ~Image(){}


};

// ostream& operator<<(ostream& out, const Image& i) {
//     out << "{ blue: " << i.blue << ", green: " << i.green 
//         << " , red: " << i.red <<  " }";
//     return out;
// }

// void get_RGB_Values(unsigned char* input, unsigned char* output, int start, int end,
//     int channel, vector<Mat> &v, string label) {

//     Image img;

//     int j = start;
//     int number_of_pixel = end;

//     // i = 0 {0, 1, 2}; i = 3 {3, 4, 5}, i = 6
//     for (int i = start; i < number_of_pixel; i+=channel) { // i+=3;
//         int blue_value = input[i];
//         int green_value = input[i + 1];
//         int red_value = input[i + 2];

//         output[j++] = img.blue = blue_value, img.green = green_value, img.red = red_value;    
//     }
// }

// void get_RGB_Values(const Mat& rgb, vector<Image> &v) { // &: reference. 

//     Image img;


//     for (int row = 0; row < rgb.rows; ++row) {
//         for (int col = 0; col < rgb.cols; ++col) {

//             // extract a single RGB value
//             Vec3b bgrpixel = rgb.at<Vec3b>(row, col); // {0,0}, {0,1}, .. {0,2}..
            
          
//             img.blue = bgrpixel[0];
//             img.green = bgrpixel[1];
//             img.red = bgrpixel[2];
//             v.push_back(img);
//         }
//     }

// }

void store_training_images(vector<Mat> &d, vector<Image> &v){
    Image img;

    for (int i = 0; i < d.size(); i++){
        img.training_image = d[i];
        img.label = "Daisy";
        v.push_back(img);
        // imshow(img.label, img.training_image);
        // waitKey(100);
    }

}



template<typename T>
void t_print(vector<T>& v) {
    if (v.empty()) return;
    for (T& i : v) cout << i << endl;
}



int main(int argc, char** argv){

    // string img(argv[1]);
    // Mat srcImage = imread(img);

    String path(argv[1]);
    vector<String> fn;

    glob(path, fn);

    vector<Mat> images;

    size_t count = fn.size();
    for (size_t i = 0; i < count; i++){
        images.push_back(imread(fn[i]));
    }

    vector<Image> dataset;


    store_training_images(images, dataset);
   

    for(int i; i < dataset.size(); i++){
        imshow(dataset[i].label, dataset[i].training_image);
        waitKey(100);
    }


    return 0;



}