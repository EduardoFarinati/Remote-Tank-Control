#!/usr/bin/python3
# A simple error prone UDP relay to test program responses on bad internet connections.
# Should be used as a midpoint (man-in-the-middle) between point A and point B.

import argparse
import socket
from random import random, uniform, shuffle, choice
from time import sleep
import string


# Debug messages flag (print each packet sent)
DEBUG = False

# Message buffer size
BUFFER_SIZE = 16

# Port to be used by the relay
PORT = 9797

# Fail communication parameters
average_latency = 100e-3
latency_spread = 0.25
lose = 0.1
corrupt = 0.02
corrupt_how_much = 0.2  # How much of the package will be corrupted, given that it is corrupted
reorder = 0.25

# Buffer of out of order packages
reordering = []

# Info counters
received = 0
sent = 0
lost = 0
corrupted = 0
reordered = 0

# List of random possible corrupted characters
corrupted_characters = string.ascii_letters + string.digits

# Range of latencies between messages
latency_min = average_latency - (latency_spread * average_latency)
latency_max = average_latency + (latency_spread * average_latency)


def debug_print(message):
    if DEBUG:
        print(message)


def parse_args():
    parser = argparse.ArgumentParser(
        description="A lossy UDP relay to simulate bad connections",
        prog="udp-lossy-relay"
    )
    parser.add_argument(
        "a_address",
        help="IP address and port of endpoint A in format 0.0.0.0:0000"
    )
    parser.add_argument(
        "b_address",
        help="IP address and port of endpoint A in format 0.0.0.0:0000"
    )

    args = parser.parse_args()

    # Splits each address on ':'
    a_ip, a_port = tuple(args.a_address.split(':'))
    b_ip, b_port = tuple(args.b_address.split(':'))

    return (a_ip, int(a_port)), (b_ip, int(b_port))


def bind_port():
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind(("", PORT))

    return udp_socket


def relay(udp_socket, a_address, b_address):
    try:
        while True:
            data, address = receive_package(udp_socket)
            debug_print(f"Received '{data}' from '{address[0]}:{address[1]}'.")

            if address == a_address:
                debug_print(f"Forwarding to {b_address}...")
                package = (data, b_address)
                try_sending(udp_socket, package)
            elif address == b_address:
                debug_print(f"Forwarding to {a_address}...")
                package = (data, a_address)
                try_sending(udp_socket, package)
            else:
                debug_print("Packet dropped, unknown address.")

    except KeyboardInterrupt:
        udp_socket.close()


def receive_package(udp_socket):
    global received

    # Receives message and compares
    package = udp_socket.recvfrom(BUFFER_SIZE)
    received += 1

    return package


def try_sending(udp_socket, package):
    # Might fail randomly
    if random() < lose:
        lose_package()
    else:
        if random() < corrupt:
            package = corrupt_package(package)
        if random() < reorder:
            reorder_package(package)
        else:
            # Sends all reordered packages
            if reordering:
                send_out_of_order(udp_socket)

            # Sends package "normally"
            send_package(udp_socket, package)



def lose_package():
    global lost

    lost += 1
    debug_print("Lost package!")
 

def corrupt_package(package):
    global corrupted

    corrupted += 1
    debug_print("Corrupted package!")

    # Will not corrupt address (it would work as a lost package)
    data, address = package
    data = corrupt_bytes(data)
    corrupted_package = (data, address)

    return corrupted_package


def reorder_package(package):
    global reordered

    reordered += 1
    debug_print("Reordering packages!")

    # Appends to the list of reordering packages
    # will be sent together out of order
    reordering.append(package)


def corrupt_bytes(byte_str):
    corrupted = ""

    for c in byte_str.decode():
        if random() < corrupt_how_much:
            corrupted += random_char()
        else:
            corrupted += c

    return corrupted.encode()


def random_char():
    return choice(corrupted_characters)


def send_out_of_order(udp_socket):
    global reordering, sent

    # Reorder packages
    shuffle(reordering)

    # Sends them
    for package in reordering:
        send_package(udp_socket, package)
    reordering = []


def send_package(udp_socket, package):
    global sent

    # Sends package with latency
    latency()
    data, address = package
    udp_socket.sendto(data, address)
    sent += 1


def latency():
    simulated_latency = uniform(latency_min, latency_max)
    debug_print(f"Simulated latency of {simulated_latency * 1e3 : .2f} ms.")
    sleep(simulated_latency)


def main():
    a_address, b_address = parse_args()

    # Opens socket
    udp_socket = bind_port()

    # Starts relaying packages
    print(f"Listening on port {PORT}...")
    print(f"Point a - {a_address[0]}:{a_address[1]}")
    print(f"Point b - {b_address[0]}:{b_address[1]}")
    print("")
    relay(udp_socket, a_address, b_address)

    # Prints statistics
    print("Package statistics")
    print(f"  Received: {received}")
    print(f"  Sent: {sent}")
    print(f"  Lost: {lost}")
    print(f"  Corrupted: {corrupted}")
    print(f"  Reordered: {reordered}")
    print("")


if __name__ == "__main__":
    main()
