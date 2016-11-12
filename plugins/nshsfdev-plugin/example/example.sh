CD="$( cd "$( dirname $0 )" && pwd )"

VPPDIR=$CD/../../../
CMDLINE=/tmp/nshsfdev_vpp.cmdline
STARTUP=/tmp/nshsfdev_vpp.conf
PCAP=$CD/nsh-over-vxlan-gpe.pcap
PG=/tmp/nshsfdev.pg

cat << EOF > $CMDLINE
cpu { $VPP_CPU }
unix { startup-config $STARTUP interactive }
dpdk { }
heapsize 128M
EOF

$VPPDIR/build-root/build-vpp_debug-native/vnet/pcap2pg -i $PCAP > $PG
sed -i '/->/c\0x0800: aa:aa:bb:bb:cc:00 -> aa:aa:bb:bb:cc:cc' $PG
cat $PG

cat << EOF > $STARTUP
exec $PG
set interface mac address pg0 aa:aa:bb:bb:cc:cc
EOF

make STARTUP_CONF=$CMDLINE -C $VPPDIR run

