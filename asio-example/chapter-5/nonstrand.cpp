/* nonstrand.cpp */
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <iostream>

boost::mutex global_stream_lock;

void WorkerThread(boost::shared_ptr<boost::asio::io_service> iosvc, int counter)
{
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " Start.\n";
    global_stream_lock.unlock();
    iosvc->run();
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " End.\n";
    global_stream_lock.unlock();
}

void Print(int number)
{
    boost::this_thread::sleep(boost::posix_time::milliseconds(3000));
    std::cout << "Number: " << number << std::endl;
}

void Print2(int &number)
{
    boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
    std::cout << "Number P: " << number << std::endl;
}

int main(void)
{
    boost::shared_ptr<boost::asio::io_service> io_svc(new boost::asio::io_service);
    boost::shared_ptr<boost::asio::io_service::work> worker(new boost::asio::io_service::work(*io_svc));

    global_stream_lock.lock();
    std::cout << "The program will exit once all work has finished.\n ";
    global_stream_lock.unlock();

    boost::thread_group threads;
    for (int i = 1; i <= 5; i++)
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));

    for (int i = 0; i < 7; ++i)
    {
        io_svc->post(boost::bind(&Print, i));
        std::cout << "[ POST ] " << i << std::endl;
    }
    io_svc->post(boost::bind(&Print, 10));
    io_svc->post(boost::bind(&Print, 20));
    io_svc->post(boost::bind(&Print, 30));
    io_svc->post(boost::bind(&Print, 40));
    io_svc->post(boost::bind(&Print, 50));

    int i = 0;
    io_svc->post(boost::bind(&Print2, boost::ref(i)));

    std::cout << "[ BEFORE ][ reset ]" << std::endl;
    worker.reset();
    std::cout << "[ AFTER ][ reset ]" << std::endl;
    threads.join_all();
    return 0;
}