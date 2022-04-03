/* clienthttpget.cpp */
#include "wrapper.h"
#ifdef _WIN32
#include <conio.h>
#else
#include "utils.h"
#endif // _WIN32
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

boost::mutex global_stream_lock;

class MyConnection : public Connection
{
private:
    void OnAccept(const std::string &host, uint16_t port)
    {
        global_stream_lock.lock();
        std::cout << "[OnAccept] " << host << ":" << port << "\n";
        global_stream_lock.unlock();
        // Start the next receive
        Recv();
    }
    void OnConnect(const std::string &host, uint16_t port)
    {
        global_stream_lock.lock();
        std::cout << "[OnConnect] " << host << ":" << port << "\n";
        global_stream_lock.unlock();
        // Start the next receive
        Recv();
        std::string str = "GET / HTTP/1.0\r\n\r\n";
        std::vector<uint8_t> request;
        std::copy(str.begin(), str.end(), std::back_inserter(request));
        Send(request);
    }
    void OnSend(const std::vector<uint8_t> &buffer)
    {
        global_stream_lock.lock();
        std::cout << "[OnSend] " << buffer.size() << " bytes\n";
        for (size_t x = 0; x < buffer.size(); x++)
        {
            std::cout << (char)buffer[x];
            if ((x + 1) % 16 == 0)
                std::cout << "\n";
        }
        std::cout << "\n";
        global_stream_lock.unlock();
    }
    void OnRecv(std::vector<uint8_t> &buffer)
    {
        global_stream_lock.lock();
        std::cout << "[OnRecv] " << buffer.size() << " bytes\n";
        for (size_t x = 0; x < buffer.size(); x++)
        {
            std::cout << (char)buffer[x];
            if ((x + 1) % 16 == 0)
                std::cout << "\n";
        }
        std::cout << "\n";
        global_stream_lock.unlock();
        // Start the next receive
        Recv();
    }
    void OnTimer(const boost::posix_time::time_duration &delta)
    {
        global_stream_lock.lock();
        std::cout << "[OnTimer] " << delta << std::endl;
        global_stream_lock.unlock();
    }
    void OnError(const boost::system::error_code &error)
    {
        global_stream_lock.lock();
        std::cout << "[OnError] " << error << "\n";
        global_stream_lock.unlock();
    }

public:
    MyConnection(boost::shared_ptr<Hive> hive) : Connection(hive) {}
    ~MyConnection() {}
};

int main(void)
{
    boost::shared_ptr<Hive> hive(new Hive());
    boost::shared_ptr<MyConnection> connection(new MyConnection(hive));
    connection->Connect("www.packtpub.com", 80);
#ifdef _WIN32
    while (!_kbhit())
#else
    while (!kbhit())
#endif // _WIN32
    {
        hive->Poll();
        boost::this_thread::sleep_for(boost::chrono::milliseconds{1000});
    }
    hive->Stop();
    return 0;
}