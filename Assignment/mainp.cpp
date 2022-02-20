#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <chrono>
#include <future>
#include <mutex>
#include <shared_mutex>
#include <cmath>
#include <math.h>
#include <bits/stdc++.h>

using namespace cv;
using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;
mutex mu;
recursive_mutex mtx;


struct Image
{
    Mat training_image;
    Mat test_image;
    Mat gray_image;
    // unsigned char *output;
    string label;
    double distance;

    // Constructor
    Image() {}

    friend ostream &operator<<(ostream &out, const Image &t);

    // Destructor
    ~Image() {}
};

void convert_to_gray_scale_serial(unsigned char *input, unsigned char *output, int start, int end,
                                  int channel)
{
    int j = start;
    int number_of_pixels = end;

    for (int i = start; i < number_of_pixels; i += channel)
    {
        int blue_value = input[i];
        int green_value = input[i + 1];
        int red_value = input[i + 2];

        output[j++] = (int)(0.114 * blue_value + 0.587 * green_value + 0.299 * red_value);
    }
}


void read_images_parallel(vector<fs::path> &ifl, vector<Image> &td, vector<String> fn, int start, int end, int depth)
{
    if (depth > 3)
    {
        Image img;

        for (auto i = start; i < end; ++i)
        {
            string subfolders(ifl[i].string());
            size_t last = subfolders.find_last_of('\\');
            string label = subfolders.substr(last);
            glob(subfolders, fn);
            size_t count = fn.size();

            for (size_t j = 0; j < count; j++)
            {
                // images.push_back(imread(fn[j]));
                img.training_image = imread(fn[j]);
                img.label = label;
                td.push_back(img);
            }
        }
    }
    else
    {   

        auto mid = (start + end) / 2;
        auto left = async(launch::async, read_images_parallel, ref(ifl), ref(td), ref(fn), start, mid, depth + 1);
        read_images_parallel(ifl, td, fn, mid, end, depth + 1);
 
    }
}

double calc_euc_dist_serial(unsigned char *input1, unsigned char *unknown, int start, int end)
{

    int number_of_pixel = end;
    double result = 0;

    for (int j = start; j < number_of_pixel; ++j)
    {
        result += (unknown[j] - input1[j]) * (unknown[j] - input1[j]);
    }
    double distance = sqrt(result);
    return distance;
}

void get_dist(vector<Image> &train_dataset, unsigned char *test_output, int start, int end, int depth)
{
    if (depth > 3)
    {
        double distance = 0;
        
        for (int i = start; i < end; i++)
        {
            auto image = train_dataset[i].training_image;

            unsigned char *train_input = (unsigned char *)image.data;
            unsigned char *train_output = new unsigned char[image.size().width * image.size().height];

            const int total_number_of_pixels = image.rows * image.cols * image.channels();

            convert_to_gray_scale_serial(train_input, train_output, 0, total_number_of_pixels, image.channels());

            double distance = calc_euc_dist_serial(train_output, test_output, 0, (total_number_of_pixels / 3));

            train_dataset[i].distance = sqrt(distance);
            
            delete train_output;
            
        }
    
    }
    else
    {   
   
        auto mid = (start + end) / 2;
        auto left = async(launch::async, get_dist, ref(train_dataset), test_output, start, mid, depth + 1);
        get_dist(train_dataset, test_output, mid, end, depth + 1);

    }
}

void compare(vector<Image> &train_d, int K)
{

    int daisy_count = 0;
    int dandelion_count = 0;
    int rose_count = 0;
    int sunflower_count = 0;
    int tulip_count = 0;

    for (int i = 0; i < K; ++i)
    {
        if (train_d[i].label.compare("\\tulip") == 0)
            tulip_count++;
        else if (train_d[i].label.compare("\\dandelion") == 0)
            dandelion_count++;
        else if (train_d[i].label.compare("\\rose") == 0)
            rose_count++;
        else if (train_d[i].label.compare("\\sunflower") == 0)
            sunflower_count++;
        else if (train_d[i].label.compare("\\daisy") == 0)
            daisy_count++;
        else
        {
        }
    }
    cout << "Daisy count: " << daisy_count << endl;
    cout << "Dand count: " << dandelion_count << endl;
    cout << "Rose count: " << rose_count << endl;
    cout << "Sun count: " << sunflower_count << endl;
    cout << "Tul count: " << tulip_count << endl;

    if (max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count}) == daisy_count)
    {
        cout << "Image is: a Daisy with confidence of: " << 100 * double(daisy_count) / K << "%" << endl;
    }
    else if (max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count}) == dandelion_count)
    {
        cout << "Image is: a Dandelion with confidence of: " << 100 * double(dandelion_count) / K << "%" << endl;
    }
    else if (max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count}) == rose_count)
    {
        cout << "Image is: a Rose with confidence of: " << 100 * double(rose_count) / K << "%" << endl;
    }
    else if (max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count}) == sunflower_count)
    {
        cout << "Image is: a Sunflower with confidence of: " << 100 * double(sunflower_count) / K << "%" << endl;
    }
    else if (max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count}) == tulip_count)
    {
        cout << "Image is: a Tulip with confidence of: " << 100 * double(tulip_count) / K << "%" << endl;
    }
    else
    {
    }
}

int main(int argc, char **argv)
{

    String train_path(argv[1]);
    String test_path;

    int K = stoi(argv[2]);


    cout << "Enter a folder of images ";
    cin >> test_path;
    cout << test_path << endl;



    vector<String> fn;
    vector<String> test_fn;

    vector<fs::path> trainimageFolderLocations;

    for (const fs::directory_entry &dir_entry : fs::directory_iterator(train_path))
    {
        trainimageFolderLocations.push_back(dir_entry);
    }

    vector<Image> train_dataset;
    vector<Image> test_dataset;

    Image test_img;

    glob(test_path, test_fn);
    size_t count = test_fn.size();

    for (size_t i = 0; i < count; i++)
    {
        test_img.test_image = imread(test_fn[i]);
        test_dataset.push_back(test_img);

    }

    auto startCompare = steady_clock::now();
    read_images_parallel(trainimageFolderLocations, train_dataset, fn, 0, trainimageFolderLocations.size(), 0);


    for (int j = 0; j < test_dataset.size(); j++)
    {

        auto test_img = test_dataset[j].test_image;

        unsigned char *test_input = (unsigned char *)test_img.data;
        unsigned char *test_output = new unsigned char[test_img.size().width * test_img.size().height];

        const int total_number_of_pixels_test = test_img.rows * test_img.cols * test_img.channels();


        convert_to_gray_scale_serial(test_input, test_output, 0, total_number_of_pixels_test, test_img.channels());

        get_dist(train_dataset, test_output, 0, train_dataset.size(), 0);

        sort(train_dataset.begin(), train_dataset.end(), [](const Image &i1, const Image &i2)
             { return i1.distance < i2.distance; });
        compare(train_dataset, K);
    }
    auto endCompare = steady_clock::now();
    auto durationCompare = duration_cast<seconds>(endCompare - startCompare).count();
    cout << "parallel compare duration: " << durationCompare << "seconds" << endl;

 

    return 0;
}