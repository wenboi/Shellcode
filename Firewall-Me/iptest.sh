#!/bin/sh

iptables -t filter -F

iptables -t filter -A INPUT -p tcp --dport 22 -j ACCEPT

iptables -t filter -A OUTPUT -p tcp --sport 22 -j ACCEPT

iptables -t filter -A OUTPUT -p icmp -j DROP

iptables -t filter -P INPUT DROP

iptables -t filter -P OUTPUT  ACCEPT

iptables -t filter -P FORWARD  DROP
