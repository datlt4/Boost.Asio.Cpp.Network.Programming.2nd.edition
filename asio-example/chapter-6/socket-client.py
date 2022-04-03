import socket

HOST = '127.0.0.1'
PORT = 8000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (HOST, PORT)
print('connecting to %s port ' + str(server_address))
s.connect(server_address)

try:
    while True:
        data = s.recv(1024)
        print('Server: ', data.decode("utf8"))

        msg = input('Client: ')
        if msg == "quit":
            break
        else:
            s.sendall(bytes(msg, "utf8"))

finally:
    s.close()
