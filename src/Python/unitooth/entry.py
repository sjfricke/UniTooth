
"""
unitooth - main program entry
"""

# built-in
import sys
import argparse

# package
from . import connect
from . import find
from . import listen

def main():
    """ unitooth entry """

    # Initialize argparse
    parser = argparse.ArgumentParser(
        description="A PyBluez wrapper for Unity applications",
        epilog="Developed by University of Wisconsin-Madison Undergraduates",
        prog="ut"
    )

    # Add capability for subparsers
    subparser = parser.add_subparsers()

    # Add sub-commands
    find.init_args(subparser)
    listen.init_args(subparser)
    connect.init_args(subparser)

    # Prevent stack-trace dump if no arguments are given
    if len(sys.argv) == 1:
        sys.argv.append("")

    # Parse arguments and then run the chosen command
    args = parser.parse_args(sys.argv[1:])
    return args.handler(args)
