#!/bin/bash
#要在root权限下.add/del/change
tc qdisc add dev eth0 root netem delay 100ms 10ms
