using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Threading;

public class unitooth : MonoBehaviour
{

    public string macAddress;
    public int port;
    public int sendVal;
    private int lastSentVal = -10;

#if UNITY_STANDALONE_WIN || UNITY_EDITOR

    [DllImport("unitoothLib")]
    public unsafe static extern int uniConnect(ulong mac_address);
    [DllImport("unitoothLib")]
    public static extern int receiveData();
	[DllImport("unitoothLib")]
	public static extern int sendData(int data);
#endif

    public int data;
    bool _threadReceiveRunning;
    bool _threadSendRunning;
    Thread _threadReceive;
    Thread _threadSendinIt;


    // Use this for initialization
    void Start()
    {
        if (macAddress != null)
        {
            macAddress = macAddress.Replace("-", ""); //get rid of any shit people put into the string
            macAddress = macAddress.Replace(":", "");
            macAddress = macAddress.Replace(".", "");
            if (macAddress.Length != 12)
            {
                Debug.LogError("Invalid MAC Address entered");
            }
            ulong newVal = ulong.Parse(macAddress, System.Globalization.NumberStyles.HexNumber);
            int res = uniConnect(newVal);
            Debug.Log("Result: " + res);
        }
        else
        {
            Debug.LogError("Please make sure to enter MAC Address of target device into script");
        }
        _threadReceive = new Thread(ThreadedWorkRec);
        _threadReceive.Start();

    }

    void ThreadedWorkRec()
    {
        _threadReceiveRunning = true;
        bool workDone = false;
        while (_threadReceiveRunning && !workDone)
        {
            //whatevs
            int data = receiveData();
            if (data != -1)
            {
                Debug.Log("Data: " + data);
            }
            workDone = true;
        }
        _threadReceiveRunning = false;
    }

    void ThreadedWorkSend()
    {
        _threadSendRunning = true;
        bool sentWorkDone = false;
        while (_threadSendRunning && !sentWorkDone)
        {
            int returnVal = sendData(sendVal);
            lastSentVal = sendVal;
            sentWorkDone = true;
        }
        _threadSendRunning = false;
    }



    void OnDisable()
    {
        if (_threadReceiveRunning)
        {
            _threadReceiveRunning = false;
            _threadReceive.Join();
        }

        if (_threadSendRunning)
        {
            _threadSendRunning = false;
            _threadSendinIt.Join();
        }

    }

    // Update is called once per frame
    void Update()
    {


        if (!_threadReceiveRunning)
        {
            _threadReceive = new Thread(ThreadedWorkRec);
            _threadReceive.Start();
        }

        // Send a value if it is non-null and hasn't yet been sent 
        if (sendVal != null && !_threadSendRunning && lastSentVal != sendVal)
        {
            _threadSendinIt = new Thread(ThreadedWorkSend);
            _threadSendinIt.Start();
        }
    }


}
