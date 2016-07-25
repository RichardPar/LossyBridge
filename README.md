# BridgeMangle
A simple Netfilter-Bridge utility that drops packets based on a random values
that are traversing the linux bridge subsystem. This emulates a bad network link between devices.

This only works with Linux Bridges as it intercepts the FORWARD 


Building - just run MAKE 

sudo insmod bridgebreak_out.ko<br>

Setup : You need atleast 2 ethernet devices in a bridge<br>
sudo brctl addbr br0<br>
sudo brctl addif br0 eth1<br>
sudo brctl addif br0 eth2<br>
sudo ifconfig br0 up<nr>


Show Status : cat /proc/loss<br>
SET Random probability : echo 5000 > /proc/loss (5000 is an example)<br>
A value from 0 to 65535 is valid<br>


