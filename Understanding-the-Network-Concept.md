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

## The Session layer

- **Session layer** có nhiệm vụ thiết lập, duy trì, kết thúc session. Có thể tưởng tượng một **session** như một kết nối giữa 2 thiết bị trong mạng.
- Ví dụ, nếu chúng ta muốn gửi một tệp từ một máy tính sang một máy tính khác, **layer** này sẽ thiết lập kết nối trước khi tệp có thể được gửi đi. **Layer** này sau đó sẽ đảm bảo rằng kết nối vẫn hoạt động cho đến khi tệp được gửi hoàn toàn. Cuối cùng, **layer** sẽ chấm dứt kết nối nếu nó không còn cần thiết nữa. Kết nối mà chúng ta nói đến là **session**.
- **Session layer** cũng đảm bảo rằng dữ liệu từ một ứng dụng khác không bị thay thế cho nhau. Ví dụ: nếu chúng tôi chạy trình duyệt Internet, ứng dụng trò chuyện và trình quản lý tải xuống cùng một lúc, **session layer** sẽ chịu trách nhiệm thiết lập các **session** cho mọi ứng dụng và đảm bảo rằng chúng vẫn tách biệt với các ứng dụng khác.
- Có ba phương pháp giao tiếp được sử dụng bởi lớp này: **simplex**, **half-duplex** hoặc **full-duplex**. Trong **simplex**, dữ liệu chỉ có thể được chuyển bởi một bên, vì vậy bên kia không thể chuyển bất kỳ dữ liệu nào. Phương pháp này không còn được sử dụng phổ biến nữa, vì chúng ta cần các ứng dụng có thể tương tác với nhau. Trong **half-duplex**, bất kỳ dữ liệu nào cũng có thể được chuyển đến tất cả các thiết bị liên quan, nhưng chỉ một thiết bị có thể chuyển dữ liệu một lúc. **Full-duplex** có thể truyền dữ liệu đến tất cả các thiết bị cùng một lúc. Để gửi và nhận dữ liệu, phương pháp này sử dụng các đường dẫn khác nhau.

## The Presentation layer