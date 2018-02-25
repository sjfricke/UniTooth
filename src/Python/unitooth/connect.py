
"""
unitooth - connect sub-command
"""

# built-in
import bluetooth

# package
from . import slave

def run(args):
    """ connect entry """

    print "Running connect"
    slave.connect(bluetooth.RFCOMM)
    return 0

def init_args(parser):
    """ initialize connect's arguments """

    subp = parser.add_parser("connect")
    subp.set_defaults(handler=run)

    return 0
