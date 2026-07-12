#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include "cpp-httplib/httplib.h"

using CString_ = const std::string;


class Download
{
private:
    std::thread th_;
public:
    using TaskFunc = std::function<void(const std::string&, const std::string&)>;

    ~Download()
    {
        if(th_.joinable())
            th_.join();
    }

    void Wait()
    {
        if(th_.joinable())
            th_.join();
    }

    void dowanload(CString_& host, CString_& path,
        const TaskFunc& callback_word_count)
    {
        std::cout << "线程： " << std::this_thread::get_id() << std::endl;
        httplib::Client cli(host);
        auto response = cli.Get(path);
        if(response && response->status == 200)
        {
            callback_word_count(path, response->body);
        }
    }

    void start_dowanload(CString_& host, CString_& path,
        const TaskFunc& callback_word_count)
    {
        auto func = std::bind(&Download::start_dowanload, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
        th_ = std::thread(func, host, path, callback_word_count);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }



};

int main()
{
    auto word_count = [](CString_& path, CString_& result) -> void
    {
        std::cout << "下载完成 " << path <<": "
            << result.length() << "->" << result.substr(0, 5)
            << std::endl;
    };

    {    
        auto d1 = Download();
        auto d2 = Download();
        auto d3 = Download();
        d1.start_dowanload("http://127.0.0.1:8000", "/novel1.txt", word_count);
        d2.start_dowanload("http://127.0.0.1:8000", "/novel2.txt", word_count);
        d3.start_dowanload("http://127.0.0.1:8000", "/novel3.txt", word_count);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}

