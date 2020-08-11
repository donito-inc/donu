#!/bin/bash -ev

mkdir -p ~/.donu
echo "rpcuser=username" >>~/.donu/donu.conf
echo "rpcpassword=`head -c 32 /dev/urandom | base64`" >>~/.donu/donu.conf

