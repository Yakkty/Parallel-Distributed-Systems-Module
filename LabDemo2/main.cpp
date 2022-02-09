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

ostream &operator<<(ostream &out, const Readings &r)
{
    string time = ctime(&r.timestamp);
    out << "Type: " << r.type << ", Value: " << r.value << ", Timestamp: " << time;
    return out;
}

queue<Readings> buffer;

int counter = 0;

void temperature_values(vector<Readings> &d)
{

    while (true)
    {

        Readings t_val;
        random_device rd;
        default_random_engine eng(rd());
        uniform_real_distribution<double> distr(0, 30);
        srand(time(NULL));

        double rtv = distr(eng);

        t_val.value = rtv;
        t_val.type = "Temperature";
        t_val.timestamp = time(0);

        // d.push_back(t_val);
        Readings read_t(t_val);

        unique_lock<mutex> lock(mu);
        buffer.push(read_t);
        cond.notify_all();
        counter++;
        // lock.unlock();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void light_values(vector<Readings> &d)
{
    while (true)
    {

        Readings l_val;
        random_device rd;
        default_random_engine eng(rd());
        uniform_real_distribution<double> distr(1000, 4000);
        srand(time(NULL));

        double rtv = distr(eng);

        l_val.value = rtv;
        l_val.type = "Light Level";
        l_val.timestamp = time(0);

        // d.push_back(l_val);

        Readings read_l(l_val);
        unique_lock<mutex> lock(mu);

        buffer.push(read_l);
        cond.notify_all();
        counter++;
        // lock.unlock();

        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void smart_home(vector<Readings> &d, int frequency)
{
    while (true)
    {
        unique_lock<mutex> lock(mu);
        cond.wait(lock, []()
                  {
                      return !buffer.empty();
                  });

        auto msg = buffer.front();

        if (msg.type == "Temperature")
        {
            cout << "Current Temperature: " << msg.value << " degrees celsius, "
                 << "Adjusting Thermostat" << endl;
            // this_thread::sleep_for(chrono::seconds(frequency));
        }
        else if (msg.type == "Light Level")
        {
            cout << "Current light level: " << msg.value << ", Adjusting room brightness" << endl;
            // this_thread::sleep_for(chrono::seconds(frequency));
        }
        else
        {
            cout << "No changes";
        }

        buffer.pop();
        // lock.unlock();
        cond.notify_all();
        this_thread::sleep_for(chrono::seconds(frequency));
    }
}

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

    vector<Readings> sensor_data;

    int frequency = atoi(argv[1]);

    thread t1(temperature_values, ref(sensor_data));
    thread t2(light_values, ref(sensor_data));
    thread t3(smart_home, ref(sensor_data), frequency);

    t1.join();
    t2.join();
    t3.join();

    // t_print(sensor_data, frequency);
}