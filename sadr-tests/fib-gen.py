#!/usr/bin/env python3

import ipaddress
import json
import os
import sys

def parse_routing_info_file(path):
    """
    Parses a routing table info file.
    It expects the data to be JSON structured.

    :arg path: Path of the routing table info file to parse
    """
    #TODO(vkmc): JSON format has been chosen in order to perform data
    # sanitization. We should add a JSON schema control.
    # The JSON file has information of the source (opt, missing source means
    # no source constrain) and destination.
    with open(path) as rfile:
        rjson = json.load(rfile)
        print(rjson)
        gen_vpp_cli_file(rjson)

def gen_vpp_cli_file(rjson):
    """
    Generates an VPP CLI route adding instructions
    Syntax: ip route add D src S via drop

    :arg rjson: JSON file with routing information
    """
    cfile = open("cli-test", "w")

    for i in rjson:
        try:
            try:
                source = rjson[i]['S']
                source_addr = ipaddress.IPv6Network(source)
            except KeyError:
                source = None
                pass
            destination = rjson[i]['D']
            destination_addr = ipaddress.IPv6Network(destination)
            gateway = rjson[i]['G']
            gateway_addr = ipaddress.IPv6Network(gateway)
            #TODO(vkmc) Call qlixed script with the S-D tuple
            cmd = "ip route add " + destination
            if source:
                cmd += " from " + source
            cmd += " via " + gateway
            print(cmd, file=cfile)
        except ipaddress.AddressValueError as e:
            print(e + "- Not valid IPv6 address")
            sys.exit(-1)
    cfile.close()

def main(path):
    if os.path.exists(path):
        parse_routing_info_file(path)
    else:
        print("fib_gen.py path_to_routing_info.json")
        sys.exit(-1)


if __name__ == '__main__':
    if len(sys.argv) > 1:
        main(sys.argv[1])
    else:
        sys.exit(-1)
    sys.exit(0)
