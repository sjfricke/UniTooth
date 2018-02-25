
"""
unitooth - find sub-command
"""

import bluetooth

def run(args):
    """ find entry """

    print "Running find"

    nearby_devices = bluetooth.discover_devices(lookup_names=True)

    for device in nearby_devices:
        print device

    return 0

def init_args(parser):
    """ initialize find's arguments """

    subp = parser.add_parser("find")
    subp.set_defaults(handler=run)

    return 0
