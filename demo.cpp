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
        results.push_back(tp.push([i]()
                                  {
                                      std::this_thread::sleep_for(std::chrono::seconds(i));
                                      return (int)i;
                                  }));
    }
    for (size_t i = 0; i < 4; i++)
    {
        auto r = results[i].get();
        std::cout << "func " << r << " return " << r << std::endl;
    }
    return 0;
}