#!/bin/bash
#uptime | awk '{system("date +%m-%d:%H:%M:%S"); print $0}' >>/var/log/uptime.log
nohup vmstat 10 |awk '{system("date +%m-%d:%H:%M:%S"); print $0}' >> /var/log/vmstat.log &
nohup nicstat 10 |awk '{system("date +%m-%d:%H:%M:%S"); print $0}' >> /var/log/nicstat.log &
nohup iostat 10 |awk '{system("date +%m-%d:%H:%M:%S"); print $0}' >> /var/log/iostat.log &
