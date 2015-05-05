#!/bin/bash

if [ -f /tmp/bluewin.pid ]
  then
      PID=$(cat /tmp/bluewin.pid)
      echo "killing" $PID
      kill $PID
      rm /tmp/bluewin.pid
fi
