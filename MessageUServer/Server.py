import ctypes
import selectors
import socket
import struct
import uuid
import datetime
from enum import Enum
import sqlite3 as sq
from urllib.request import pathname2url

VERSION = 2
ZERO = 0
MESSAGE_TYPE_LEN = 1  # 1 byte for the message type field in 1103 request
CONTENT_SIZE_LEN = MESSAGE_ID_LEN = 4  # 4 bytes for the content size field in 1103 request
CLIENT_ID_LEN = 16
REQUEST_HEADER_SIZE = 23
NAME_MAX_SIZE = 255
PUB_KEY_SIZE = 160
CLIENTS_INSERT_QUERY = "INSERT INTO clients(ID, UserName, PublicKey, LastSeen) values (?, ?, ?, ?)"
MESSAGES_INSERT_QUERY = "INSERT INTO messages(ID, ToClient, FromClient, type, Content) values (?, ?, ?, ?, ?)"
ALL_CLIENTS_QUERY = "SELECT * FROM clients"
MESSAGES_SELECT_QUERY = "SELECT * FROM messages WHERE ToClient = ?"
MESSAGES_DELETE_QUERY = "DELETE FROM messages WHERE ToClient = ?"


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


class Server(object):
    def __init__(self):
        try:
            with open('myport.info', 'r') as port:
                self.__sel = selectors.DefaultSelector()
                self.__PORT = int(port.readline())
                self.__index = 0
                dburi = 'file:{}?mode=rw'.format(pathname2url('server.db'))
                self.__conn = sq.connect(dburi, uri=True)
        except sq.OperationalError:
            # handle missing database case
            self.__conn = sq.connect('server.db')
            query = """
                    CREATE TABLE clients (
                    ID          text PRIMARY KEY CHECK(LENGTH(ID) <= 16),
                    UserName    blob(255) NOT NULL,
                    PublicKey   blob(160) NOT NULL,
                    LastSeen    timestamp);
                    """
            self.__conn.execute(query)
            query = """
                    CREATE TABLE messages (
                    ID          integer PRIMARY KEY,
                    ToClient    blob(16) NOT NULL,
                    FromClient  blob(16) NOT NULL,
                    type        text NOT NULL CHECK(LENGTH(type) <= 1),
                    Content     blob);
                    """
            self.__conn.execute(query)
        except FileNotFoundError:
            print("File myport.info not found !")
            exit()

    def start(self):

        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind(('', self.__PORT))
            s.listen(1000)
            # s.setblocking(False)      #TODO TODO !!!!
            self.__sel.register(s, selectors.EVENT_READ, self.accept)
            while True:
                events = self.__sel.select()
                for key, mask in events:
                    callback = key.data
                    callback(key.fileobj, mask)

    def accept(self, sock, mask):
        conn, addr = sock.accept()
        print('accepted', conn, 'from', addr)
        # conn.setblocking(False)  #TODO TODO !!!!!
        self.__sel.register(conn, selectors.EVENT_READ, self.handle_client_request)

    def handle_client_request(self, conn, mask):
        try:

            data = conn.recv(REQUEST_HEADER_SIZE)  # receive the header
            if data:
                source_cid = struct.unpack('<' + 16 * 'B', data[:16])
                ver, code, pay_size = struct.unpack_from('<BHI', data, 16)

                if code == RequestCode.userRegister.value:
                    # receive the rest of the packet
                    data = conn.recv(NAME_MAX_SIZE)
                    name = struct.unpack('<' + NAME_MAX_SIZE * 'B', data)
                    data = conn.recv(PUB_KEY_SIZE)
                    pub_key = struct.unpack('<' + PUB_KEY_SIZE * 'B', data)
                    # fetch all clients from database and check if the user name already exists
                    result = self.__conn.execute(ALL_CLIENTS_QUERY).fetchall()
                    for entry in result:
                        if self.BYTES_INTARRAY(entry[1]) == name:
                            # error - username already exists
                            self.send_error(conn)
                            return
                    # add new entry
                    uid = uuid.uuid4()
                    # insert the client entry in to the
                    self.__conn.execute(CLIENTS_INSERT_QUERY,
                                        (uid.bytes, self.INTARRAY_BYTES(name), self.INTARRAY_BYTES(pub_key),
                                         datetime.datetime.now()))
                    self.__conn.commit()
                    frmt = '<BHI' + CLIENT_ID_LEN * 'B'
                    buf_size_in_bytes = struct.calcsize(frmt)
                    buf = ctypes.create_string_buffer(buf_size_in_bytes)
                    values = (VERSION, ResponseCode.registerSuccess.value, CLIENT_ID_LEN, *(tuple(uid.bytes)))
                    struct.pack_into(frmt, buf, 0, *values)
                    conn.sendall(buf)

                elif code == RequestCode.clientsList.value:
                    clients = self.__conn.execute(ALL_CLIENTS_QUERY).fetchall()
                    frmt = '<BHI' + (len(clients) - 1) * (NAME_MAX_SIZE + CLIENT_ID_LEN) * 'B'
                    buf_size_in_bytes = struct.calcsize(frmt)
                    buf = ctypes.create_string_buffer(buf_size_in_bytes)
                    values = (
                        VERSION, ResponseCode.clientList.value,
                        (len(clients) - 1) * (NAME_MAX_SIZE + CLIENT_ID_LEN))
                    for entry in clients:
                        if source_cid != self.BYTES_INTARRAY(entry[0]):  # the requester shouldn't get itself as client
                            values += self.BYTES_INTARRAY(entry[0]) + self.BYTES_INTARRAY(entry[1])
                    struct.pack_into(frmt, buf, 0, *values)
                    conn.sendall(buf)

                elif code == RequestCode.pullClientPubKey.value:
                    clients = self.__conn.execute(ALL_CLIENTS_QUERY).fetchall()
                    # receive the wanted client id
                    data = conn.recv(CLIENT_ID_LEN)
                    requested_cid = struct.unpack('<' + CLIENT_ID_LEN * 'B', data)
                    # check if the username exists in the clients
                    public_key = ()
                    for entry in clients:
                        if self.BYTES_INTARRAY(entry[0]) == requested_cid:
                            public_key = self.BYTES_INTARRAY(entry[2])
                    if public_key == ():
                        self.send_error(conn)
                        return
                    frmt = '<BHI' + (CLIENT_ID_LEN + PUB_KEY_SIZE) * 'B'
                    buf_size_in_bytes = struct.calcsize(frmt)
                    buf = ctypes.create_string_buffer(buf_size_in_bytes)
                    values = (
                        VERSION, ResponseCode.pubKey.value, CLIENT_ID_LEN + PUB_KEY_SIZE, *(requested_cid + public_key))
                    struct.pack_into(frmt, buf, 0, *values)
                    conn.sendall(buf)

                elif code == RequestCode.sendMsg.value:
                    # receive the rest of the packet
                    data = conn.recv(CLIENT_ID_LEN + MESSAGE_TYPE_LEN + CONTENT_SIZE_LEN)
                    destination_cid = struct.unpack('<' + 16 * 'B', data[:16])
                    msg_type, content_size = struct.unpack_from('<BI', data, 16)

                    data = conn.recv(content_size)
                    self.__conn.execute(MESSAGES_INSERT_QUERY, (
                        self.__index, self.INTARRAY_BYTES(destination_cid), self.INTARRAY_BYTES(source_cid), msg_type,
                        data))
                    self.__conn.commit()

                    frmt = '<BHI' + (CLIENT_ID_LEN + MESSAGE_ID_LEN) * 'B'
                    buf_size_in_bytes = struct.calcsize(frmt)
                    buf = ctypes.create_string_buffer(buf_size_in_bytes)
                    index_in_bytes = struct.pack("I", self.__index)
                    values = (VERSION, ResponseCode.msgSent.value, CLIENT_ID_LEN + MESSAGE_ID_LEN,
                              *(destination_cid + tuple(index_in_bytes)))
                    self.__index += 1
                    struct.pack_into(frmt, buf, 0, *values)
                    conn.sendall(buf)

                elif code == RequestCode.pullMsgs.value:
                    frmt = '<BHI'
                    values = (VERSION, ResponseCode.msgPull.value)  # prepare the tuple to be packed and sent
                    payload = ()
                    payload_size = 0

                    messages = self.__conn.execute(MESSAGES_SELECT_QUERY, (self.INTARRAY_BYTES(
                        source_cid),)).fetchall()  # pull messages where msg destination is for the client who requested to pull

                    for msg in messages:  # loop over the messages
                        this_msg_size_in_bytes = (
                                CLIENT_ID_LEN + MESSAGE_ID_LEN + MESSAGE_TYPE_LEN + CONTENT_SIZE_LEN + len(
                            self.BYTES_INTARRAY(msg[4])))
                        payload_size += this_msg_size_in_bytes  # update the total size
                        # add the message content
                        content_len = len(msg[4])
                        payload += (self.BYTES_INTARRAY(msg[2]) + tuple(struct.pack("I", msg[0])) + tuple(
                            struct.pack("B", int(msg[3]))) +
                                    tuple(struct.pack("I", content_len)) + self.BYTES_INTARRAY(msg[4]))
                        frmt += (this_msg_size_in_bytes * 'B')

                    # remove the messages
                    self.__conn.execute(MESSAGES_DELETE_QUERY, (self.INTARRAY_BYTES(source_cid),))
                    self.__conn.commit()
                    # send the response packet
                    values = (*values, payload_size, *payload)
                    buf_size_in_bytes = struct.calcsize(frmt)
                    buf = ctypes.create_string_buffer(buf_size_in_bytes)
                    struct.pack_into(frmt, buf, 0, *values)
                    conn.sendall(buf)

        except socket.error:
            print("Error while trying to process client request")
        except sq.OperationalError:
            if self.__conn:
                self.__conn.close()
                print("Error while accessing the database")
                self.send_error(conn)

    @staticmethod
    def send_error(conn):
        frmt = '<BHI'
        buf_size_in_bytes = struct.calcsize(frmt)
        buf = ctypes.create_string_buffer(buf_size_in_bytes)
        values = (VERSION, ResponseCode.error.value, ZERO)
        struct.pack_into(frmt, buf, 0, *values)
        conn.sendall(buf)

    @staticmethod
    def BYTES_INTARRAY(x):
        data_ints = struct.unpack('<' + 'B' * len(x), x)
        return data_ints

    @staticmethod
    def INTARRAY_BYTES(x):
        data_bytes = struct.pack('<' + 'B' * len(x), *x)
        return data_bytes
