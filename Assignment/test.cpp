#include <iostream>
#include <cmath>
#include <math.h>
#include <bits/stdc++.h>

using namespace std;

int main() {

    int daisy_count = 5;
    int dandelion_count = 7;
    int rose_count = 2;
    int sunflower_count = 14;
    int tulip_count = 1;

    auto estimated_label = max({daisy_count, dandelion_count, rose_count, sunflower_count, tulip_count});

    cout << estimated_label << endl;
}