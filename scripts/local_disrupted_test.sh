#!/bin/bash

# Run make to compile code
make clean
make
make debug

# Run UDP relay
./scripts/udp-lossy-relay.py 127.0.0.1:9696 127.0.0.1:9595

# Run server
gnome-terminal -- gdb ./debug/bin/server -p 9696 -a 127.0.0.1:9595

# Run client
gnome-terminal -- gdb ./debug/bin/client -p 9595
