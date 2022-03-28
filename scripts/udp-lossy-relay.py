#!/usr/bin/python3
# A simple error prone UDP relay to test program responses on bad internet connections.
# Should be used as a midpoint (man-in-the-middle) between point A and point B.

import argparse
import socket
from random import random, shuffle, choice
from time import sleep
import string


# Debug messages flag (print each packet sent)
DEBUG = False

# Message buffer size
BUFFER_SIZE = 16

# Port to be used by the relay
PORT = 9797

# Fail communication parameters
latency = 100e-3
lose = 0.2
corrupt = 0.2
corrupt_how_much = 0.2  # How much of the package will be corrupted, if the package is corrupted
reorder = 0

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
    global received

    try:
        while True:
            # Receives message and compares
            data, address = udp_socket.recvfrom(BUFFER_SIZE)
            debug_print(f"Received '{data}' from '{address[0]}:{address[1]}'.")

            if address == a_address:
                received += 1
                debug_print(f"Forwarding to {b_address}...")
                try_sending(udp_socket, data, b_address)

            elif address == b_address:
                received += 1
                debug_print(f"Forwarding to {a_address}...")
                try_sending(udp_socket, data, a_address)

            else:
                debug_print("Packet dropped, unknown address.")

    except KeyboardInterrupt:
        udp_socket.close()


def try_sending(udp_socket, data, address):
    global sent, lost, reordered, corrupted

    # Might fail randomly
    sleep(latency)
    if random() < lose:
        lost += 1
        debug_print("Lost package!")
    else:
        if random() < corrupt:
            corrupted += 1
            data = corrupt_data(data)
            debug_print("Corrupted package!")
        if random() < reorder:
            reordered += 1
            debug_print("Reordering packages!")
            reordering.append((data, address))
        else:
            # Sends all reordered packages
            if reordering:
                send_out_of_order(udp_socket)

            # Sends package normally
            udp_socket.sendto(data, address)
            sent += 1

 
def corrupt_data(data):
    corrupted = ""
    for c in data.decode():
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
    for data, address in reordering:
        udp_socket.sendto(data, address)
        sent += 1
    reordering = []


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

