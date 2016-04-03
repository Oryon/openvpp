#!/bin/sh
export IF=$1
export DMAC=`ip -6 neigh show dev $IF | cut -f 3 -d \ `
sudo tcpreplay-edit --enet-dmac=$DMAC --loop=1 --preload-pcap -i $IF eric_best.pcap
