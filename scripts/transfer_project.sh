#!/bin/bash

# Removing existing project in remote
ssh -o 'PubkeyAcceptedKeyTypes +ssh-rsa' eng10048_5@pandora.ece.ufrgs.br -i ~/ufrgs_protocolos_key "rm -rf Remote-Tank-Control"

# Make directory in remote
ssh -o 'PubkeyAcceptedKeyTypes +ssh-rsa' eng10048_5@pandora.ece.ufrgs.br -i ~/ufrgs_protocolos_key "mkdir -p Remote-Tank-Control"

# Copy project main files
scp -o 'PubkeyAcceptedKeyTypes +ssh-rsa'  -i ~/ufrgs_protocolos_key ./Makefile eng10048_5@pandora.ece.ufrgs.br:~/Remote-Tank-Control/Makefile
scp -o 'PubkeyAcceptedKeyTypes +ssh-rsa'  -i ~/ufrgs_protocolos_key -r ./src eng10048_5@pandora.ece.ufrgs.br:~/Remote-Tank-Control/src
