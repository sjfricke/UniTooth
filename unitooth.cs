using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Net.Sockets;

public class unitooth : MonoBehaviour {

    private static int AF_BTH = 32; //bluetooth address family
    private static int SOCK_STREAM = 1; //socket type of TCP
    private static int BTHPROTO_RFCOMM = 3; //RFCOMM protocol type
    private static int uniSocket;

#if UNITY_STANDALONE_WIN || UNITY_EDITOR
    [DllImport("ws2_32.dll", EntryPoint = "socket")]
    public static extern int socket(int af, int type, int protocol);

    public static int socket()
    {
        return socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    }
#endif

    // Use this for initialization
    void Start () {
        uniSocket = socket();
        Debug.Log("Data: " + uniSocket);
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
