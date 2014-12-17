#!/bin/bash
/etc/init.d/networking restart
ifconfig eth0 down
ifconfig eth0 up
ifconfig eth0 172.16.60.1/24

echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts

route add -net 172.16.61.0/24 gw 172.16.60.254
route add default gw 172.16.60.254