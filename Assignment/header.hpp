//Header file to hold image struct, commonly used functions

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

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

// Image structure, to store images, labels and distances
struct Image
{
    Mat training_image;
    Mat test_image;
    Mat gray_image;
    string label;
    double distance;

    // Constructor
    Image() {}

    // Destructor
    ~Image() {}
};

// Greyscale conversion function, accomplished by iterating through every pixel and converting said pixel to greyscale with a formula
void convert_to_gray_scale_serial(unsigned char *input, unsigned char *output, int start, int end,
                                  int channel)
{
    int j = start;
    int number_of_pixels = end;

    // i += channel as these coloured images have 3 channels
    for (int i = start; i < number_of_pixels; i += channel)
    {
        int blue_value = input[i];
        int green_value = input[i + 1];
        int red_value = input[i + 2];

        output[j++] = (int)(0.114 * blue_value + 0.587 * green_value + 0.299 * red_value);
    }
}

// Function to read training images into a structure (vector training_data)
void read_images(vector<fs::path> &ifl, vector<Image> &td, vector<String> &fn)
{
    // Create an image object
    Image img;

    // Loop through every subfolder in the training data folder locations vector
    // i.e i = 1 could be the folder containing all daisy training images, i = 2 could be rose, etc
    for (auto i = 0; i < ifl.size(); ++i)
    {
        // Substringing the end of the folder path to store as a label, as every item in the daisy folder will be a daisy
        string subfolders(ifl[i].string());
        size_t last = subfolders.find_last_of('\\');
        string label = subfolders.substr(last);

        // Retrieve every file in subfolder and push to training_data vector
        glob(subfolders, fn);
        // Count is equal to the number of files in the folder
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

//Function to calculate distance between training image and test image
double calc_euc_dist_serial(unsigned char *input1, unsigned char *unknown, int start, int end)
{

    int number_of_pixel = end;
    double result = 0;

    //Iterate through every pixel (number is same for both as they are the same size)
    for (int j = start; j < number_of_pixel; ++j)
    {   
        //Sum the result of test image pixel value - training image pixel value
        result += (unknown[j] - input1[j]) * (unknown[j] - input1[j]);
    }
    //Sqrt the sum and return it, this provides the distance between training image and test image
    double distance = sqrt(result);
    return distance;
}