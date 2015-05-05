#!/bin/bash

if  [ -f server ]
then
    ./server 5000 &
fi
