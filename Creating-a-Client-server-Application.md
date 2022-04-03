- Trong chương trước, chúng ta đã đi sâu vào các thư viện **Boost.Asio**, các thư viện này rất quan trọng để phát triển một ứng dụng mạng. Và bây giờ, chúng ta sẽ chuyển sang thảo luận sâu hơn về một ứng dụng client-server có thể giao tiếp với nhau qua mạng máy tính giữa hai hoặc nhiều máy tính. Một trong số chúng được gọi là **client** và cái còn lại là **server**.
- Chúng ta sẽ thảo luận về sự phát triển của **server**, **server** có thể gửi và nhận lưu lượng dữ liệu từ **client** và cũng tạo ra một chương trình phía **client** để nhận lưu lượng dữ liệu. Trong chương này, chúng ta sẽ thảo luận về các chủ đề sau:

    - Thiết lập kết nối giữa **client** và **server**
    - Gửi và nhận dữ liệu giữa **client** và **server**
    - Gói code được sử dụng thường xuyên nhất để đơn giản hóa quá trình lập trình bằng cách tránh sử dụng lại code.

# Establishing a connection

Chúng ta đã nói về hai loại Giao thức **Internet Protocol** (IP) trong Chương 2, Tìm hiểu các khái niệm về mạng. Đó là **Transmission Control Protocol (TCP)** và **User Datagram Protocol (UDP)**. **TCP** là hướng kết nối *(connection-oriented)*, có nghĩa là dữ liệu có thể được gửi ngay sau khi kết nối được thiết lập. Ngược lại, UDP là giao thức Internet không kết nối *(connectionless)*, có nghĩa là giao thức chỉ gửi dữ liệu trực tiếp đến thiết bị đích. Trong chương này, chúng ta sẽ chỉ nói về *TCP*; do đó, chúng ta phải thiết lập kết nối trước. Kết nối chỉ có thể được thiết lập nếu hai bên (trong trường hợp này là *client* và *server*) chấp nhận kết nối. Ở đây, chúng ta sẽ cố gắng thiết lập kết nối đồng bộ *(synchronously)* và không đồng bộ *(asynchronously)*.

## A synchronous client

- Chúng ta bắt đầu với việc thiết lập kết nối đồng bộ với một *server* từ xa. Nó hoạt động như một *client*, sẽ mở một kết nối đến trang web *Packt Publishing (www.packtpub.com)*. Chúng ta sẽ sử dụng giao thức TCP, như chúng ta đã thảo luận trước đó trong Chương 2. Đây là code:

```cpp
/* connectsync.cpp */
#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

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

int main(void)
{
    boost::shared_ptr<boost::asio::io_service> io_svc(new boost::asio::io_service);
    boost::shared_ptr<boost::asio::io_service::work> worker(new boost::asio::io_service::work(*io_svc));
    boost::shared_ptr<boost::asio::io_service::strand> strand(new boost::asio::io_service::strand(*io_svc));

    global_stream_lock.lock();
    std::cout << "Press ENTER to exit!\n";
    global_stream_lock.unlock();

    boost::thread_group threads;
    for (int i = 1; i <= 2; i++)
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));

    boost::asio::ip::tcp::socket sckt(*io_svc);
    try
    {
        boost::asio::ip::tcp::resolver resolver(*io_svc);
        boost::asio::ip::tcp::resolver::query query("www.packtpub.com", boost::lexical_cast<std::string>(80));
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
        boost::asio::ip::tcp::endpoint endpoint = *iterator;
        global_stream_lock.lock();
        std::cout << "Connecting to: " << endpoint << std::endl;
        global_stream_lock.unlock();
        sckt.connect(endpoint);
        std::cout << "Connected!\n";
    }
    catch (std::exception &ex)
    {
        global_stream_lock.lock();
        std::cout << "Message: " << ex.what() << ".\n";
        global_stream_lock.unlock();
    }

    std::cin.get();
    boost::system::error_code ec;
    sckt.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    std::cout << "[ sckt.shutdown ][ ec ]: " << ec << ".\n";
    sckt.close(ec);
    std::cout << "[ sckt.close ][ ec ]: " << ec << ".\n";
    io_svc->stop();
    threads.join_all();
    return 0;
}
```

- Bây giờ, chúng ta hãy phân tích code. Như chúng ta có thể thấy trong code trước, chúng ta sử dụng code mẫu trước đó của mình và chèn một dòng code để giúp nó có thể thiết lập kết nối. Hãy thu hút sự chú ý của chúng ta đến dòng chúng ta đã chèn:

```cpp
boost::asio::ip::tcp::socket sckt(*io_svc);
```

- Bây giờ chúng ta có một biến toàn cục, đó là socket. Biến này sẽ được sử dụng để cung cấp chức năng ổ cắm. Nó đến từ namespace `boost::asio::ip::tcp` vì chúng tôi sử dụng TCP làm giao thức của mình:

```cpp
boost::asio::ip::tcp::resolver resolver(*io_svc);
boost::asio::ip::tcp::resolver::query query("www.packtpub.com", boost::lexical_cast<std::string>(80));
boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
```

- Chúng tôi cũng sử dụng namespace `boost::asio::ip::tcp::resolver`. Nó được sử dụng để lấy địa chỉ của **server** lưu trữ từ xa mà chúng tôi muốn kết nối. Với lớp `query()`, chúng ta chuyển địa chỉ Internet và **port** làm tham số. Nhưng bởi vì chúng tôi sử dụng kiểu số nguyên cho một số cổng, chúng tôi phải chuyển đổi nó thành một chuỗi bằng cách sử dụng lexical_cast. Lớp **query** được sử dụng để mô tả truy vấn có thể được chuyển tới **resolver**. Sau đó, bằng cách sử dụng lớp **iterator**, chúng tôi sẽ xác định **iterators** từ kết quả được trả về bởi **resolver**:

```cpp
boost::asio::ip::tcp::endpoint endpoint = *iterator;
```

- Sau khi **iterator** được tạo thành công, chúng tôi cung cấp nó cho biến kiểu `endpoint`. **Endpoint** sẽ lưu trữ danh sách các địa chỉ ip được tạo bởi **resolver**:

```cpp
sckt.connect(endpoint);
```

- Sau đó, hàm thành viên `connect()` sẽ kết nối **socket** với **endpoint** mà chúng ta đã chỉ định trước đó. Nếu mọi thứ chạy bình thường và không có lỗi hoặc ngoại lệ nào được đưa ra, kết nối hiện đã được thiết lập:

```cpp
boost::system::error_code ec;
sckt.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
sckt.close(ec);
```

- Để giải phóng kết nối, trước tiên chúng ta phải tắt quá trình gửi và nhận dữ liệu trên socket bằng cách sử dụng hàm `shutdown()`; sau đó, chúng tôi gọi hàm `close()` để đóng **socket**.

## An asynchronous client

- Chúng tôi đã có thể thiết lập kết nối một cách **đồng bộ**. Nhưng nếu chúng ta cần kết nối **không đồng bộ** với đích để chương trình không bị đóng băng trong khi cố gắng tạo kết nối thì sao? Hãy cùng chúng tôi xem qua đoạn code sau để tìm câu trả lời:

```cpp
/* connectasync.cpp */
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>

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
                std::cout << "[ WorkerThread ] Message: " << ec << ".\n";
                global_stream_lock.unlock();
            }
            break;
        }
        catch (std::exception &ex)
        {
            global_stream_lock.lock();
            std::cout << "[ WorkerThread ] Message: " << ex.what() << ".\n";
            global_stream_lock.unlock();
        }
    }
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " End.\n";
    global_stream_lock.unlock();
}

void OnConnect(const boost::system::error_code &ec)
{
    if (ec)
    {
        global_stream_lock.lock();
        std::cout << "OnConnect Error: " << ec << ".\n";
        global_stream_lock.unlock();
    }
    else
    {
        global_stream_lock.lock();
        std::cout << "Connected!.\n";
        global_stream_lock.unlock();
    }
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
    for (int i = 1; i <= 2; i++)
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));

    boost::shared_ptr<boost::asio::ip::tcp::socket> sckt(new boost::asio::ip::tcp::socket(*io_svc));
    try
    {
        boost::asio::ip::tcp::resolver resolver(*io_svc);
        boost::asio::ip::tcp::resolver::query query("www.packtpub.vn", boost::lexical_cast<std::string>(80));
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
        boost::asio::ip::tcp::endpoint endpoint = *iterator;
        global_stream_lock.lock();
        std::cout << "Connecting to: " << endpoint << std::endl;
        global_stream_lock.unlock();
        sckt->async_connect(endpoint, boost::bind(OnConnect, _1));
        std::cout << "[ LOG ] sckt->async_connect" << std::endl;
    }
    catch (std::exception &ex)
    {
        global_stream_lock.lock();
        std::cout << "[ Main ] Message: " << ex.what() << ".\n";
        global_stream_lock.unlock();
    }

    std::cin.get();
    boost::system::error_code ec;
    sckt->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    sckt->close(ec);
    io_svc->stop();
    threads.join_all();
    return 0;
}
```

- Như chúng ta có thể thấy trong đoạn mã trước, chúng ta thêm hàm `OnConnect()`. Vì đối tượng socket là không thể sao chép và chúng ta cần đảm bảo rằng nó vẫn hợp lệ trong khi trình xử lý đang chờ được gọi, chúng ta phải sử dụng namespace `boost::shared_ptr`. Chúng tôi cũng sử dụng namespace `boost::bind` để gọi trình xử lý, tức là, hàm `OnConnect()`.

## An asynchronous server

- Chúng ta đã biết cách kết nối với **server** từ xa một cách đồng bộ và không đồng bộ. Bây giờ, chúng ta sẽ tạo chương trình **server** để nói chuyện với chương trình **client-side** mà chúng ta đã tạo trước đó. Bởi vì chúng ta sẽ xử lý chương trình không đồng bộ trong namespace `boost::asio`, chúng ta sẽ chỉ thảo luận về chương trình **client-side** trong một **server** không đồng bộ. Hãy để chúng tôi xem qua đoạn mã sau:

```cpp
/* serverasync.cpp */
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>

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

void OnAccept(const boost::system::error_code &ec)
{
    if (ec)
    {
        global_stream_lock.lock();
        std::cout << "OnAccept Error: " << ec << ".\n";
        global_stream_lock.unlock();
    }
    else
    {
        global_stream_lock.lock();
        std::cout << "Accepted!\n";
        global_stream_lock.unlock();
    }
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
    for (int i = 1; i <= 2; i++)
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));

    boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor(new boost::asio::ip::tcp::acceptor(*io_svc));
    boost::shared_ptr<boost::asio::ip::tcp::socket> sckt(new boost::asio::ip::tcp::socket(*io_svc));
    try
    {
        boost::asio::ip::tcp::resolver resolver(*io_svc);
        boost::asio::ip::tcp::resolver::query query("127.0.0.1", boost::lexical_cast<std::string>(8000));
        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
        acceptor->open(endpoint.protocol());
        acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
        acceptor->bind(endpoint);
        acceptor->listen(boost::asio::socket_base::max_connections);
        acceptor->async_accept(*sckt, boost::bind(OnAccept, _1));
        global_stream_lock.lock();
        std::cout << "Listening on: " << endpoint << std::endl;
        std::cout << "Max connections: " << boost::asio::socket_base::max_connections << std::endl;
        global_stream_lock.unlock();
    }
    catch (std::exception &ex)
    {
        global_stream_lock.lock();
        std::cout << "Message: " << ex.what() << ".\n";
        global_stream_lock.unlock();
    }

    std::cin.get();
    boost::system::error_code ec;
    acceptor->close(ec);
    sckt->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    sckt->close(ec);
    io_svc->stop();
    threads.join_all();
    return 0;
}
```

- Trước khi chúng tôi chạy chương trình, hãy để chúng tôi phân biệt mã. Bây giờ chúng ta có một đối tượng mới, đó là `tcp::acceptor`. Đối tượng này được sử dụng để chấp nhận các kết nối **socket** mới. Do việc sử dụng hàm `accept()`, chúng tôi cần thêm thư viện `mswsock` vào quy trình biên dịch của mình:


```cpp
acptor->open(endpoint.protocol());
acptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
acptor->bind(endpoint);
acptor->listen(boost::asio::socket_base::max_connections);
acptor->async_accept(*sckt, boost::bind(OnAccept, _1));
```

Từ đoạn mã trước, chúng ta có thể thấy rằng chương trình gọi hàm `open()` để mở **acceptor** bằng cách sử dụng giao thức được truy xuất từ biến `endpoint`. Sau đó, bằng cách sử dụng hàm `set_option`, chúng tôi đặt một tùy chọn trên **acceptor** để không sử dụng lại địa chỉ. **acceptor** cũng được liên kết với endpoint bằng cách sử dụng hàm `bind()`. Sau đó, chúng tôi gọi hàm `listen()` để đặt **acceptor** vào trạng thái mà nó sẽ lắng nghe các kết nối mới. Cuối cùng, **acceptor** sẽ chấp nhận các kết nối mới bằng cách sử dụng hàm `async_accept()`, hàm này sẽ bắt đầu chấp nhận không đồng bộ. Tuy nhiên, vì chúng ta chỉ có một đối tượng **socket** và gọi hàm `async_accept()` chỉ một lần, chương trình sẽ chỉ chấp nhận một kết nối.

# Reading and writing to the socket

- Chúng tôi chính thức có thể tạo kết nối **client-server**. Bây giờ, chúng ta sẽ viết và đọc **socket** để làm cho kết nối trở nên hữu ích hơn. Chúng tôi sẽ sửa đổi mã trước đó của mình, `serverasync.cpp` và thêm đối tượng `basic_stream_socket`, đối tượng này cung cấp chức năng **stream-oriented socket**.

[**_Code_**](chapter-6/readwritesocket.cpp)

- Nếu chúng ta so sánh mã của file `readwritesocket.cpp` với `serverasync.cpp`, chúng tôi sẽ thấy rằng chúng tôi thêm một lớp mới có tên là `ClientContext`. Nó chứa năm hàm thành viên: `Send()`, `OnSend()`, `Recv()`, `OnRecv()` và `Close()`.

## The Send() and OnSend() functions

- Trong hàm `Send()`, chúng tôi truyền một mảng các ký tự và độ dài của chúng. Trước khi hàm gửi mảng ký tự, nó phải kiểm tra xem tham số `m_send_buffer` có trống hay không. Quá trình gửi chỉ có thể xảy ra nếu bộ đệm không trống.
- Namespace `boost::asio::async_write` ghi socket và gọi trình xử lý hàm `OnSend()`. Sau đó, nó xóa bộ đệm và gửi dữ liệu đang chờ xử lý tiếp theo nếu có.

## The Recv() and OnRecv() functions

- Ngược lại với hàm `Send()`, hàm `Recv()` sẽ gọi hàm `async_read_some()` để nhận tập dữ liệu, và hàm xử lý hàm `OnRecv()` sẽ định dạng dữ liệu nhận được sang *định dạng hexa*.

# Developing a client and server program

## Creating a simple echo server

- Chúng tôi sẽ tạo một chương trình **server** sẽ phản hồi tất cả lưu lượng mà nó truy xuất từ **client**. Trong trường hợp này, chúng tôi sẽ sử dụng `telnet` làm **client**, như chúng tôi đã làm trước đây. Tệp phải được lưu dưới dạng echoserver.cpp và nội dung sẽ giống như sau:

[**_Code_**](chapter-6/echoserver.cpp)

The first time we run the program, it will listen to port 8000 in localhost. We can see in the main block that the program calls the `poll()` function in the Hive class if there is no keyboard hit. This means that the program will close if any key is pressed because it will invoke the `Stop()` function in the Hive class, which will stop the io_service object. Every 1000 milliseconds, the timer will tick because the constructor of the Acceptor class initiates the interval of the timer for 1000 milliseconds.

Now, open another console window and type the command `telnet 127.0.0.1 8000` to make telnet our client. After the echoserver accepts the connection, every time we press the alphanumeric option on the keyboard, the echoserver will send the character back to telnet.

When the server accepts the connection from the client, the `OnAccept()` function handler will be invoked immediately. I pressed the A, B, and C keys respectively in the telnet window, and then echoserver received the characters and sent them back to the client. The telnet window also displays A, B, and C.

## Creating a simple client program

- Now, we will move on to develop the client-side program. It will receive the content of the Packt Publishing website through the `HTTP GET` command, and the code will be like the following:

- Just after the connection is established, the program sends an HTTP GET command to port `80` of `www.packtpub.com` using the following code snippet:

```cpp
std::string str = "GET / HTTP/1.0\r\n\r\n";
std::vector<uint8_t> request;
std::copy(str.begin(), str.end(), std::back_inserter(request));
Send(request)
```

It then sends the request to the socket using the `Send()` function in the `Connection` class inside the code of the `wrapper.cpp` file. The code snippet of the `Send()` function is as follows:

```cpp
m_io_strand.post(boost::bind(&Connection::DispatchSend, 
shared_from_this(), buffer));
```

- As we can see, we use the strand object in order to allow all events to be serially run. In addition, because of the strand object, we do not have to use the lock object every time the event occurs.

- After the request is sent, the program will pool the incoming data using the following code snippet:

```cpp
m_io_service.poll();
```

- Then, once the data is coming, it will be displayed in the console by the `OnRecv()` function handler, as we can see in the preceding image.

# Summary

- There are three basic steps when it comes to developing a network application. The first step includes establishing a connection between the source and target, which means the **client** and **server**. We can configure the `socket` object along with the `acceptor` object to establish the connection.
- Secondly, we exchange data by reading and writing to the socket. For this purpose, we can use the `basic_stream_socket` functions collection. In our previous example, we used the `boost::asio::async_write()` method to send the data and the `boost::asio::async_read()` method to receive the data. Finally, the last step is releasing the connection. By using the `shutdown()` method in the `ip::tcp::socketobject`, we can disable the sending and receiving of data on the socket. Then, invoking the `close()` method after the `shutdown()` function will close the socket and free up the handler. We also have already created a wrapper for all functions, which is most frequently used in network application programming by accessing the **Boost.Asio** library. This means we can develop a network application simply and efficiently since we do not need to reuse code over and over again.