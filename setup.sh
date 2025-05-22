#!/bin/bash

gcc `pkg-config gtk+-3.0 --cflags` SmartPlan.c -o SmartPlan `pkg-config gtk+-3.0 --libs`

mv Smartplan /usr/local/bin/

echo "Setup is done, you can type Smartplan -help for info"