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

#include "header.hpp"

//Namespaces
using namespace cv;
using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

//Globally declared mutex
mutex mu;


//Divide and conquer function to iterate through train_dataset, convert images to greyscale, calculate Euclidean distance, and store this distance into the vector
void get_dist(vector<Image> &train_dataset, unsigned char *test_output, size_t start, size_t end, int depth, vector<Image> &d)
{   
    Image img;
    //Check depth to continously split up the dataset into desired base cases
    if (depth > 3)
    {   

        double distance = 0;
        size_t count = end;

        //Loop through dataset segment
        for (size_t i = start; i < end; i++)
        {

            //Store train image in variable
            auto image = train_dataset[i].training_image;

            //Store train dataset label in variable
            string label = train_dataset[i].label;

            //access train image data
            unsigned char *train_input = (unsigned char *)image.data;

            //create memory for train image
            unsigned char *train_output = new unsigned char[image.size().width * image.size().height];


            //calculate total number of pixels
            const int total_number_of_pixels = image.rows * image.cols * image.channels();

            //function call to convert train images to greyscale
            convert_to_gray_scale_serial(train_input, train_output, 0, total_number_of_pixels, image.channels());


            //function call to calculate euclidean distance and store in distance variable
            double distance = calc_euc_dist_serial(train_output, test_output, 0, (total_number_of_pixels / 3));

            //Lock mutex to push distance and label to distances vector
            unique_lock<mutex> lock(mu);
            img.distance = distance;
            img.label = label;

            d.push_back(img);
            //unique lock automatically releases mutex when leaves scope


            //Delete train output memory to avoid memory leaks
            delete train_output;
        }
    }
    //If depth !=3, split datasaet into two and recursively call function, setting left half to be async to run simultaenously with the right side
    else
    {
        //Increment depth by 1 each time to edge closer to desired base case
        size_t mid = (start + end) / 2;
        //recursive function calls
        auto left = async(launch::async, get_dist, ref(train_dataset), test_output, start, mid, depth + 1, ref(d));
        get_dist(train_dataset, test_output, mid, end, depth + 1, d);
    }
}

//function for KNN calculation 
void compare(vector<Image> &distances, int K, string name)
{

//Variables to hold count values for each label
    int daisy_count = 0;
    int dandelion_count = 0;
    int rose_count = 0;
    int sunflower_count = 0;
    int tulip_count = 0;

//Iterate through distances vector relative to K value
    for (int i = 0; i < K; ++i)
    {  
        //Increment respective counts
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
    //Debugging couts
    cout << "Daisy count: " << daisy_count << endl;
    cout << "Dand count: " << dandelion_count << endl;
    cout << "Rose count: " << rose_count << endl;
    cout << "Sun count: " << sunflower_count << endl;
    cout << "Tul count: " << tulip_count << endl;


//Else if statements to output estimated classification of test images, with % confidence of result
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

//Main method

int main(int argc, char **argv)
{

  //Checking that there are an appropriate number of command line arguments
    if (argc != 3) {
        cout << "Error in arguments..\n";
        exit(-1);
    }

//Storing path to train folders as 1st command line argument
    String train_path(argv[1]);

//Storing folder path to test image as user input
    String test_path;

//Store K value as 2nd command line argument
    int K = stoi(argv[2]);

//Getting test folder path
    cout << "Enter a folder of images ";
    cin >> test_path;
    cout << test_path << endl;

    auto startCompare = steady_clock::now();

//Vector to store training filenames and test image filenames
    vector<String> fn;
    vector<String> test_fn;

//Vector to store training subfolders i.e daisy folder, sunflower folder etc
    vector<fs::path> trainimageFolderLocations;

//foreach to push these locations to image folder locations vector
    for (const fs::directory_entry &dir_entry : fs::directory_iterator(train_path))
    {
        trainimageFolderLocations.push_back(dir_entry);
    }

//Vectors for training data, test images and distances
    vector<Image> train_dataset;
    vector<Image> test_dataset;
    vector<Image> distances;

//Create test_img object of type image 
    Image test_img;

    glob(test_path, test_fn);
    size_t count = test_fn.size();

    // example array declaration
    //  Image test_images [count];

//For loop to store testing images and names in test dataset vector
    for (size_t i = 0; i < count; i++)
    {

        test_img.test_image = imread(test_fn[i]);
        string file_path = test_fn[i];
        size_t last = file_path.find_last_of('\\');
        test_img.label = file_path.substr(last);
        test_dataset.push_back(test_img);
    }


//Calling read images function to store training images and labels
    read_images(trainimageFolderLocations, train_dataset, fn);

//Iterate through each testing image
    for (int j = 0; j < test_dataset.size(); j++)
    {

        //Store test image in variable
        auto test_img = test_dataset[j].test_image;

        //access test image data
        unsigned char *test_input = (unsigned char *)test_img.data;

        //create memory for new images
        unsigned char *test_output = new unsigned char[test_img.size().width * test_img.size().height];

        //Calculate total number of pixels
        const int total_number_of_pixels_test = test_img.rows * test_img.cols * test_img.channels();

        //Convert to greyscale function call for distance comparison
        convert_to_gray_scale_serial(test_input, test_output, 0, total_number_of_pixels_test, test_img.channels());

        //Function call to calculate distance and store in distances array
        get_dist(train_dataset, test_output, 0, train_dataset.size(), 0, distances);

        //sort distance vector based on distance
        sort(distances.begin(), distances.end(), [](const Image &i1, const Image &i2)
             { return i1.distance < i2.distance; });

        //store test image name in a variable for couts with KNN classification
        string name = test_dataset[j].label;
    
        //function call for KNN classification
        compare(distances, K, name);

        //Clear distances vector to not pollute comparison for each subsequent test image
        distances.clear(); 
    
        
    }
    
    
    auto endCompare = steady_clock::now();
    auto durationCompare = duration_cast<seconds>(endCompare - startCompare).count();
    cout << "parallel compare duration: " << durationCompare << "seconds" << endl;

    return 0;
}