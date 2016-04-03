#!/usr/bin/env python3.5

import ipaddress
import json
import os
import sys
import random
from scapy.all import *

def parse_config(path):
	global config, dst_prefix, src_prefix
	with open(path) as rfile:
		config = json.load(rfile)
		# TODO: do more validity checks on the configuration
		if config['dst_plen_min'] > config['dst_plen_max'] or config['dst_plen_inc'] <= 0:
			raise Exception('Invalid destination prefix length range')
		if config['src_plen_min'] > config['src_plen_max'] or config['src_plen_inc'] <= 0:
			raise Exception('Invalid source prefix length range')
		try:
			dst_prefix = ipaddress.IPv6Address(config['dst_prefix'])
		except KeyError:
			raise Exception('dst_prefix is not a valid IPv6 address')
		try:
			src_prefix = ipaddress.IPv6Address(config['src_prefix'])
		except KeyError:
			raise Exception('src_prefix is not a valid IPv6 address')

def generate_cli(path):
	"""
	Generates an VPP CLI route adding instructions
	Syntax: ip route add D/n from S/n via next_hop

	:arg config: configuration array
	"""
	conf_file = open(path + '.conf', "w")
	for src_plen in range(config['src_plen_min'], config['src_plen_max'] + 1, config['src_plen_inc']):
		print("ip route add", dst_prefix.compressed + "/" + str(config['dst_plen_max']), "from", ipaddress.IPv6Network(config['src_prefix'] + '/' + str(src_plen), strict= False), "via", config['gw'], file=conf_file)
	for dst_plen in range(config['dst_plen_min'], config['dst_plen_max'] + 1, config['dst_plen_inc']):
		print("ip route add", ipaddress.IPv6Network(config['dst_prefix'] + '/' + str(dst_plen), strict= False), "from", src_prefix.compressed + '/' + str(config['src_plen_max']), "via", config['gw'], file=conf_file)
	conf_file.close()

def generate_best_case_packets(path):
	src_prefix = ipaddress.IPv6Network(config['src_prefix'] + '/' + str(config['src_plen_max']), strict=False)
	src_address = src_prefix.network_address.compressed
	dst_prefix = ipaddress.IPv6Network(config['dst_prefix'] + '/' + str(config['dst_plen_max']), strict=False)
	dst_address = dst_prefix.network_address.compressed
	pkt = Ether(dst=config['next_hop_mac']) / IPv6(src = src_address, dst = dst_address) / UDP(dport=40, sport=RandShort())
	wrpcap(path + "_best.pcap",pkt)

def generate_worse_case_packets(path):
	src_prefix = ipaddress.IPv6Network(config['src_prefix'] + '/' + str(config['src_plen_min']), strict=False)
	src_address = src_prefix.network_address.compressed
	dst_prefix = ipaddress.IPv6Network(config['dst_prefix'] + '/' + str(config['dst_plen_min']), strict=False)
	dst_address = dst_prefix.network_address.compressed
	pkt = Ether(dst=config['next_hop_mac']) / IPv6(src = src_address, dst = dst_address) / UDP(dport=40, sport=RandShort())
	wrpcap(path + "_worse.pcap",pkt)

def random_address(prefix, plen):
	addr_as_int = int(ipaddress.IPv6Address(prefix))
	print("Prefix", prefix, ", plen ", plen, ", addr_as_int", addr_as_int)
	if plen < 128:
		# need to flip the bit at plen + 1 to ensure that this address is not part of a longer prefix
		bit_to_flip = 1 << (128 - plen)
		addr_as_int = addr_as_int ^ bit_to_flip
	print(ipaddress.IPv6Address(addr_as_int).compressed)
	return ipaddress.IPv6Address(addr_as_int).compressed

#
# Could be tricky as the src/dst should be random in the plen but must not be in the plen+1
# Random bits on the right + if member of the longer prefix then flip the plen bit?

def generate_random_packets(path):
	print("Not implemented yet") 
	pkts = []
	for i in range(config['random_packet_count']):
		print(i)
		dst_address = random_address(config['dst_prefix'], random.randint(config['dst_plen_min'], config['dst_plen_max']))
		src_address = random_address(config['src_prefix'], random.randint(config['src_plen_min'], config['src_plen_max']))
		pkt = Ether(dst=config['next_hop_mac']) / IPv6(src = src_address, dst = dst_address) / UDP(dport=40, sport=RandShort())
		pkts.append(pkt)
	wrpcap(path + "_random.pcap",pkts)

def main(path):
	if os.path.exists(path + '.json'):
		parse_config(path + '.json')
		generate_cli(path)
		generate_worse_case_packets(path)
		generate_best_case_packets(path)
		random.seed()
		generate_random_packets(path)
	else:
		print(sys.argv[0] + " config (no .JSON)")
		sys.exit(-1)


if __name__ == '__main__':
    if len(sys.argv) == 2:
        main(sys.argv[1])
    else:
        sys.exit(-1)
    sys.exit(0)
