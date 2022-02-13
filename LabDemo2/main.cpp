#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <thread>
#include <random>
#include <windows.h>
#include <cstdlib>
#include <mutex>
#include <queue>
#include <condition_variable>

using namespace std;

mutex mu;
condition_variable cond;

//Structure to store temperature and light values (type, value, time);
struct Readings
{
    string type;
    double value;
    time_t timestamp;

    //Constructor
    Readings(string t = {" "}, double v = 0, time_t ts = 0) : type(t), value(v), timestamp(ts) {}

    //Destructor
    ~Readings() {}

    friend ostream &operator<<(ostream &out, const Readings &r);
};

//Overloaded put to operator to print values for debugging
ostream &operator<<(ostream &out, const Readings &r)
{
    string time = ctime(&r.timestamp);
    out << "Type: " << r.type << ", Value: " << r.value << ", Timestamp: " << time;
    return out;
}

queue<Readings> buffer;

int counter = 0;


// Function to generate temperature values (producer 1) to then push to a buffer for a consumer to retrieve
void temperature_values(vector<Readings> &d)
{
    //While loop so values are consistently being produced
    while (true)
    {

        //Random number generation between values of 0 - 30 (typical room temperature variance)
        Readings t_val;
        random_device rd;
        default_random_engine eng(rd());
        uniform_real_distribution<double> distr(0, 30);
        //seed the rng generation using time
        srand(time(NULL));

        double rtv = distr(eng);

        //Setting values to push to the buffer (originally to a vector for testing)
        t_val.value = rtv;
        t_val.type = "Temperature";
        t_val.timestamp = time(0);

        // d.push_back(t_val);
        Readings read_t(t_val);

        //Mutex to protect buffer, so buffer isnt accessed simultaneously by both producers
        unique_lock<mutex> lock(mu);

        //Values pushed to buffer
        buffer.push(read_t);

        //Notify Consumer
        cond.notify_one();
    
        //Sleep thread to allow for alternation with other producer (light value producer)
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}


//Function to generate light values (producer 2)
void light_values(vector<Readings> &d)
{
    //While loop to continously produce light values
    while (true)
    {
        //Random number generation between 1000 - 4000 (estimation of room light levels in lumens)
        Readings l_val;
        random_device rd;
        default_random_engine eng(rd());
        uniform_real_distribution<double> distr(1000, 4000);

        //Seed rng generation using time
        srand(time(NULL));

        double rtv = distr(eng);

        //Setting values to push to the buffer
        l_val.value = rtv;
        l_val.type = "Light Level";
        l_val.timestamp = time(0);

        // d.push_back(l_val);

        Readings read_l(l_val);
        //Mutex to protect buffer, so it's not accessed simultaenously by the other producer (temperature producer)
        unique_lock<mutex> lock(mu);

        //Pushing values to the buffer
        buffer.push(read_l);
        //Notify sleeping consumer
        cond.notify_one();
    
        
        //Sleep thread to allow for alternation with other producer (temperature value producer)
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

//Smart home function (consumer)
void smart_home(vector<Readings> &d, int frequency)
{   
    //While loop to continously interact with receiving values
    while (true)
    {   
        //Mutex utilised as two threads are interacting with this function
        unique_lock<mutex> lock(mu);
        //Waiting for notification from condition variable
        cond.wait(lock, []()
                  {
                      return !buffer.empty();
                  });

        //Storing front of the buffer in a variable
        auto msg = buffer.front();

        //Determine if the values received are temperature or light levels, and executing accordingly
        if (msg.type == "Temperature")
        {
            cout << "Current Temperature: " << msg.value << " degrees celsius, "
                 << "Adjusting Thermostat" << endl;
            
        }
        else if (msg.type == "Light Level")
        {
            cout << "Current light level: " << msg.value << ", Adjusting room brightness" << endl;
            
        }
        else
        {
            cout << "No changes";
        }

        //Removing item from buffer, shrinking the queue
        buffer.pop();
        
        //Produce values at a frequency provided as a command line argument
        this_thread::sleep_for(chrono::seconds(frequency));
    }
}

//Template function to print readings objects for debugging purposes
template <typename t>
void t_print(vector<t> &v, int frequency)
{
    if (v.empty())
    {
        cout << "V empty" << endl;
        return;
    }

    for (t &i : v)
    {
        cout << i << endl;
        this_thread::sleep_for(chrono::milliseconds(frequency));
    }
}

int main(int argc, char *argv[])
{

    //Vector of type readings
    vector<Readings> sensor_data;

    //Storing desired frequency of readings as a command line argument
    int frequency = atoi(argv[1]);

    //Creating 3 threads for both producers and the single consumer
    thread t1(temperature_values, ref(sensor_data));
    thread t2(light_values, ref(sensor_data));
    thread t3(smart_home, ref(sensor_data), frequency);

    //Joining threads 
    t1.join();
    t2.join();
    t3.join();

}