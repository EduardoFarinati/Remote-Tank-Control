#!/bin/bash
# Remote internal passphrase: pass12
ssh -o 'PubkeyAcceptedKeyTypes +ssh-rsa' -Y eng10048_5@pandora.ece.ufrgs.br -i ~/ufrgs_protocolos_key
