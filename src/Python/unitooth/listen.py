
"""
unitooth - listen sub-command
"""

# built-in
import bluetooth

# package
from . import master

def run(args):
    """ listen entry """

    print "Running listen"
    master.listen(bluetooth.RFCOMM)
    return 0

def init_args(parser):
    """ initialize listen's arguments """

    subp = parser.add_parser("listen")
    subp.set_defaults(handler=run)

    return 0
