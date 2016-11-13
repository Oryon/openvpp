#!/bin/bash

CD="$( cd "$( dirname $0 )" && pwd )"

VPPDIR=$CD/../../../
CMDLINE=/tmp/nshsfdev_vpp.cmdline
STARTUP=/tmp/nshsfdev_vpp.conf
PCAP=$CD/nsh-over-vxlan-gpe.pcap
#PG=/tmp/nshsfdev.pg
PG=$CD/nshsfdev.pg
EXAMPLE_PLUGIN=$CD/libsf-example.so.1.0.1

make -C $CD all || exit 1

cat << EOF > $CMDLINE
cpu { $VPP_CPU }
unix { startup-config $STARTUP interactive }
dpdk { }
heapsize 128M
EOF

#$VPPDIR/build-root/build-vpp_debug-native/vnet/pcap2pg -i $PCAP > $PG
#sed -i '/->/c\0x0800: aa:aa:bb:bb:cc:00 -> aa:aa:bb:bb:cc:cc' $PG
cat $PG

cat << EOF > $STARTUP
exec $PG
set interface mac address pg0 aa:aa:bb:bb:cc:cc
nsh load $EXAMPLE_PLUGIN
create nsh entry nsp 10 nsi 1 md-type 1
create nsh map nsp 10 nsi 1 mapped-nsp 10 mapped-nsi 1 nsh_action pop encap-other nshsfdev 0
create nsh entry nsp 10 nsi 0 md-type 1
create nsh map nsp 10 nsi 0 mapped-nsp 10 mapped-nsi 0 nsh_action pop encap-ethernet 1 aaaabbbbccccaaaabbbbcc00894f
trace add pg-input 1
pack en
EOF

make STARTUP_CONF=$CMDLINE -C $VPPDIR run

