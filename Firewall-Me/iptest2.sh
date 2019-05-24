###!/bin/sh


sudo iptables -t  filter -A INPUT -p icmp -j ACCEPT

sudo iptables -t filter -A OUTPUT -p icmp -j ACCEPT

sudo iptables -t filter -A INPUT -p tcp --dport 22 -j DROP



