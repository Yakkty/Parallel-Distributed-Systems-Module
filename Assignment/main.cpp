#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <chrono>
#include <cmath>
#include <math.h>
#include <bits/stdc++.h>

using namespace cv;
using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

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

// convert to grayscale
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

void read_images(vector<fs::path> &ifl, vector<Image> &td, vector<String> fn)
{
    Image img;

    for (auto i = 0; i < ifl.size(); ++i)
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

double calc_euc_dist(unsigned char *input1, unsigned char *unknown, int start, int end)
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

int main(int argc, char **argv)
{

    String train_path(argv[1]);
    // string testImage;
    String test_path;

    int K = stoi(argv[2]);

    // cout << "Enter an image ";
    // cin >> testImage;
    cout << "Enter a folder of images ";
    cin >> test_path;
    cout << test_path << endl;

    // Mat testImg = imread(testImage);

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
        // cout << "image pushed to test dataset" << endl;
    }

    read_images(trainimageFolderLocations, train_dataset, fn);
    auto start = steady_clock::now();

    for (int j = 0; j < test_dataset.size(); j++)
    {

        auto test_img = test_dataset[j].test_image;

        unsigned char *test_input = (unsigned char *)test_img.data;
        unsigned char *test_output = new unsigned char[test_img.size().width * test_img.size().height];

        const int total_number_of_pixels_test = test_img.rows * test_img.cols * test_img.channels();

        // Mat gray_image_test = Mat(test_img.size().height, test_img.size().width, CV_8UC1, (unsigned *)test_output);

        convert_to_gray_scale_serial(test_input, test_output, 0, total_number_of_pixels_test, test_img.channels());

        for (int i = 0; i < train_dataset.size(); i++)
        {
            auto image = train_dataset[i].training_image;
            // string label = train_dataset[i].label;
            unsigned char *train_input = (unsigned char *)image.data;
            unsigned char *train_output = new unsigned char[image.size().width * image.size().height];

            const int total_number_of_pixels = image.rows * image.cols * image.channels();

            // Convert training images to grayscale

            convert_to_gray_scale_serial(train_input, train_output, 0, total_number_of_pixels, train_dataset[i].training_image.channels());

            // Mat gray_image = Mat(image.size().height, image.size().width, CV_8UC1, (unsigned *)train_output);

            // train_dataset[i].gray_image = gray_image;
            // train_dataset[i].label = label;

            train_dataset[i].distance = calc_euc_dist(train_output, test_output, 0, (total_number_of_pixels / 3));
        }

        sort(train_dataset.begin(), train_dataset.end(), [](const Image &i1, const Image &i2)
             { return i1.distance < i2.distance; });

        int daisy_count = 0;
        int dandelion_count = 0;
        int rose_count = 0;
        int sunflower_count = 0;
        int tulip_count = 0;

        for (int i = 0; i < K; ++i)
        {
            if (train_dataset[i].label.compare("\\tulip") == 0)
                tulip_count++;
            else if (train_dataset[i].label.compare("\\dandelion") == 0)
                dandelion_count++;
            else if (train_dataset[i].label.compare("\\rose") == 0)
                rose_count++;
            else if (train_dataset[i].label.compare("\\sunflower") == 0)
                sunflower_count++;
            else if (train_dataset[i].label.compare("\\daisy") == 0)
                daisy_count++;
            else
            {
            }
        }

        auto estimated_label = max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count});

        cout << "Daisy count: " << daisy_count << endl;
        cout << "Dand count: " << dandelion_count << endl;
        cout << "Rose count: " << rose_count << endl;
        cout << "Sun count: " << sunflower_count << endl;
        cout << "Tul count: " << tulip_count << endl;
        cout << estimated_label << endl;

        if (estimated_label == daisy_count)
        {
            cout << "Image " << j << " is: a Daisy with confidence of: " << 100 * double(daisy_count) / K << "%" << endl;
        }
        else if (estimated_label == dandelion_count)
        {
            cout << "Image " << j << " is: a Dandelion with confidence of: " << 100 * double(dandelion_count) / K << "%" << endl;
        }
        else if (estimated_label == rose_count)
        {
            cout << "Image " << j << " is: a Rose with confidence of: " << 100 * double(rose_count) / K << "%" << endl;
        }
        else if (estimated_label == sunflower_count)
        {
            cout << "Image " << j << " is: a Sunflower with confidence of: " << 100 * double(sunflower_count) / K << "%" << endl;
        }
        else if (estimated_label == tulip_count)
        {
            cout << "Image " << j << " is: a Tulip with confidence of: " << 100 * double(tulip_count) / K << "%" << endl;
        }
        else
        {
            cout << "Use an odd k value";
        }
    }

    auto end = steady_clock::now();
    auto duration = duration_cast<seconds>(end - start).count();

    cout << "serial duration: " << duration << " seconds" << endl;

    // sort dataset

    return 0;
}