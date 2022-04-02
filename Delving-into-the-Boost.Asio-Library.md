- Bây giờ ta có thể chạy đối tượng `io_service` và cung cấp cho nó một số **work** phải làm, đã đến lúc ta tìm hiểu thêm về các đối tượng khác trong thư viện `Boost.Asio` để phát triển ứng dụng mạng. Tất cả các hoạt động của đối tượng `io_service` mà chúng tôi đã sử dụng trước đây được chạy không đồng bộ nhưng không theo thứ tự được tuần tự hóa, có nghĩa là chúng tôi không thể xác định thứ tự **work** của đối tượng `io_service` sẽ được chạy. Ngoài ra, chúng tôi phải xem xét những gì chúng tôi sẽ làm nếu ứng dụng của chúng tôi gặp bất kỳ lỗi nào trong thời gian chạy và suy nghĩ về khoảng thời gian khi chạy bất kỳ **work** đối tượng `io_service` nào. Do đó, trong chương này, ta sẽ thảo luận về các chủ đề sau:

    - Thực hiện tuần tự **work** của đối tượng `io_service`
    - Nắm bắt (các) ngoại lệ và xử lý chúng đúng cách
    - Thực hiện **work** trong khoảng thời gian mong muốn

# Serializing the I/O service work

- Giả sử ta muốn xếp hàng các **work** cần làm nhưng thứ tự là quan trọng. Nếu ta chỉ áp dụng phương thức không đồng bộ, ta sẽ không biết thứ tự **work** mà ta sẽ nhận được. ta cần đảm bảo rằng thứ tự **work** là thứ ta muốn và đã thiết kế nó. Ví dụ: nếu chúng tôi `post` **work** A, **work** B và **work** C, theo thứ tự đó, chúng tôi muốn giữ thứ tự đó trong thời gian chạy.

## Using the strand function

- **Strand** là một lớp trong đối tượng `io_service` cung cấp tuần tự hóa thực thi trình xử lý. Nó có thể được sử dụng để đảm bảo **work** chúng ta có sẽ được thực hiện theo trình tự. Chúng ta hãy kiểm tra đoạn code sau để hiểu tuần tự hóa bằng cách sử dụng hàm **strand**. Nhưng trước tiên, chúng ta sẽ bắt đầu mà không sử dụng các hàm `strand()` và `lock()`:

```cpp
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
```

## Wrapping a handler through the strand object

- Có một hàm trong `boost::asio::strand` được gọi là phương thức `wrap()`. Dựa trên tài liệu **Boost** chính thức, nó tạo ra một đối tượng **handler** mới, cái mà sẽ tự động truyển **wrapped handler** đến hàm `dispatch` của đối tượng `strand` khi nó được gọi. Chúng ta hãy xem đoạn code sau để giải thích nó:

```cpp
/* strandwrap.cpp */
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
    std::cout << "Number: " << number << std::endl;
}

int main(void)
{
    boost::shared_ptr<boost::asio::io_service> io_svc(new boost::asio::io_service);
    boost::shared_ptr<boost::asio::io_service::work> worker(new boost::asio::io_service::work(*io_svc));
    boost::asio::io_service::strand strand(*io_svc);

    global_stream_lock.lock();
    std::cout << "The program will exit once all work has finished." << std::endl;
    global_stream_lock.unlock();

    boost::thread_group threads;
    for (int i = 1; i <= 5; i++)
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));

    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    io_svc->post(strand.wrap(boost::bind(&Print, 1)));
    io_svc->post(strand.wrap(boost::bind(&Print, 2)));
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    io_svc->post(strand.wrap(boost::bind(&Print, 3)));
    io_svc->post(strand.wrap(boost::bind(&Print, 4)));
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    io_svc->post(strand.wrap(boost::bind(&Print, 5)));
    io_svc->post(strand.wrap(boost::bind(&Print, 6)));

    worker.reset();
    threads.join_all();
    return 0;
}
```

- Mặc dù `work` được đảm bảo sẽ được thực hiện nối tiếp, nhưng nó không được đảm bảo thứ tự của công việc nào thực sự diễn ra bởi **built-in handler wrapper**. Và nếu thứ tự thực sự quan trọng, chúng ta phải xem xét ** built-in handler wrapper** khi sử dụng đối tượng `strand`.

# Handling exceptions and errors

## Handling an exception

- **An exception** là một cách phản ứng với một tình huống trong đó code có những trường hợp ngoại lệ bằng cách chuyển quyền kiểm soát cho **handler**. Để xử lý ngoại lệ, chúng ta cần sử dụng khối try-catch trong code của mình; sau đó, nếu một trường hợp ngoại lệ phát sinh, một ngoại lệ sẽ được ném cho **exception handler**.

```cpp
/* exception.cpp */
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
    try
    {
        iosvc->run();
        global_stream_lock.lock();
        std::cout << "Thread " << counter << " End.\n";
        global_stream_lock.unlock();
    }
    catch (std::exception &ex)
    {
        global_stream_lock.lock();
        std::cout << "Message: " << ex.what() << ".\n";
        global_stream_lock.unlock();
    }
}

void ThrowAnException(boost::shared_ptr<boost::asio::io_service> iosvc, int counter)
{
    global_stream_lock.lock();
    std::cout << "Throw Exception " << counter << "\n";
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    global_stream_lock.unlock();
    throw(std::runtime_error("The Exception !!!, counter: " + std::to_string(counter)));
}

int main(void)
{
    boost::shared_ptr<boost::asio::io_service> io_svc(new boost::asio::io_service);
    boost::shared_ptr<boost::asio::io_service::work> worker(new boost::asio::io_service::work(*io_svc));

    global_stream_lock.lock();
    std::cout << "The program will exit once all work has finished.\n ";
    global_stream_lock.unlock();
    boost::thread_group threads;
    for (int i = 1; i <= 2; i++)
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));

    io_svc->post(boost::bind(&ThrowAnException, io_svc, 1));
    io_svc->post(boost::bind(&ThrowAnException, io_svc, 2));
    io_svc->post(boost::bind(&ThrowAnException, io_svc, 4));
    io_svc->post(boost::bind(&ThrowAnException, io_svc, 3));
    io_svc->post(boost::bind(&ThrowAnException, io_svc, 5));
    threads.join_all();

    return 0;
}
```

## Handling an error

Trong ví dụ trước của chúng tôi, chúng tôi đã sử dụng hàm `run()` mà không có bất kỳ tham số nào, nhưng trên thực tế, hàm có hai phương thức overloading, `std::size_t run()` và `std::size_t run(boost::system::error_code &ec)`. Phương thức sau có một tham số **error code** sẽ được đặt nếu xảy ra lỗi.

```cpp
/* errorcode.cpp */
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
    boost::system::error_code ec;
    iosvc->run(ec);
    if (ec)
    {
        global_stream_lock.lock();
        std::cout << "Message: " << ec << ".\n";
        global_stream_lock.unlock();
    }
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " End.\n";
    global_stream_lock.unlock();
}

void ThrowAnException(boost::shared_ptr<boost::asio::io_service> iosvc)
{
    global_stream_lock.lock();
    std::cout << "Throw Exception\n";
    global_stream_lock.unlock();
    iosvc->post(boost::bind(&ThrowAnException, iosvc));
    throw(std::runtime_error("The Exception !!!"));
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

    io_svc->post(boost::bind(&ThrowAnException, io_svc));
    threads.join_all();
    return 0;
}
```

# Timing the work execution using the timer class

- Có một lớp trong thư viện Boost C ++ cung cấp khả năng tiến hành **blocking** hoặc  **asynchronous wait for a timer** cho đến khi nó hết hạn, được gọi là **deadline timer**. **Deadline timer** cho biết một trong hai trạng thái: **expired** hoặc **not expired**.

## An expiring timer

```cpp
/* timer.cpp */
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
    while (true)
    {
        try
        {
            boost::system::error_code ec;
            iosvc->run(ec);
            if (ec)
            {
                global_stream_lock.lock();
                std::cout << "Message: " << ec << ".\n";
                global_stream_lock.unlock();
            }
            break;
        }
        catch (std::exception &ex)
        {
            global_stream_lock.lock();
            global_stream_lock.unlock();
        }
    }
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " End.\n";
    global_stream_lock.unlock();
}

void TimerHandler(const boost::system::error_code &ec)
{
    if (ec)
    {
        global_stream_lock.lock();
        std::cout << "Error Message: " << ec << ".\n";
        global_stream_lock.unlock();
    }
    else
    {
        global_stream_lock.lock();
        std::cout << "You see this line because you have waited for 10 seconds.\n ";
        std::cout << "Now press ENTER to exit.\n";
        global_stream_lock.unlock();
    }
}

int main(void)
{
    boost::shared_ptr<boost::asio::io_service> io_svc(new boost::asio::io_service);
    boost::shared_ptr<boost::asio::io_service::work> worker(new boost::asio::io_service::work(*io_svc));

    global_stream_lock.lock();
    std::cout << "Wait for ten seconds to see what happen, ";
    std::cout << "otherwise press ENTER to exit!\n";
    global_stream_lock.unlock();

    boost::thread_group threads;
    for (int i = 1; i <= 5; i++)
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));

    boost::asio::deadline_timer timer(*io_svc);
    timer.expires_from_now(boost::posix_time::seconds(10));
    timer.async_wait(TimerHandler);
    std::cin.get();
    io_svc->stop();
    threads.join_all();
    return 0;
}
```

- Sau khi bộ đếm thời gian hết hạn, hàm `TimerHandler` sẽ được gọi và vì không có lỗi nên chương trình sẽ thực thi code bên trong khối `else`.
- Và vì chúng tôi đã sử dụng hàm `async_wait()`, chúng tôi có thể nhấn phím **Enter** để thoát khỏi chương trình trước khi chúng tôi nhìn thấy dòng: **Now press ENTER to exit**.

## Using the timer along with the `boost::bind` function

- Hãy để chúng tôi cố gắng tạo bộ hẹn giờ định kỳ. Chúng ta phải khởi tạo đối tượng hẹn giờ toàn cục để đối tượng trở thành đối tượng được chia sẻ. Để đạt được điều này, chúng ta cần sự trợ giúp từ con trỏ shared_ptr và phương thức `boost::bind` để tạo và giữ cho luồng an toàn vì chúng ta sẽ sử dụng một đối tượng được chia sẻ:

```cpp
/* timer2.cpp */
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

    while (true)
    {
        try
        {
            boost::system::error_code ec;
            iosvc->run(ec);
            if (ec)
            {
                global_stream_lock.lock();
                std::cout << "Message: " << ec << ".\n";
                global_stream_lock.unlock();
            }
            break;
        }
        catch (std::exception &ex)
        {
            global_stream_lock.lock();
            std::cout << "Message: " << ex.what() << ".\n";
            global_stream_lock.unlock();
        }
    }
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " End.\n";
    global_stream_lock.unlock();
}

void TimerHandler(const boost::system::error_code &ec, boost::shared_ptr<boost::asio::deadline_timer> tmr)
{
    if (ec)
    {
        global_stream_lock.lock();
        std::cout << "Error Message: " << ec << ".\n";
        global_stream_lock.unlock();
    }
    else
    {
        global_stream_lock.lock();
        std::cout << "You see this every three seconds.\n";
        global_stream_lock.unlock();
        tmr->expires_from_now(boost::posix_time::seconds(3));
        tmr->async_wait(boost::bind(&TimerHandler, _1, tmr));
    }
}

int main(void)
{
    boost::shared_ptr<boost::asio::io_service> io_svc(new boost::asio::io_service);
    boost::shared_ptr<boost::asio::io_service::work> worker(new boost::asio::io_service::work(*io_svc));

    global_stream_lock.lock();
    std::cout << "Press ENTER to exit!\n";
    global_stream_lock.unlock();

    boost::thread_group threads;
    for (int i = 1; i <= 5; i++)
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));

    boost::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(*io_svc));
    timer->expires_from_now(boost::posix_time::seconds(3));
    timer->async_wait(boost::bind(&TimerHandler, _1, timer));
    std::cin.get();
    io_svc->stop();
    threads.join_all();
    return 0;
}
```

- Từ đầu ra, chúng tôi thấy rằng bộ đếm thời gian được tick cứ ba giây một lần và công việc sẽ bị dừng sau khi người dùng nhấn phím **Enter**. Bây giờ, chúng ta hãy xem đoạn code sau:

```cpp
timer->async_wait(boost::bind(&TimerHandler, _1, timer));
```

- Hàm `boost::bind` giúp chúng ta sử dụng đối tượng hẹn giờ chung. Và nếu chúng ta nhìn sâu hơn, chúng ta có thể sử dụng tham số `_1` cho hàm `boost::bind` của chúng ta. Nếu chúng ta đọc [tài liệu](https://www.boost.org/doc/libs/1_58_0/libs/bind/doc/html/bind.html) về hàm `boost::bind`, chúng ta sẽ thấy rằng tham số _1 là đối số giữ chỗ sẽ được thay thế bằng đối số đầu vào đầu tiên.

## Using the timer along with the boost::strand function

- Vì bộ định thời được thực thi không đồng bộ, có thể việc thực thi bộ định thời không phải trong một quá trình tuần tự hóa. Bộ đếm thời gian có thể được thực thi trong một luồng trong khi một sự kiện khác được thực thi cùng một lúc. Như chúng ta đã thảo luận trước đây, chúng ta có thể sử dụng hàm `strand` để sắp xếp thứ tự thực hiện. Hãy để chúng tôi xem qua đoạn mã sau:

```cpp
/* timer3.cpp */
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
    while (true)
    {
        try
        {
            boost::system::error_code ec;
            iosvc->run(ec);
            if (ec)
            {
                global_stream_lock.lock();
                std::cout << "Message: " << ec << ".\n";
                global_stream_lock.unlock();
            }
            break;
        }
        catch (std::exception &ex)
        {
            global_stream_lock.lock();
            std::cout << "Message: " << ex.what() << ".\n";
            global_stream_lock.unlock();
        }
    }
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " End.\n";
    global_stream_lock.unlock();
}

void TimerHandler(const boost::system::error_code &ec, boost::shared_ptr<boost::asio::deadline_timer> tmr, boost::shared_ptr<boost::asio::io_service::strand> strand)
{
    if (ec)
    {
        global_stream_lock.lock();
        std::cout << "Error Message: " << ec << ".\n";
        global_stream_lock.unlock();
    }
    else
    {
        global_stream_lock.lock();
        std::cout << "You see this every second.\n";
        global_stream_lock.unlock();
        tmr->expires_from_now(boost::posix_time::seconds(1));
        tmr->async_wait(strand->wrap(boost::bind(&TimerHandler, _1, tmr, strand)));
    }
}

void Print(int number)
{
    std::cout << "Number: " << number << std::endl;
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
}

int main(void)
{
    boost::shared_ptr<boost::asio::io_service> io_svc(new boost::asio::io_service);
    boost::shared_ptr<boost::asio::io_service::work> worker(new boost::asio::io_service::work(*io_svc));
    boost::shared_ptr<boost::asio::io_service::strand> strand(new boost::asio::io_service::strand(*io_svc));
    global_stream_lock.lock();
    std::cout << "Press ENTER to exit!\n";
    global_stream_lock.unlock();

    boost::thread_group threads;
    for (int i = 1; i <= 5; i++)
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));

    boost::this_thread::sleep(boost::posix_time::seconds(1));
    strand->post(boost::bind(&Print, 1));
    strand->post(boost::bind(&Print, 2));
    strand->post(boost::bind(&Print, 3));
    strand->post(boost::bind(&Print, 4));
    strand->post(boost::bind(&Print, 5));

    boost::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(*io_svc));
    timer->expires_from_now(boost::posix_time::seconds(1));
    timer->async_wait(strand->wrap(boost::bind(&TimerHandler, _1, timer, strand)));
    std::cin.get();
    io_svc->stop();
    threads.join_all();
    return 0;
}
```

# Summary

- We have successfully serialized the io_service object's work by using the strand object, so we can ensure the order of work we have designed. We can also ensure our program will run smoothly without any crashes by using error and exception handling. Lastly, in this chapter, we discussed the waiting time, since this is important when creating a network application.
- Now, let us move on to the next chapter to talk about creating a server-client application that will make communication possible between two parties, the server and the client.
