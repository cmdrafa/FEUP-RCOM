#!/bin/bash

ifconfig eth0 up
ifconfig eth0 172.16.30.0/24
route add default gw 172.16.30.254

