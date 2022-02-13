#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>

using namespace std;

// Structure to store CSV dataset
struct Flower
{
    // initalize variables representing objects in a csv row
    int id;
    double sepal_length;
    double sepal_width;
    double petal_length;
    double petal_width;
    string species;

    // constructor
    Flower(int fid = 0, double psl = 0, double psw = 0, double ppl = 0, double ppw = 0, string ps = {" "}) : id(fid), sepal_length(psl), sepal_width(psw), petal_length(ppl),
                                                                                                             petal_width(ppw), species(ps) {}

    // copy constructor
    Flower(const Flower &rhs) : id(rhs.id), sepal_length(rhs.sepal_length), sepal_width(rhs.sepal_width),
                                petal_length(rhs.petal_length), petal_width(rhs.petal_width), species(rhs.species) {}

    // overload assignment operator
    Flower &operator=(const Flower &rhs)
    {
        id = rhs.id;
        sepal_length = rhs.sepal_length;
        sepal_width = rhs.sepal_width;
        petal_length = rhs.petal_length;
        petal_width = rhs.petal_width;
        species = rhs.species;

        return *this;
    }

    //Friend functions
    //put to operator
    friend ostream &operator<<(ostream &out, const Flower &f);

    friend bool operator==(const Flower &lhs, const Flower &rhs);

    friend bool operator!=(const Flower &lhs, const Flower &rhs);

    // destructor
    ~Flower() {}
};

// overload put to (<<) operator to print objects to a standard output
ostream &operator<<(ostream &out, const Flower &f)
{
    out << "( id: " << f.id << ", sepal length: " << f.sepal_length << ", sepal width: " << f.sepal_width << ", petal_length: " << f.petal_length << ", petal_width: "
        << f.petal_width << ", species: " << f.species << " )";
    return out;
}

// Overload == operator for object comparison
bool operator==(const Flower &lhs, const Flower &rhs)
{
    return (lhs.petal_length == rhs.petal_length &&
            lhs.petal_width == rhs.petal_width &&
            lhs.sepal_width == rhs.sepal_width &&
            lhs.sepal_length == rhs.sepal_length &&
            lhs.species == rhs.species);
}

//Overload != operator for object comparison
bool operator!=(const Flower &lhs, const Flower &rhs)
{
    return !(lhs == rhs);
}

// Function to read CSV data and store in a structure, to then later store in a vector
void readCSVFile(const string filename, vector<Flower> &data)
{

    ifstream file_handler(filename);

    string line{};
    int line_counter{0};

    while (getline(file_handler, line))
    {

        if (line_counter < 1)
        {
            ++line_counter;
            continue;
        }

        try
        {
            stringstream ss(line);
            int counter{0};

            Flower f;
            while (ss.good())
            {
                string value{};
                getline(ss, value, ',');

                f.id = counter;
                if (counter == 0)
                    f.sepal_length = stod(value);
                else if (counter == 1)
                    f.sepal_width = stod(value);
                else if (counter == 2)
                    f.petal_length = stod(value);
                else if (counter == 3)
                    f.petal_width = stod(value);
                else if (counter == 4)
                    f.species = value;

                ++counter;
            }
            data.push_back(f);
        }
        catch (std::exception &e)
        {
            cerr << "Error parsing csv file" << e.what() << endl;
        }

        ++line_counter;
    }
}

//Template function to print objects to a standard output
template <typename T>
void f_print(vector<T> &v)
{
    if (v.empty())
        return;

    for (T &i : v)
        cout << i << endl;
}

int main(int argc, char *argv[])
{

    string filename = argv[1];

    // cout << "Enter the file path: ";
    // cin >> filename;

    cout << "filename: " << filename << endl;

    vector<Flower> flower_data;

    //Calling read csv function, passing the filename provided by the user and a vector of type <Flower> for the data to be stored in

    readCSVFile(filename, flower_data);

    // Print function called, passing flower data to print flower objects
    f_print(flower_data);

    // Comparing data objects example
    if (flower_data[5].sepal_length == flower_data[9].sepal_length)
        cout << "Sepal lengths are equal \n";
    else
        cout << "Sepal lengths are unequal \n";

    // functions for finding min and max values of Flower attributes
    auto minPetalLength = min_element(flower_data.begin(), flower_data.end(), [](const Flower &a, const Flower &b)
                                      { return a.petal_length < b.petal_length; });

    auto maxPetalLength = max_element(flower_data.begin(), flower_data.end(), [](const Flower &a, const Flower &b)
                                      { return a.petal_length < b.petal_length; });

    auto minPetalWidth = min_element(flower_data.begin(), flower_data.end(), [](const Flower &a, const Flower &b)
                                     { return a.petal_width < b.petal_width; });

    auto maxPetalWidth = max_element(flower_data.begin(), flower_data.end(), [](const Flower &a, const Flower &b)
                                     { return a.petal_width < b.petal_width; });

    auto minSepalLength = min_element(flower_data.begin(), flower_data.end(), [](const Flower &a, const Flower &b)
                                      { return a.sepal_length < b.sepal_length; });

    auto maxSepalLength = max_element(flower_data.begin(), flower_data.end(), [](const Flower &a, const Flower &b)
                                      { return a.sepal_length < b.sepal_length; });

    auto minSepalWidth = min_element(flower_data.begin(), flower_data.end(), [](const Flower &a, const Flower &b)
                                     { return a.sepal_width < b.sepal_width; });

    auto maxSepalWidth = max_element(flower_data.begin(), flower_data.end(), [](const Flower &a, const Flower &b)
                                     { return a.sepal_width < b.sepal_width; });

    cout << "Smallest Petal Length is : " << minPetalLength->petal_length << " Largest Petal Length is : " << maxPetalLength->petal_length << "\n";
    cout << "Smallest Petal Width is : " << minPetalWidth->petal_width << " Largest Petal Width is : " << maxPetalWidth->petal_width << "\n";
    cout << "Smallest Sepal Width is : " << minSepalWidth->sepal_width << " Largest Sepal Width is : " << maxSepalWidth->sepal_width << "\n";
    cout << "Smallest Sepal Length is : " << minSepalLength->sepal_length << " Largest Sepal Length is : " << maxSepalWidth->sepal_length << "\n";

    // Creating a new flower object using copy constructor based off of an already existing flower obtained from CSV reading
    Flower f2(flower_data[3]);

    cout << "F2 petal length: " << f2.petal_length << ", petal width: " << f2.petal_width << ", sepal length: " << f2.sepal_length
         << ", sepal width: " << f2.sepal_width << ", species: " << f2.species << endl;

    return 0;
}