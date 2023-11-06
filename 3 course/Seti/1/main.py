import ipaddress
import threading
import uuid

# from colorama import Fore, Back, Style
from scapy.all import *
from scapy.layers.inet import UDP
from scapy.layers.inet import IP
from scapy.layers.inet6 import IPv6


mutex_dict = threading.Lock()

class Flag:
    def __init__(self):
        self.value = True

    def change_state(self):
        self.value = not self.value


def check_multicast_group(ip_str):
    try:
        ip = ipaddress.ip_address(ip_str)

        # checking if ip is a multicast IPv4 224.0.0.0 - 239.255.255.255
        if ip.version == 4 and ip.is_multicast:
            return "IPv4"

        # checking if ip is a multicast IPv6 ff00::/8
        elif ip.version == 6 and ip.is_multicast:
            return "IPv6"

        else:
            return "Not a multicast group"

    except ValueError:
        return "Not a valid IP address"


def ping_multicast(ip_multicast, sport, app_uid):
    if check_multicast_group(ip_multicast) == "IPv4":
        packet = IP(dst=ip_multicast) / UDP(sport=sport) / Raw(load=app_uid)
    else:
        packet = IPv6(dst=ip_multicast) / UDP(sport=sport) / Raw(load=app_uid)

    send(packet, verbose=False)

    # perplexity

def sniffer_multicast(ip_multicast, stop_flag: Flag, dict_apps: dict, app_uid: str):
    while stop_flag.value:
        # print(f"Sniffing data")
        packet = sniff(count=1, filter="udp and dst host " + ip_multicast, timeout=1)
        for pkt in packet:
            payload = pkt[UDP][Raw].load.decode("utf-8")
            if payload != app_uid[12:]:
                ip = pkt[IPv6].src if IPv6 in pkt else pkt[IP].src
                with mutex_dict:
                    dict_apps[payload] = (ip + ":" + str(pkt[UDP].sport), time.monotonic())
                

def copies_printer(stop_flag: Flag, set_apps: dict):
    while stop_flag.value:
        os.system('cls||clear')
        print("---------")
        print("IP\tUID")
        for key in list(set_apps):
            if time.monotonic() - set_apps[key][1] > 10:
                with mutex_dict:
                    set_apps.pop(key)
                continue
            print(f"{set_apps[key][0]}\t{key}")
        print("---------")
        time.sleep(1)


if __name__ == "__main__":
    ip_multicast = input("Enter multicast group IP: ")
    protocol = check_multicast_group(ip_multicast)

    sport = int(input("Enter port number: "))
    if sport < 0 or sport > 65535:
        print(f"Error: {sport}")
        sys.exit()

    if protocol == "Not a multicast group" or protocol == "Not a valid IP address":
        print(f"Error: {protocol}")
        sys.exit()
    else:
        print(f"Address {ip_multicast} is a multicast group protocol {protocol}")

    app_uid = str(uuid.uuid4())
    print(f"App UID: {app_uid}")

    dict_apps = dict()
    stop_flag = Flag()
    snif_thread = threading.Thread(
        target=sniffer_multicast, args=(ip_multicast, stop_flag, dict_apps, app_uid)
    )
    print_thread = threading.Thread(
        target=copies_printer,
        args=(
            stop_flag,
            dict_apps,
        ),
    )

    try:
        snif_thread.start()
        print_thread.start()
        while True:
            ping_multicast(ip_multicast, sport, app_uid)
            time.sleep(1)
    except KeyboardInterrupt:
        stop_flag.change_state()
        snif_thread.join()
        print_thread.join()
        print("Done")
        sys.exit()
