import socket
import os
import datetime
import mimetypes

file_name = "logs.txt"
port = 8080
path = "/home/kubuntu/Documents/low_levle_things/http_server/python3/browny-v1.0/"


def logging(user_address, network_activity):
    with open(file_name, "a+") as file:
        file.write(f"{str(datetime.datetime.now())}: {user_address}/{network_activity}\n")


def http_server(server_socket):
    client_socket, client_address = server_socket.accept()
    http_request = client_socket.recv(1024).decode("utf-8")
    request = list((list(http_request.split("\n"))[0]).split(" "))[1][1:]
    logging(client_address, request)

    full_path = os.path.abspath(os.path.join(path, request))
    if not full_path.startswith(path) or not os.path.exists(full_path):
        with open(os.path.join(path, "404.html"), "rb") as file:
            client_socket.sendall(b"HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n")
            client_socket.sendall(file.read())
    else:
        content_type, _ = mimetypes.guess_type(full_path)
        content_type = content_type or "application/octet-stream"
        with open(full_path, "rb") as file:
            client_socket.sendall(f"HTTP/1.1 200 OK\r\nContent-Type: {content_type}\r\n\r\n".encode())
            client_socket.sendall(file.read())
    client_socket.close()


server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(("127.0.0.1", port))
server_socket.listen(20)
print("The server is running on port", port)

while True:
    http_server(server_socket)
ind
