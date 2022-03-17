# Networking System
- Network architecture được cấu trúc thành các layers và protocols.
- Từng layer trong cấu trúc mạng có vai trò riêng và mục đính chính là cung cấp 1 service nào đó tới layer cao hơn và giao tiếp với layer liền kề.
- Protocols là một tập các rule và conventions được sử dụng cho giao tiếp giữa các phần tử --> để tiêu chuẩn hóa quá trình giao tiếp.
- Hai kiến trúc mạng phổ biến hiện nay: **OSI (Open System Interconnection)**, **TCP/IP**.

[<img src= "Assets/network-architecture.png" alignment="center">]()

# The OSI reference model

- **OSI model** được sử dụng để kết nối các open system mà không phụ thuộc vào hệ điều hành.
- **OSI model** gồm 7 layer, mỗi layer có chức năng xác địn và định nghĩa các dữ liệu được xử lý.

## The Physical Layer

- Đây là lớp đầu tiên trong **OSI model** và định nghĩa các thông số kỹ thuật vật lý của mạng, bao gồm physical media (cables, connectors) và basic devices (repeaters, hubs).
- Layer này có trách nhiệm luồng dữ liệu truyền các bit thô đầu vào thành các số **0** và các số **1** trên kênh truyền thông. Nó quan tâm tới việc bảo toàn dữ liệu và đảm bảo rằng các bits được gửi từ 1 thiết bị đến thiết bị khác là hoàn toàn giống nhau.

## The Data Link layer

- Nhiệm vụ của lớp **Data Link** cung cấp liên kết cho việc truyền raw data. Trước khi data được truyền đi, nó được chia thành các **data frames**, và **Data Link** truyền chúng liên tiếp. Bên nhận sẽ gửi lại một **acknowledge frame** với từng frame được gửi đối với service tin cậy.
- **Data Link layer** gồm 2 sublayers: **Logical Link Control (LLC)** và **Media Access Control (MAC)**. **LLC sublayer** có nhiệm vụ check lỗi truyền thông tin và quản lý việc truyền dữ liệu, trong khi **MAC sublayer** xác định các để lấy dữ liệu từ **physical media** hoặc lưu data vào **physical media**.
- **MAC adress** hay còn gọi là **physical address** được sử dụng để định danh từng thiết bị kết nối vào mạng và nó là unique.

## The Network layer

- **Network layer** có nhiệm vụ xác định các tốt nhất để định tuyến các packets từ thiết bị nguồn đến thiết bị đích. Nó sẽ tạo ra các **routing tables** sử dụng **Internet Protocol (IP)** là **routing protocol**, và địa chỉ IP được sử dụng để đảm bảo rằng dữ liệu được định tuyến đến đích cần thiết. Hiện nay có hai phiên bản IP: IPv4 và IPv6.

## The Transport layer

- **Transport layer** có nhiệm vụ truyền dữ liệu từ nguồn tới đích. Nó sẽ chia dữ liệu thành các phần nhỏ hơn gọi là **segments**. và sau đó ghép nối tất cả các **segments** để khôi phục lại dữ liệu ban đầu tại đích.
- Có 2 giao thức chính làm việc ở layer này: **Transmission Control Protocol (TCP)** và **User Datagram Protocol(UDP)**.
-**TCP** cung cấp trao đổi data bằng việc thiết lập một **session**. Dữ liệu sẽ không được truyền đi cho đến khi **session** được thiết lập. **TCP** được gọi là **connection-oriented protocol** - nghĩa là session phải được thiết lập trước khi truyền dữ liệu.
- **UDP** là phương pháp truyền dữ liệu không đảm bảo vì nó không thiết lập một session nào. **UDP** được gọi là **connection-less protocol**.
