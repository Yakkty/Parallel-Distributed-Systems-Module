#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <filesystem>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;



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

// convert to grayscale


template<typename T>
void t_print(vector<T>& v) {
    if (v.empty()) return;
    for (T& i : v) cout << i << endl;
}

void read_images(vector<fs::path> &ifl, vector<Image> &v, vector<String> fn){
    Image img;

     for (auto i = 0; i < ifl.size(); ++i){
        string subfolders(ifl[i].string());
        size_t last = subfolders.find_last_of('\\');
        string label = subfolders.substr(last);
        glob(subfolders, fn);
        size_t count = fn.size();

        for(size_t j = 0; j < count; j++){
            // images.push_back(imread(fn[j]));
            img.training_image = imread(fn[j]);
            img.label = label;
            v.push_back(img);
    
        }
      
       
    }
    
}




int main(int argc, char** argv){


    String path(argv[1]);
    vector<String> fn;
  

    vector<fs::path> imageFolderLocations;


    for (const fs::directory_entry& dir_entry : fs::directory_iterator(path)){
        imageFolderLocations.push_back(dir_entry);
    }

     vector<Image> dataset;



     read_images(imageFolderLocations, dataset, fn);

     for(int i; i < dataset.size(); i++){
        imshow(dataset[i].label, dataset[i].training_image);
        waitKey(100);
    }

    return 0;



}