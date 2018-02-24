
"""
unitooth - listen sub-command
"""

def run(args):
    """ listen entry """

    print "Running listen"
    return 0

def init_args(parser):
    """ initialize listen's arguments """

    subp = parser.add_parser("listen")
    subp.set_defaults(handler=run)

    return 0
