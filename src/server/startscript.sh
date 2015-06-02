#!/bin/bash
# author David Boeryd

if  [ -f server ]
then
    ./server 5000 &
fi
