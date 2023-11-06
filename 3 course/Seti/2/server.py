import socket
from threading import Lock, Thread, Event
import time
import os
import json
from prettytable import PrettyTable
from enum import Enum

STATE = Enum("STATE", ["RUNNING", "SUCCESS", "FAILED"])


class client_info:
    def __init__(self, client_socket: socket) -> None:
        self.client_socket = client_socket
        self.mutex = Lock()
        self.average_speed = 0
        self.time = 0
        self.speed = 0
        self.state = STATE.RUNNING
        self.file_name = ""
        self.size = 0
        self.uploaded_size = 0
        self.addr = ''


def process_client(client_info: client_info, flag: Event, context: dict):
    data = client_info.client_socket.recv(2)

    size_name = int.from_bytes(data, byteorder='big')
    name = client_info.client_socket.recv(size_name).decode("utf-8")
    with client_info.mutex:
        client_info.file_name = name
        client_info.addr = client_info.client_socket.getsockname()[0]

    size_data = int.from_bytes(client_info.client_socket.recv(8), byteorder='big')

    with client_info.mutex:
        client_info.size = size_data

    with open(f"./uploads/{name}", "wb", ) as file:
        uploaded_size = 0
        while not flag.is_set() and uploaded_size < size_data:
            start = time.monotonic()
            data = client_info.client_socket.recv(
                min(size_data - uploaded_size, 1024),
                socket.MSG_WAITALL
            )

            if not data:
                client_info.client_socket.close()
                client_info.state = STATE.FAILED
                return
            file.write(data)
            end = time.monotonic()
            uploaded_size += len(data)
            with client_info.mutex:
                client_info.speed = uploaded_size / (end - start)
                client_info.time += end - start
                client_info.average_speed = uploaded_size / client_info.time
                client_info.uploaded_size = uploaded_size
            time.sleep(context['delay'])

    with client_info.mutex:
        client_info.client_socket.close()
        client_info.state = STATE.FAILED if client_info.uploaded_size != client_info.size else STATE.SUCCESS


def print_client_info(client: client_info):
    with client.mutex:
        print(
            f"{client.addr}\t"
            f"{client.file_name}\t{client.time:.2f}\t"
            f"{client.speed:.2f}\t{client.average_speed:.2f}\t"
            f"{client.state.name}\t{client.uploaded_size}/{client.size}%"
        )


def printing_info_task(clients: list, flag: Event):
    th = ['IP', 'NAME', 'TIME', 'SPEED', 'AVERAGE_SPEED', 'STATE', 'PROGRESS']

    while not flag.is_set():
        table = PrettyTable(th)
        # os.system("cls||clear")

        for client in clients:
            table.add_row([f"{client.addr}", f"{client.file_name}", f"{client.time:.2f}",
                           f"{client.speed:.2f}", f"{client.average_speed:.2f}",
                           f"{client.state.name}", f"{client.uploaded_size}/{client.size}%"])
            if client.state == STATE.SUCCESS or client.state == STATE.FAILED:
                clients.pop(clients.index(client))
        print(table)
        time.sleep(3)


def wait_all_clients(clients: list):
    while len(clients) > 0:
        for i in clients:
            if i.state != STATE.RUNNING:
                clients.pop(clients.index(i))
        time.sleep(1)


def main():
    with open("config.json", 'r') as file:
        context = json.load(file)
        context['delay'] = float(context['delay'])
        context['port'] = int(context['port'])

    my_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    my_socket.bind(("", int(context['port'])))

    my_socket.listen(5)
    clients = []

    flag = Event()

    try:
        printing_info_thread = Thread(target=printing_info_task, args=(clients, flag))
        printing_info_thread.start()
        while True:
            client_socket, client_address = my_socket.accept()
            client = client_info(client_socket)
            clients.append(client)

            Thread(target=process_client, args=(client, flag, context), daemon=False).start()
    except KeyboardInterrupt:
        flag.set()
        printing_info_thread.join()
        wait_all_clients(clients)
        my_socket.close()
        print("Done")


if __name__ == "__main__":
    main()
