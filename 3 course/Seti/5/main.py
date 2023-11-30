import socket
import threading
import time

import select

SOCKS_VERSION = 5


class Proxy:

    def __init__(self):
        """
        Initializes a new instance of the class.
        """
        self.username = "username"
        self.password = "password"
        self.connections = {}
        self.server_socket = None
        # self.is_need_to_close = {}

    def handle_client(self, connection):
        """
        Handles a client connection.

        Args:
            connection: The socket connection to the client.

        Returns:
            None
        """
        # greeting header
        # read and unpack 2 bytes from a client
        version, nmethods = connection.recv(2)

        # get available methods [0, 1, 2]
        methods = self.get_available_methods(nmethods, connection)


        # accept only USERNAME/PASSWORD auth
        if 2 not in set(methods):
            # close connection
            print(methods)
            connection.close()
            return

        # send welcome message
        connection.sendall(bytes([SOCKS_VERSION, 2]))

        if not self.verify_credentials(connection):
            return

        # request (version=5)
        version, cmd, _, address_type = connection.recv(4)

        if address_type == 1:  # IPv4
            address = socket.inet_ntoa(connection.recv(4))
        elif address_type == 3:  # Domain name
            domain_length = connection.recv(1)[0]
            address = connection.recv(domain_length)
            address = socket.gethostbyname(address)

        # convert bytes to unsigned short array
        port = int.from_bytes(connection.recv(2), 'big', signed=False)

        try:
            if cmd == 1:  # CONNECT
                remote = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                remote.connect((address, port))
                bind_address = remote.getsockname()
                print("* Connected to {} {}".format(address, port))
            else:
                connection.close()

            addr = int.from_bytes(socket.inet_aton(bind_address[0]), 'big', signed=False)
            port = bind_address[1]

            reply = b''.join([
                SOCKS_VERSION.to_bytes(1, 'big'),
                int(0).to_bytes(1, 'big'),
                int(0).to_bytes(1, 'big'),
                int(1).to_bytes(1, 'big'),
                addr.to_bytes(4, 'big'),
                port.to_bytes(2, 'big')
            ])
        except Exception as e:
            # return connection refused error
            reply = self.generate_failed_reply(address_type, 5)

        connection.sendall(reply)

        if reply[1] == 0 and cmd == 1:
            self.connections[connection] = remote
            self.connections[remote] = connection

        # establish data exchange
        # if reply[1] == 0 and cmd == 1:
        #     self.exchange_loop(connection, remote)

        # connection.close()

    def exchange_loop(self):
        """
        Runs an infinite loop that exchanges data between a client and a remote server.

        Args:
            client (socket): The client socket object.
            remote (socket): The remote server socket object.

        Returns:
            None
        """
        while True:
            # wait until client or remote is available for read
            # print(list(self.connections.keys()))
            # time.sleep(1)
            # print(list(self.connections.keys()))
            r, w, e = select.select(list(self.connections.keys()), [], [], 1)

            # print(r)
            for socket in r:
                if socket in self.connections.keys():
                    data = socket.recv(4096)
                    remote = self.connections[socket]
                    if remote.send(data) <= 0:
                        self.connections.pop(socket)
                        self.connections.pop(remote)
                        socket.close()
                        remote.close()



    def generate_failed_reply(self, address_type, error_number):
        """
        Generate a failed reply for a SOCKS proxy connection.

        Args:
            address_type (int): The type of address being requested.
            error_number (int): The error number indicating the reason for the failure.

        Returns:
            bytes: The generated failed reply as a byte string.
        """
        return b''.join([
            SOCKS_VERSION.to_bytes(1, 'big'),
            error_number.to_bytes(1, 'big'),
            int(0).to_bytes(1, 'big'),
            address_type.to_bytes(1, 'big'),
            int(0).to_bytes(4, 'big'),
            int(0).to_bytes(4, 'big')
        ])

    def verify_credentials(self, connection):
        """
        Verifies the credentials provided by the client.

        Args:
            connection (socket): The connection object representing the client connection.

        Returns:
            bool: True if the credentials are valid, False otherwise.
        """
        version = ord(connection.recv(1))  # should be 1

        username_len = ord(connection.recv(1))
        username = connection.recv(username_len).decode('utf-8')

        password_len = ord(connection.recv(1))
        password = connection.recv(password_len).decode('utf-8')

        print(username)
        print(password)

        if username == self.username and password == self.password:
            # success, status = 0
            response = bytes([version, 0])
            connection.sendall(response)
            return True

        # failure, status != 0
        response = bytes([version, 0xFF])
        connection.sendall(response)
        connection.close()
        return False

    def get_available_methods(self, nmethods, connection):
        """
        Get the available methods from a connection.

        Args:
            nmethods (int): The number of methods to retrieve.
            connection: The connection object to retrieve the methods from.

        Returns:
            list: A list of available methods.
        """
        methods = []
        for i in range(nmethods):
            methods.append(ord(connection.recv(1)))
        return methods

    def clients_accept(self):
        while True:
            conn, addr = self.server_socket.accept()
            self.handle_client(conn)
            print("* new connection from {}".format(addr))

    def run(self, host, port):
        """
        Run the Socks5 proxy server on the specified host and port.

        Parameters:
            host (str): The IP address or hostname of the host to bind the server to.
            port (int): The port number to bind the server to.

        Returns:
            None
        """
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.bind((host, port))
        self.server_socket.listen()

        print("* Socks5 proxy server is running on {}:{}".format(host, port))

        t = threading.Thread(target=self.clients_accept)
        t.start()

        self.exchange_loop()


        # while True:
        #     conn, addr = s.accept()
        #     print("* new connection from {}".format(addr))
        #     t = threading.Thread(target=self.handle_client, args=(conn,))
        #     t.start()


if __name__ == "__main__":
    port = int(input("Enter port: "))
    proxy = Proxy()
    proxy.run("127.0.0.1", port)
