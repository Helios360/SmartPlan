#!/bin/bash
touch Events.csv

echo "prio,id,year,month,day,hours,minutes,seconds,duration,peoples,desc\n" > Events.csv

gcc SmartPlan.c gui.c -o smartplan `pkg-config --cflags --libs gtk+-3.0` -rdynamic

mv Smartplan /usr/local/bin/

echo "Setup is done, you can type smartplan -help to begin"