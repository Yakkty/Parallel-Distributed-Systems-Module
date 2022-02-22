///////Parallel implementation of image classification program using KNN algorithm

//Includes
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

//Namespaces
using namespace cv;
using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

//Globally declared mutex
mutex mu;


//Image structure, to store images, labels and distances
struct Image
{
    Mat training_image;
    Mat test_image;
    Mat gray_image;
    string label;
    double distance;

    // Constructor
    Image() {}

    friend ostream &operator<<(ostream &out, const Image &t);

    // Destructor
    ~Image() {}
};

//Greyscale conversion function, accomplished by iterating through every pixel and converting said pixel to greyscale with a formula
void convert_to_gray_scale_serial(unsigned char *input, unsigned char *output, int start, int end,
                                  int channel)
{
    int j = start;
    int number_of_pixels = end;

    //i += channel as these coloured images have 3 channels
    for (int i = start; i < number_of_pixels; i += channel)
    {
        int blue_value = input[i];
        int green_value = input[i + 1];
        int red_value = input[i + 2];

        output[j++] = (int)(0.114 * blue_value + 0.587 * green_value + 0.299 * red_value);
    }
}

//Function to read training images into a structure (vector training_data)
void read_images(vector<fs::path> &ifl, vector<Image> &td, vector<String> &fn)
{   
    //Create an image object
    Image img;

    //Loop through every subfolder in the training data folder locations vector
    //i.e i = 1 could be the folder containing all daisy training images, i = 2 could be rose, etc
    for (auto i = 0; i < ifl.size(); ++i)
    {   
        //Substringing the end of the folder path to store as a label, as every item in the daisy folder will be a daisy
        string subfolders(ifl[i].string());
        size_t last = subfolders.find_last_of('\\');
        string label = subfolders.substr(last);

        //Retrieve every file in subfolder and push to training_data vector
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

//Divide and conquer function to iterate through train_dataset, convert images to greyscale, calculate Euclidean distance, and store this distance into the vector
void get_dist(vector<Image> &train_dataset, unsigned char *test_output, size_t start, size_t end, int depth, vector<Image> &d)
{   
    Image img;
    //Check depth to continously split up the dataset into desired base cases
    if (depth > 3)
    {   

        double distance = 0;
        size_t count = end;
        for (size_t i = start; i < end; i++)
        {

            auto image = train_dataset[i].training_image;
            string label = train_dataset[i].label;
    
            unsigned char *train_input = (unsigned char *)image.data;
            unsigned char *train_output = new unsigned char[image.size().width * image.size().height];

            const int total_number_of_pixels = image.rows * image.cols * image.channels();

            convert_to_gray_scale_serial(train_input, train_output, 0, total_number_of_pixels, image.channels());

            double distance = calc_euc_dist_serial(train_output, test_output, 0, (total_number_of_pixels / 3));

            unique_lock<mutex> lock(mu);
            // train_dataset[i].distance = distance;
            img.distance = distance;
            img.label = label;

            d.push_back(img);



            delete train_output;
        }
    }
    //If depth !=3, split datasaet into two, setting left half to be async to run simultaenously with the right side
    else
    {
        //Increment depth by 1 each time to edge closer to desired base case
        size_t mid = (start + end) / 2;
        auto left = async(launch::async, get_dist, ref(train_dataset), test_output, start, mid, depth + 1, ref(d));
        get_dist(train_dataset, test_output, mid, end, depth + 1, d);
    }
}

void compare(vector<Image> &distances, int K, string name)
{

    int daisy_count = 0;
    int dandelion_count = 0;
    int rose_count = 0;
    int sunflower_count = 0;
    int tulip_count = 0;

    for (int i = 0; i < K; ++i)
    {
        if (distances[i].label.compare("\\tulip") == 0)
            tulip_count++;
        else if (distances[i].label.compare("\\dandelion") == 0)
            dandelion_count++;
        else if (distances[i].label.compare("\\rose") == 0)
            rose_count++;
        else if (distances[i].label.compare("\\sunflower") == 0)
            sunflower_count++;
        else if (distances[i].label.compare("\\daisy") == 0)
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
        cout << "Image " << name << " is: a Daisy with confidence of: " << 100 * double(daisy_count) / K << "%" << endl;
    }
    else if (max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count}) == dandelion_count)
    {
        cout << "Image " << name << " is: a Dandelion with confidence of: " << 100 * double(dandelion_count) / K << "%" << endl;
    }
    else if (max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count}) == rose_count)
    {
        cout << "Image " << name << " is: a Rose with confidence of: " << 100 * double(rose_count) / K << "%" << endl;
    }
    else if (max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count}) == sunflower_count)
    {
        cout << "Image " << name << " is: a Sunflower with confidence of: " << 100 * double(sunflower_count) / K << "%" << endl;
    }
    else if (max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count}) == tulip_count)
    {
        cout << "Image " << name << " is: a Tulip with confidence of: " << 100 * double(tulip_count) / K << "%" << endl;
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
    vector<Image> distances;

    Image test_img;

    glob(test_path, test_fn);
    size_t count = test_fn.size();

    // example array declaration
    //  Image test_images [count];

    for (size_t i = 0; i < count; i++)
    {

        test_img.test_image = imread(test_fn[i]);
        string file_path = test_fn[i];
        size_t last = file_path.find_last_of('\\');
        test_img.label = file_path.substr(last);
        test_dataset.push_back(test_img);
    }

    auto startCompare = steady_clock::now();

    read_images(trainimageFolderLocations, train_dataset, fn);

    for (int j = 0; j < test_dataset.size(); j++)
    {

        auto test_img = test_dataset[j].test_image;

        unsigned char *test_input = (unsigned char *)test_img.data;
        unsigned char *test_output = new unsigned char[test_img.size().width * test_img.size().height];

        const int total_number_of_pixels_test = test_img.rows * test_img.cols * test_img.channels();

        convert_to_gray_scale_serial(test_input, test_output, 0, total_number_of_pixels_test, test_img.channels());

        get_dist(train_dataset, test_output, 0, train_dataset.size(), 0, distances);

        // look into storing the results into an array
        sort(distances.begin(), distances.end(), [](const Image &i1, const Image &i2)
             { return i1.distance < i2.distance; });

        string name = test_dataset[j].label;
    
        int counter = 0;
        compare(distances, K, name);

        distances.clear();
    
        
    }
    
    
    auto endCompare = steady_clock::now();
    auto durationCompare = duration_cast<seconds>(endCompare - startCompare).count();
    cout << "parallel compare duration: " << durationCompare << "seconds" << endl;

    return 0;
}