#!/bin/bash
scp -o 'PubkeyAcceptedKeyTypes +ssh-rsa'  -i ~/ufrgs_protocolos_key $1 eng10048_5@pandora.ece.ufrgs.br:~/$2
