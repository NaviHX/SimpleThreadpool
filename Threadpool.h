#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <future>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <stdexcept>

class Threadpool
{
public:
    Threadpool(size_t size = 2);
    Threadpool(Threadpool &tp) = delete;
    Threadpool(Threadpool &&tp) = delete;
    Threadpool &operator=(const Threadpool &tp) = delete;
    ~Threadpool();

    bool empty() { return task_q.empty(); }
    bool stopped() { return stop; }

    template <class F, class... Args>
    std::future<typename std::result_of<F(Args...)>::type> push(F &&f, Args&&... args);

private:
    bool stop;
    std::mutex mtx;
    std::condition_variable cv;

    std::vector<std::thread> thread_v;
    std::queue<std::function<void()>> task_q;
};

Threadpool::Threadpool(size_t size) : stop(false)
{
    for (size_t i = 0; i < size; i++)
    {
        thread_v.emplace_back([this]()
                              {
                                  while (1)
                                  {
                                      std::function<void()> task;
                                      {
                                          std::unique_lock<std::mutex> lock(this->mtx);
                                          cv.wait(lock, [this]()
                                                  { return this->stopped() || !this->empty(); });
                                          if (this->stopped() && this->empty())
                                              return;
                                          task = this->task_q.front();
                                          this->task_q.pop();
                                      }
                                      task();
                                  }
                              });
    }
}

Threadpool::~Threadpool()
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();
    for (size_t i = 0; i < thread_v.size(); i++)
        thread_v[i].join();
}

template <class F, class... Args>
std::future<typename std::result_of<F(Args...)>::type> Threadpool::push(F &&f, Args &&...args)
{
    using ret_type = typename std::result_of<F(Args...)>::type;
    auto taskp = std::make_shared<std::packaged_task<ret_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<ret_type> ret = taskp->get_future();

    {
        std::unique_lock<std::mutex> lock(mtx);
        if (stopped() == true)
            throw std::runtime_error("cannot add new tasks in a stopped pool");
        task_q.push([taskp]()
                    { (*taskp)(); });
    }
    cv.notify_one();
    return ret;
}

#endif