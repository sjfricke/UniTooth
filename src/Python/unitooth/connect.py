
"""
unitooth - connect sub-command
"""

def run(args):
    """ connect entry """

    print "Running connect"
    return 0

def init_args(parser):
    """ initialize connect's arguments """

    subp = parser.add_parser("connect")
    subp.set_defaults(handler=run)

    return 0
