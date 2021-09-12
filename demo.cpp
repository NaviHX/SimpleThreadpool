#include <iostream>
#include <chrono>
#include <future>
#include <functional>
#include "Threadpool.h"

int main()
{
    Threadpool tp(4);
    std::vector<std::future<int>> results;
    for (size_t i = 0; i < 4; i++)
    {
        results.push_back(tp.push([](int i)
                                  { std::this_thread::sleep_for(std::chrono::seconds(i)); return i; },
                                  i));
    }
    for (size_t i = 0; i < 4; i++)
    {
        auto r = results[i].get();
        std::cout << "func " << r << " return" << std::endl;
    }
    return 0;
}