//C++ program to calculate PSNR between two images of equal size


//Imports
#include <iostream>
#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>
#include <chrono>
#include <future>
#include <cmath>


//Namespaces
using namespace cv;
using namespace std;
using namespace std::chrono;


//Converting images to gray scale for simplification, (1 channel instead of 3)
void convert_to_gray_scale_serial(unsigned char *input, unsigned char *output, int start, int end,
                                  int channel)
{

    int j = start;
    int number_of_pixel = end;

    // i = 0 {0, 1, 2}; i = 3 {3, 4, 5}, i = 6
    for (int i = start; i < number_of_pixel; i += channel)
    { // i+=3;
        int blue_value = input[i];
        int green_value = input[i + 1];
        int red_value = input[i + 2];

        output[j++] = (int)(0.114 * blue_value + 0.587 * green_value + 0.299 * red_value);
    }
}

// Function to calculate PSNR serially
double calc_noise_serial(unsigned char *input1, unsigned char *input2, int start, int end)
{


    int number_of_pixel = end;
    double result = 0;

    //Iterate through each pixel and sum the result
    for (int i = start; i < number_of_pixel; ++i)
    {
        result += pow((input1[i] - input2[i]), 2);
    }

    //Calculate mse by dividing the result by the number of pixels
    double mse = result / (number_of_pixel);
    //Calculate psnr
    double psnr = 10.0 * log10((255 * 255) / mse);
    return psnr;

}

// Parallel function to calculate PSNR
void calc_noise_parallel(unsigned char *input1, unsigned char *input2, int start, int end, int depth)
{

    //Divide and conquer strategy, split up the image recursively and compute psnr at specified base case.
    if (depth > 3)
    {
        // equation
        int number_of_pixel = end;
        double result = 0;

        for (int i = start; i < number_of_pixel; ++i)
        {
            result += pow((input1[i] - input2[i]), 2);
        }

        // result is divided by the number of pixels of the segment, not the overall number of pixels of the image, a frustrating place to get stuck
        double mse = result / (end - start);
        double psnr = 10.0 * log10((255 * 255) / mse);
        cout << psnr << endl;
        
    }
    else
    {
        auto mid = (start + end) / 2;


        //Calling left side asynchronously so it can run simultaenously to the right side
        auto left = async(launch::async, calc_noise_parallel, input1, input2, start, mid, depth + 1);

        calc_noise_parallel(input1, input2, mid, end, depth + 1);
        
    }
}

int main(int argc, char **argv)
{
    std::cout << "starting the opencv basic.cpp...." << std::endl;

    //Checking that there are an appropriate number of command line arguments
    if (argc != 3) {
        cout << "Error in arguments..\n";
        exit(-1);
    }

    string img(argv[1]);
    string img2(argv[2]);
    Mat srcImage = imread(img);
    Mat srcImage2 = imread(img2);

    // Checking that images have been provided
    if (!srcImage.data && !srcImage2.data) {
        cout << "Error in reading the image!" << endl;
        return 1;
    }

    // // access the original image data
    unsigned char *input = (unsigned char *)srcImage.data;
    unsigned char *input2 = (unsigned char *)srcImage2.data;

    // // create memory for new images.
    unsigned char *output = new unsigned char[srcImage.size().width * srcImage.size().height];
    unsigned char *output2 = new unsigned char[srcImage2.size().width * srcImage2.size().height];

    // // function call to convert_to_grayscale_serial()
    const int total_number_of_pixels = srcImage.rows * srcImage.cols * srcImage.channels();
    const int total_number_of_pixels2 = srcImage2.rows * srcImage2.cols * srcImage2.channels();
    convert_to_gray_scale_serial(input, output, 0, total_number_of_pixels, srcImage.channels());
    convert_to_gray_scale_serial(input2, output2, 0, total_number_of_pixels2, srcImage2.channels());

    // // create Mat variable, and provide output address.
    Mat gray_image = Mat(srcImage.size().height, srcImage.size().width, CV_8UC1, (unsigned *)output);
    Mat gray_image2 = Mat(srcImage2.size().height, srcImage2.size().width, CV_8UC1, (unsigned *)output2);

    auto start = steady_clock::now();

    //Function call to calculate noise serially, passing the outputs of the greyscale images as inputs, total number of pixels is divided by 3 to get the value for 1 channel
    auto result_serial = calc_noise_serial(output, output2, 0, total_number_of_pixels / 3);

    auto end = steady_clock::now();
    auto duration = duration_cast<microseconds> (end - start).count();
    //Calculate duration for serial implementation

    cout << "serial duration: " << duration << " microseconds" << endl;

    //Cout for debugging serial psnr value
    cout << "psnr: " << result_serial << endl;

    auto start2 = steady_clock::now();

    //Function call to calculate noise parallely, passing outputs of the greyscale images as inputs, and dividing the total number of pixels by 3 to get the value for 1 channel
    //Depth begins at 0
    calc_noise_parallel(output, output2, 0, (total_number_of_pixels / 3), 0);

    auto end2 = steady_clock::now();
    auto duration2 = duration_cast<microseconds> (end2 - start2).count();
    //Duration calculation for parallel inplementation
    cout << "Parallel duration: " << duration2 << " microseconds" << endl;

    namedWindow("srcImage", 1);
    namedWindow("srcImage2", 1);

    //Displaying both images
    imshow("srcImage", gray_image);
    imshow("srcImage2", gray_image2);

    waitKey(0);
    return 0;
}
