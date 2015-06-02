#!/bin/bash
#author David Boeryd
#taken partly from here http://www.linuxquestions.org/questions/linux-newbie-8/how-to-read-a-pid-from-a-text-file-and-kill-the-corresponding-process-816562/ answer written by user smoke about reading from file
if [ -f /tmp/bluewin.pid ]
  then
      PID=$(cat /tmp/bluewin.pid)
      echo "killing" $PID
      kill $PID
      rm /tmp/bluewin.pid
fi
