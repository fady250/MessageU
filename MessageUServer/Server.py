import socket


class Server(object):
    def __init__(self):
        try:
            with open('myport.info', 'r') as port:
                self.__PORT = int(port.readline())
                self.__HOST = ''
                self.__clients = []             # holds objects of type ClientEntry
                # disctionary that holds client as key and list of messages as value
        except FileNotFoundError:
            print("File myport.info not found !")
            exit()

    def start(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind((self.__HOST, self.__PORT))
            s.listen()
            conn, addr = s.accept()
            with conn:
                print('Connected by', addr)
                # check in the examples madrich if we have examopkle how to handle 
                while True:
                    data = conn.recv(1024)
                    text = data.decode("utf-8")
                    print("Received message: " + text)
                    print("Enter message ")
                    reply = input()
                    replydata = bytearray(reply, "utf-8")
                    newdata = bytearray(1024)
                    for i in range(min(len(replydata), len(newdata))):
                        newdata[i] = replydata[i]
                    conn.sendall(newdata)
