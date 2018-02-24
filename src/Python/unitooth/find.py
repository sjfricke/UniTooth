
"""
unitooth - find sub-command
"""

def run(args):
    """ find entry """

    print "Running find"
    return 0

def init_args(parser):
    """ initialize find's arguments """

    subp = parser.add_parser("find")
    subp.set_defaults(handler=run)

    return 0
