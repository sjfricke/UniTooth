using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public class unitooth : MonoBehaviour {

    private static int AF_BTH = 32; //bluetooth address family
    private static int SOCK_STREAM = 1; //socket type of TCP
    private static int BTHPROTO_RFCOMM = 3; //RFCOMM protocol type
    private static int INVALID_SOCKET = -1; //invalid socket identifier
    private static int uniSocket;
    public byte[] btaddr;

    public string macAddress;

    public struct SOCKADDR_BTH
    {
        public ushort addressFamily;
        public byte[] btAddr;
        public System.Guid serviceClassId;
        public ulong port;
    };

#if UNITY_STANDALONE_WIN || UNITY_EDITOR
    [DllImport("ws2_32.dll", EntryPoint = "socket")]
    public static extern int socket(int af, int type, int protocol);
    [DllImport("ws2_32.dll", EntryPoint = "connect")]
    public static extern int connect(int s, SOCKADDR_BTH name, int namelen);
    [DllImport("ws2_32.dll", EntryPoint = "WSAGetLastError")]
    public static extern int WSAGetLastError();

    public static int socket()
    {
        return socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    }

    public static int connect(int socketfd, byte[] btaddr)
    {
        SOCKADDR_BTH newSocketAddr = new SOCKADDR_BTH();
        newSocketAddr.addressFamily = (ushort)AF_BTH;
        newSocketAddr.btAddr = btaddr;
        newSocketAddr.port = 0;
        return connect(socketfd, newSocketAddr, sizeof(ushort) + 6 + sizeof(ulong));

    }
#endif

    // Use this for initialization
    void Start () {
        uniSocket = socket();
        if (uniSocket == INVALID_SOCKET)
        {
            Debug.LogError("Unable to start bluetooth. Please make sure Bluetooth is enabled on your computer");
        }

        if (macAddress != null)
        {
            macAddress = macAddress.Replace("-", ""); //get rid of any shit people put into the string
            macAddress = macAddress.Replace(":", "");
            macAddress = macAddress.Replace(".", "");

            Debug.Log(macAddress);
            if (macAddress.Length != 12)
            {
                Debug.LogError("Invalid MAC Address entered");
            }
            string[] bytes = new string[6];
            int j = 0;
            for (int i = 0; i < 12; i += 2)
            {
                bytes[j] = macAddress.Substring(i, 2);
                j++;
            }
            btaddr = new byte[6];
            for (int i = 0; i < 6; i++)
            {
                btaddr[i] = byte.Parse(bytes[i], System.Globalization.NumberStyles.HexNumber);
            }
            int res = connect(uniSocket, btaddr);
            Debug.Log("Result: " + res);
            if (res == -1)
            {
                Debug.Log("Error: " + WSAGetLastError());
            }
        } else {
            Debug.LogError("Please make sure to enter MAC Address of target device into script");
        }
        
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
