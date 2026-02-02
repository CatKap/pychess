import socket


cli_properties = ('localhost', 4567)

while True:
    print("Input a msg to send")
    msg = input(">>> ")

    sock = socket.create_connection(cli_properties)
    sock.settimeout(1)
    sock.sendall(msg.encode())
    print("step0") 
    answer = ""
    data = sock.recv(16)
    print("step1")
    while data:
        answer += data.decode()
        data = sock.recv(16)
        print(data.decode())
    print("Answer is")
    sock.close()


    


