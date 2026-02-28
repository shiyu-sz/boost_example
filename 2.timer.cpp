#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/timer/timer.hpp>
#include <thread>
#include <boost/timer/progress_display.hpp>

void sync_timer()
{
    boost::asio::io_context io;
    // 创建一个3秒后超时的定时器
    boost::asio::deadline_timer timer(io, boost::posix_time::seconds(3));
    std::cout << "开始等待: " << boost::posix_time::second_clock::local_time() << std::endl;
    // 同步等待（阻塞当前线程）
    timer.wait();
    std::cout << "定时器触发: " << boost::posix_time::second_clock::local_time() << std::endl;
}

void async_timeout_handle(const boost::system::error_code& ec) {
    if (!ec) {
        std::cout << "定时器触发: " << boost::posix_time::second_clock::local_time() << std::endl;
    } else {
        std::cout << "错误: " << ec.message() << std::endl;
    }
}

void async_timer()
{
    boost::asio::io_context io;
    // 创建2秒后超时的定时器
    boost::asio::deadline_timer timer(io, boost::posix_time::seconds(2));
    std::cout << "开始异步等待: " << boost::posix_time::second_clock::local_time() << std::endl;
    // 异步等待（非阻塞
    timer.async_wait(&async_timeout_handle);
    std::cout << "异步等待已启动，继续执行其他操作..." << std::endl;
    // 运行事件循环（必须调用才能触发异步操作）
    io.run();  // 阻塞直到所有异步操作完成
}

void system_timer()
{
    boost::asio::io_context io;
    // 使用 system_timer 和 C++11 的 chrono
    boost::asio::system_timer timer(io, std::chrono::seconds(2));
    std::cout << "开始等待..." << std::endl;
    timer.async_wait([](const boost::system::error_code& ec) {
        if (!ec) {
            std::cout << "定时器触发!" << std::endl;
        }
    });
    io.run();
}

void auto_cpu_timer()
{
    {
        boost::timer::auto_cpu_timer t;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void progress_display()
{
    std::vector<int> numbers(100);
    std::cout << "处理数据中..." << std::endl;
    // 创建进度显示器，总工作量为100
    boost::timer::progress_display pd(numbers.size());
    for (size_t i = 0; i < numbers.size(); ++i) {
        // 模拟一些处理工作
        numbers[i] = i * i;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // 增加进度计数 - 使用前置++运算符
        ++pd;
    }
    std::cout << "处理完成！" << std::endl;
}

void date_ptime()
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;
    
    // 1. 创建日期对象
    date d1(2024, 12, 25);                     // 圣诞节
    date d2 = from_string("2025-01-01");       // 元旦
    date d3 = day_clock::local_day();           // 获取当前本地日期

    std::cout << "d1: " << d1 << std::endl;
    std::cout << "d2: " << d2 << std::endl;
    std::cout << "Today: " << d3 << std::endl;

    // 2. 日期计算和时长
    days duration = d2 - d1;                     // 计算日期差
    std::cout << "Days between d2 and d1: " << duration.days() << std::endl;

    date d4 = d1 + weeks(2);                     // 在当前日期上加两周
    std::cout << "d1 + 2 weeks: " << d4 << std::endl;

    // 3. 获取日期各部分
    std::cout << "Year: " << d1.year() 
              << ", Month: " << d1.month().as_number() 
              << ", Day: " << d1.day() << std::endl;

    // 4. 日期周期
    date_period p1(d1, days(7));                 // 定义一个从d1开始，为期一周的周期
    date_period p2(date(2024,12,24), date(2024,12,26)); // 平安夜到节礼日

    if (p1.intersects(p2)) {
        std::cout << "Periods intersect." << std::endl;
        date_period overlap = p1.intersection(p2);
        std::cout << "Overlap period: " << overlap << std::endl;
    }

    ptime t2 = time_from_string("2024-05-20 15:45:00"); // 2024-05-20 15:45:00
    std::cout << "t2: " << t2 << std::endl;
}

int main() {
    // sync_timer();   // 同步定时器
    // async_timer();  // 异步定时器
    // system_timer();  // C++11的定时器
    // auto_cpu_timer();   // 计时器
    // progress_display();     // 显示处理进度
    date_ptime();       // 时间日期
    return 0;
}