#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <cmath>
#include <math.h>
#include <bits/stdc++.h>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;



struct Image
{
    Mat training_image;
    Mat gray_image;
    string label;
    double distance;

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
void convert_to_gray_scale_serial(unsigned char *input, unsigned char *output, int start, int end,
                           int channel)
{
    int j = start;
    int number_of_pixels = end;

    for (int i = start; i < number_of_pixels; i += channel){
        int blue_value = input[i];
        int green_value = input[i + 1];
        int red_value = input [i + 2];

        output[j++] = (int)(0.114 * blue_value + 0.587 * green_value + 0.299 * red_value);
    }
}

template<typename T>
void t_print(vector<T>& v) {
    if (v.empty()) return;
    for (T& i : v) cout << i << endl;
}

void read_images(vector<fs::path> &ifl, vector<Image> &td, vector<String> fn){
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
            td.push_back(img);
    
        }
      
       
    }
    
}

double calc_euc_dist(unsigned char *input1, unsigned char *unknown, int start, int end){


        int number_of_pixel = end;
        double result = 0;

        for(int j = start; j < number_of_pixel; ++j){
            result += (unknown[j] - input1[j]) * (unknown[j] - input1[j]);
        }

        double distance = sqrt(result);
        return distance;

}





int main(int argc, char** argv){


    String path(argv[1]);
    string testImage;

    int K = stoi(argv[2]);

    cout << "Enter an image ";
    cin >> testImage;

    Mat testImg = imread(testImage);

    vector<String> fn;
  

    vector<fs::path> imageFolderLocations;


    for (const fs::directory_entry& dir_entry : fs::directory_iterator(path)){
        imageFolderLocations.push_back(dir_entry);
    }

     vector<Image> train_dataset;

     vector<Image> train_dataset_gray;
     vector<Image> distance_lbl_array;



     read_images(imageFolderLocations, train_dataset, fn);

    //  for(int i; i < train_dataset.size(); i++){
    //     imshow(train_dataset[i].label, train_dataset[i].training_image);
    //     waitKey(100);
    // }

    unsigned char *test_input = (unsigned char *)testImg.data;
    unsigned char *test_output = new unsigned char[testImg.size().width * testImg.size().height];
    const int total_number_of_pixels_test = testImg.rows * testImg.cols * testImg.channels();

    convert_to_gray_scale_serial(test_input, test_output, 0, total_number_of_pixels_test, testImg.channels());

    for (int i = 0; i < train_dataset.size(); i++){
        auto image = train_dataset[i].training_image;
        string label = train_dataset[i].label;
        unsigned char *train_input = (unsigned char *)image.data;
        unsigned char *train_output = new unsigned char[image.size().width * image.size().height];

        const int total_number_of_pixels = image.rows * image.cols * image.channels();

        //Convert training images to grayscale
        convert_to_gray_scale_serial(train_input, train_output, 0, total_number_of_pixels, train_dataset[i].training_image.channels());
    

        Mat gray_image = Mat(image.size().height, image.size().width, CV_8UC1, (unsigned *)train_output);

        train_dataset[i].gray_image = gray_image;
        train_dataset[i].label = label;
        train_dataset[i].distance = calc_euc_dist(train_output, test_output, 0, (total_number_of_pixels / 3));

        
    }
    

    // sort dataset 
    sort(train_dataset.begin(), train_dataset.end(), [](const Image &i1, const Image &i2){
        return i1.distance < i2.distance; 
    });

    int daisy_count = 0;
    int dandelion_count = 0;
    int rose_count = 0;
    int sunflower_count = 0;
    int tulip_count = 0;

    // for (int i = 0; i < train_dataset.size(); i++){
    //     cout << train_dataset[i].label << endl;
    // }


    for(int i = 0; i < K; ++i){
        if(train_dataset[i].label.compare("\\tulip") == 0) tulip_count++;
        else if(train_dataset[i].label.compare("\\dandelion") == 0) dandelion_count++;
        else if(train_dataset[i].label.compare("\\rose") == 0) rose_count++;
        else if(train_dataset[i].label.compare("\\sunflower") == 0) sunflower_count++;
        else if(train_dataset[i].label.compare("\\daisy") == 0) daisy_count++;
        else {

        }

    }

   auto estimated_label = max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count});

   cout << "Daisy count: " << daisy_count << endl;
   cout << "dand count: "  << dandelion_count << endl;
   cout << "rose count: " << rose_count << endl;
   cout << "sun count: " << sunflower_count << endl;
   cout << "tul count: " << tulip_count << endl;
   cout << estimated_label << endl;

   if(estimated_label == daisy_count){
       cout << "Unknown image is: a Daisy with confidence of: " << 100 * double(daisy_count) / K << "%" << endl;
   }
   else if(estimated_label == dandelion_count){
       cout << "Unknown image is: a Dandelion with confidence of: " << 100 * double(dandelion_count) / K << "%" << endl;
   }
   else if(estimated_label == rose_count){
       cout << "Unknown image is: a Rose with confidence of: " << 100 * double(rose_count) / K << "%" << endl;
   }
   else if(estimated_label == sunflower_count){
       cout << "Unknown image is: a Sunflower with confidence of: " << 100 * double(sunflower_count) / K << "%" << endl;
   }
   else if(estimated_label == tulip_count){
       cout << "Unknown image is: a Tulip with confidence of: " << 100 * double(tulip_count) / K << "%" << endl;
   } else {
       cout << "Use an odd k value";
   }



    // for(int i; i < train_dataset_gray.size(); i++){
    //     imshow(train_dataset_gray[i].label, train_dataset_gray[i].training_image);
    //     waitKey(100);
    // }

    return 0;



}