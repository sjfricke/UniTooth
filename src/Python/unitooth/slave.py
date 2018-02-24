
"""
unitooth - slave utilities
"""

# built-in
import bluetooth

# package
from . import SDP_NAME, SDP_UUID

def connect(protocol):
    """ client socket wrapper """

    if protocol != bluetooth.RFCOMM and protocol != bluetooth.L2CAP:
        print "protocol not valid."
        return False

    matches = bluetooth.find_service(uuid=SDP_UUID, name=SDP_NAME)

    if not len(matches):
        print "No %s services found." % SDP_NAME
        return False

    for match in matches:
        print "'%s' %s:%s" % match["name"], match["host"], match["port"]

    sock = bluetooth.BluetoothSocket(protocol)
    sock.connect((match["host"], match["port"]))
    sock.send("hello!!")
    sock.close()

    return True
