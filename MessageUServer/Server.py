import ctypes
import selectors
import socket
import struct
import uuid
import ClientEntry
import MessageEntry
import sys
from enum import Enum

VERSION = 1
ZERO = 0
MESSAGE_TYPE_LEN = 1  # 1 byte for the message type field in 1103 request
CONTENT_SIZE_LEN = MESSAGE_ID_LEN = 4  # 4 bytes for the content size field in 1103 request
CLIENT_ID_LEN = 16
REQUEST_HEADER_SIZE = 23
NAME_MAX_SIZE = 255
PUB_KEY_SIZE = 160


class RequestCode(Enum):
    userRegister = 1100
    clientsList = 1101
    pullClientPubKey = 1102
    sendMsg = 1103
    pullMsgs = 1104


class ResponseCode(Enum):
    registerSuccess = 2100
    clientList = 2101
    pubKey = 2102
    msgSent = 2103
    msgPull = 2104
    error = 9000


class MessageType(Enum):
    symKeyReq = 1
    symKeySend = 2
    textMsgSend = 3
    fileSend = 4


class Server(object):
    def __init__(self):
        try:
            with open('myport.info', 'r') as port:
                self.__sel = selectors.DefaultSelector()
                self.__PORT = int(port.readline())
                self.__index = 0
                self.__clients = []  # Hold objects of type ClientEntry
                self.__messages = []  # Hold objects of type MessageEntry
        except FileNotFoundError:
            print("File myport.info not found !")
            exit()

    def start(self):

        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind(('', self.__PORT))
            s.listen(100)
            #s.setblocking(False)
            self.__sel.register(s, selectors.EVENT_READ, self.accept)
            while True:
                events = self.__sel.select()
                for key, mask in events:
                    callback = key.data
                    callback(key.fileobj, mask)

    def accept(self, sock, mask):
        conn, addr = sock.accept()
        print('accepted', conn, 'from', addr)
        #conn.setblocking(False)
        self.__sel.register(conn, selectors.EVENT_READ, self.handle_client_request)

    def handle_client_request(self, conn, mask):
        data = conn.recv(REQUEST_HEADER_SIZE)  # receive the header
        if data:
            #cid1, cid2, ver, code, pay_size = struct.unpack('<2dBHI', data)
            cid = struct.unpack('<' + 16 * 'B', data[:16])
            ver, code, pay_size = struct.unpack_from('<BHI', data, 16)
            #from_cid = cid1 + cid2  # TODO verify if not the opposite way ?
            if code == RequestCode.userRegister.value:
                # receive the rest of the packet
                data = conn.recv(NAME_MAX_SIZE)
                name = struct.unpack('<' + NAME_MAX_SIZE * 'B', data)
                data = conn.recv(PUB_KEY_SIZE)
                pub_key = struct.unpack('<' + PUB_KEY_SIZE * 'B', data)
                # check if the user name exists in the clients
                for entry in self.__clients:
                    if entry.UserName == name:
                        # error - username already exists
                        frmt = '<BHI'
                        buf_size_in_bytes = struct.calcsize(frmt)  # TODO need to send big endian ?
                        buf = ctypes.create_string_buffer(buf_size_in_bytes)
                        values = (VERSION, ResponseCode.error.value, ZERO)
                        struct.pack_into(frmt, buf, 0, *values)
                        conn.sendall(buf)
                        return
                # add new entry
                uid = uuid.uuid4()
                print(uid)      #TODO clean
                print(uid.hex)
                print(uid.bytes)
                uid_bytes = bytearray(uid.bytes)
                print(uid_bytes)
                print(tuple(uid_bytes))
                self.__clients.append(ClientEntry.ClientEntry(tuple(uid_bytes), name, pub_key))
                frmt = '<BHI' + CLIENT_ID_LEN * 'B'
                buf_size_in_bytes = struct.calcsize(frmt)  # TODO need to send big endian ?
                buf = ctypes.create_string_buffer(buf_size_in_bytes)
                #client_id = str.encode(uid.bytes)  # move to byte representation
                #values = (VERSION, ResponseCode.registerSuccess.value, CLIENT_ID_LEN, *client_id)
                values = (VERSION, ResponseCode.registerSuccess.value, CLIENT_ID_LEN, *(tuple(uid_bytes)))
                struct.pack_into(frmt, buf, 0, *values)
                conn.sendall(buf)

            elif code == RequestCode.clientsList.value:
                frmt = '<BHI' + self.__clients.__len__() * (NAME_MAX_SIZE + CLIENT_ID_LEN) * 'B'
                buf_size_in_bytes = struct.calcsize(frmt)  # TODO need to send big endian ?
                buf = ctypes.create_string_buffer(buf_size_in_bytes)
                values = (
                    VERSION, ResponseCode.clientList.value, self.__clients.__len__() * (NAME_MAX_SIZE + CLIENT_ID_LEN))
                for entry in self.__clients:
                    print (values)
                    print (entry.ID)
                    print (entry.UserName)
                    values += entry.ID + entry.UserName
                struct.pack_into(frmt, buf, 0, *values)
                conn.sendall(buf)

            elif code == RequestCode.pullClientPubKey.value:
                # receive the wanted client id
                data = conn.recv(CLIENT_ID_LEN)
                requested_cid = struct.unpack('<' + CLIENT_ID_LEN * 'B', data)
                print("requested cid --========================---- ")
                print(requested_cid)  # TODO clean
                # check if the user name exists in the clients
                public_key = ()
                for entry in self.__clients:
                    if entry.ID == requested_cid:
                        public_key = entry.PubKey
                if public_key == ():
                    # TODO error handle
                    pass
                print("public key to be returned - " + str(public_key))  # TODO clean
                frmt = '<BHI' + (CLIENT_ID_LEN + PUB_KEY_SIZE) * 'B'
                buf_size_in_bytes = struct.calcsize(frmt)  # TODO need to send big endian ?
                buf = ctypes.create_string_buffer(buf_size_in_bytes)
                values = (VERSION, ResponseCode.pubKey.value, CLIENT_ID_LEN + PUB_KEY_SIZE, *(requested_cid + public_key))
                struct.pack_into(frmt, buf, 0, *values)
                conn.sendall(buf)

            elif code == RequestCode.sendMsg:
                # receive the rest of the packet
                data = conn.recv(CLIENT_ID_LEN + MESSAGE_TYPE_LEN + CONTENT_SIZE_LEN)
                cid1, cid2, msg_type, content_size = struct.unpack('<2dBI', data)
                to_cid = cid1 + cid2
                data = conn.recv(content_size)
                # data.decode() # TODO shouold this be saved decoded ?
                self.__messages.append(MessageEntry.MessageEntry(self.__index, to_cid, from_cid, msg_type, data))

                frmt = '<BHI' + (CLIENT_ID_LEN + MESSAGE_ID_LEN) * 'B'
                buf_size_in_bytes = struct.calcsize(frmt)  # TODO need to send big endian ?
                buf = ctypes.create_string_buffer(buf_size_in_bytes)
                values = (VERSION, ResponseCode.msgSent.value, CLIENT_ID_LEN + MESSAGE_ID_LEN,
                          *(str.encode(to_cid) + str.encode(str(self.__index))))
                self.__index += 1
                struct.pack_into(frmt, buf, 0, *values)
                conn.sendall(buf)

            elif code == RequestCode.pullMsgs:
                frmt = '<BHI'
                values = [VERSION, ResponseCode.clientList.value, ZERO]
                msg_list = []
                for msg in self.__messages:
                    if msg.ToClient == from_cid:
                        msg_list.append(msg)
                        this_msg_size_in_bytes = (
                                CLIENT_ID_LEN + MESSAGE_ID_LEN + MESSAGE_TYPE_LEN + CONTENT_SIZE_LEN + sys.getsizeof(
                            msg.Content))
                        values[2] += this_msg_size_in_bytes  # update the total size
                        # add the message content
                        values.append(*(str.encode(from_cid) + str.encode(str(msg.ID)) + str.encode(msg.Type) +
                                        str.encode(str(sys.getsizeof(msg.Content))) + str.encode(msg.Content)))

                        frmt += this_msg_size_in_bytes * 'B'
                        self.__messages.remove(msg)

                buf_size_in_bytes = struct.calcsize(frmt)  # TODO need to send big endian ?
                buf = ctypes.create_string_buffer(buf_size_in_bytes)
                struct.pack_into(frmt, buf, 0, *(tuple(values)))
                conn.sendall(buf)
