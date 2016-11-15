#include <iostream>
#include <thread>
#include <functional>
#include <mutex>

static int g_apple = 100;
std::mutex g_apple_mutex;

void ms_sleep(int millisecs)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(millisecs));
}

int sell(int n)
{
    std::cout << "thread arg, n:" << n << std::endl;

    while(true)
    {
        std::lock_guard<std::mutex> lock(g_apple_mutex);
        if(g_apple <= 0)
        {
            break;
        }
        g_apple -= 2;
        std::cout << "sold 2 apple by thread:" << std::this_thread::get_id() << ", apple:" << g_apple << std::endl;

        ms_sleep(200);
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    std::thread t1(sell, 11);
    std::thread t2(sell, 12);

    t1.join();
    t2.join();

    return 0;
}