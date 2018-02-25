using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public class unitooth : MonoBehaviour {

    public string macAddress;
    public int port;

#if UNITY_STANDALONE_WIN || UNITY_EDITOR

    [DllImport("unitoothLib")]
    public unsafe static extern int uniConnect(ulong mac_address);

#endif

    // Use this for initialization
    void Start () {
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
        } else {
            Debug.LogError("Please make sure to enter MAC Address of target device into script");
        }
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
