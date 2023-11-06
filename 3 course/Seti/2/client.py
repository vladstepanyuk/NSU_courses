import socket
from threading import Lock, Thread, Event
import time
import os
import tqdm
import json
from enum import Enum

context = {}

def main():

    with open('config.json', 'r') as file:
        context = json.load(file)
        context['filepath'] = input("Enter filepath: ")
        context['ip'] = input("Enter ip: ")
        context['port'] = int(context['port'])
        context['delay'] = float(context['delay'])

    filesize = os.path.getsize(context['filepath'])

    server_ip = context['ip']
    server_port = context['port']
    my_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    my_socket.connect((server_ip, server_port))

    basename_file = os.path.basename(context['filepath']).encode("utf-8")

    size = my_socket.send(len(basename_file).to_bytes(2, 'big'))
    if size != 2:
        print("Error: Server not available")
        my_socket.close()
        return

    size = my_socket.send(basename_file)

    if size != len(basename_file):
        print("Error: Server not available")
        my_socket.close()
        return

    size = my_socket.send(filesize.to_bytes(8, 'big'))
    if size != 8:
        print("Error: Server not available")
        my_socket.close()
        return

    with open(context['filepath'], "rb") as file:
        for i in tqdm.tqdm(range(0, filesize, 1024)):
            data = file.read(1024 if filesize - i > 1024 else filesize - i)
            size = my_socket.send(data)
            time.sleep(context['delay'])
            if size != len(data):
                print("Error: Server not available")
                my_socket.close()
                return

    my_socket.close()
    print("Done")


if __name__ == "__main__":
    main()
