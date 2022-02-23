#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <chrono>
#include <cmath>
#include <math.h>
#include <bits/stdc++.h>

#include "header.hpp"

using namespace cv;
using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;



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

    auto start = steady_clock::now();

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

//For loop to store testing images and names in test dataset vector
    for (size_t i = 0; i < count; i++)
    {
       test_img.test_image = imread(test_fn[i]);
        string file_path = test_fn[i];
        size_t last = file_path.find_last_of('\\');
        test_img.label = file_path.substr(last);
        test_dataset.push_back(test_img);
        // cout << "image pushed to test dataset" << endl;
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

        //Create dist_obj of type image to be able to push distances and labels to distances vector
        Image dist_obj;


        for (int i = 0; i < train_dataset.size(); i++)
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

            // Convert training images to grayscale
            convert_to_gray_scale_serial(train_input, train_output, 0, total_number_of_pixels, train_dataset[i].training_image.channels());

          
            //Push distance and label to distances vector
            dist_obj.distance = calc_euc_dist_serial(train_output, test_output, 0, (total_number_of_pixels / 3));
            dist_obj.label = label;
            distances.push_back(dist_obj);

            //no need to delete train output memory as it was created in the stack
        }

        //sort distance vector based on distance
        sort(distances.begin(), distances.end(), [](const Image &i1, const Image &i2)
             { return i1.distance < i2.distance; });

        
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

        //Storing max label value in variable
        auto estimated_label = max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count});
       
        //Debugging couts
        cout << "Daisy count: " << daisy_count << endl;
        cout << "Dand count: " << dandelion_count << endl;
        cout << "Rose count: " << rose_count << endl;
        cout << "Sun count: " << sunflower_count << endl;
        cout << "Tul count: " << tulip_count << endl;
        cout << estimated_label << endl;

        //Else if statementst to output estimated classification of test images with % confidence of result
        if (estimated_label == daisy_count)
        {
            cout << "Image " << test_dataset[j].label << " is: a Daisy with confidence of: " << 100 * double(daisy_count) / K << "%" << endl;
        }
        else if (estimated_label == dandelion_count)
        {
            cout << "Image " <<  test_dataset[j].label << " is: a Dandelion with confidence of: " << 100 * double(dandelion_count) / K << "%" << endl;
        }
        else if (estimated_label == rose_count)
        {
            cout << "Image " <<  test_dataset[j].label << " is: a Rose with confidence of: " << 100 * double(rose_count) / K << "%" << endl;
        }
        else if (estimated_label == sunflower_count)
        {
            cout << "Image " <<  test_dataset[j].label << " is: a Sunflower with confidence of: " << 100 * double(sunflower_count) / K << "%" << endl;
        }
        else if (estimated_label == tulip_count)
        {
            cout << "Image " <<  test_dataset[j].label << " is: a Tulip with confidence of: " << 100 * double(tulip_count) / K << "%" << endl;
        }
        else
        {
            cout << "Use an odd k value";
        }

        //clear distances vector to not pollute comparison for each subsequent test image
        distances.clear();
    }

    auto end = steady_clock::now();
    auto duration = duration_cast<seconds>(end - start).count();

    cout << "serial duration: " << duration << " seconds" << endl;


    return 0;
}