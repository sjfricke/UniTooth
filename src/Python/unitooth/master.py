
"""
unitooth - master utilities
"""

# built-in
import bluetooth

# package
from . import SDP_NAME, SDP_UUID

def listen(protocol):
    """ server socket wrapper """

    if protocol != bluetooth.RFCOMM and protocol != bluetooth.L2CAP:
        print "protocol not valid."
        return False

    ssock = bluetooth.BluetoothSocket(protocol)
    port = bluetooth.get_available_port(protocol)

    bluetooth.advertise_service(ssock, SDP_NAME, SDP_UUID)

    # blocks?
    csock, addr = ssock.accept()

    print "Accepted connection from ", addr

    data = csock.recv(1024)
    print "received [%s]" % data

    csock.close()
    ssock.close()

    return True
