// unitoothLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "unitoothLib.h"
#include <ws2bth.h>
#include <stdio.h>
#include <initguid.h>
#include <winsock2.h>
#include <strsafe.h>
#include <intsafe.h>
#include <cstdlib>

#pragma comment(lib,"WS2_32")

// {B62C4E8D-62CC-404b-BBBF-BF3E3BBB1374}
DEFINE_GUID(g_guidServiceClass, 0xb62c4e8d, 0x62cc, 0x404b, 0xbb, 0xbf, 0xbf, 0x3e, 0x3b, 0xbb, 0x13, 0x74);

#define CXN_TEST_DATA_STRING              (L"~!@#$%^&*()-_=+?<>1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
#define CXN_TRANSFER_DATA_LENGTH          (sizeof(CXN_TEST_DATA_STRING))


#define CXN_BDADDR_STR_LEN                17   // 6 two-digit hex values plus 5 colons
#define CXN_MAX_INQUIRY_RETRY             3
#define CXN_DELAY_NEXT_INQUIRY            15
#define CXN_SUCCESS                       0
#define CXN_ERROR                         1
#define CXN_DEFAULT_LISTEN_BACKLOG        4
#define SCKT_ERROR						3
#define CNCT_ERROR						2
#define PARSE_ERROR						5
#define PARSE_ERROR1					6
#define PARSE_ERROR2					7
#define SEND_ERROR						11


wchar_t g_szRemoteName[BTH_MAX_NAME_SIZE + 1] = { 0 };  // 1 extra for trailing NULL character
wchar_t g_szRemoteAddr[CXN_BDADDR_STR_LEN + 1] = { 0 }; // 1 extra for trailing NULL character
int  g_ulMaxCxnCycles = 1;



//ULONG NameToBthAddr(_In_ const LPWSTR pszRemoteName, _Out_ PSOCKADDR_BTH pRemoteBthAddr);
ULONG RunClientMode(_In_ SOCKADDR_BTH ululRemoteBthAddr, _In_ int iMaxCxnCycles = 1);

	int uniConnect(ULONGLONG mac_address)
	{
		//if (mac_address[0] != '02:0') {
		//	return (int)mac_address[1];
		//}
		
		ULONG       ulRetCode = CXN_SUCCESS;
		WSADATA     WSAData = { 0 };
		SOCKADDR_BTH RemoteBthAddr = { 0 };
		BTH_ADDR bt_addr = { 0 };

		if (ulRetCode != 0) {
			return ulRetCode;
		}

		ulRetCode = WSAStartup(MAKEWORD(2, 2), &WSAData);

		RemoteBthAddr.addressFamily = AF_BTH;
		RemoteBthAddr.btAddr = mac_address;
		RemoteBthAddr.port = 1; // hard coding channel 1 for now
		RemoteBthAddr.serviceClassId = g_guidServiceClass;

		//ulRetCode = RunClientMode(RemoteBthAddr, 1);

		//
		// Open a bluetooth socket using RFCOMM protocol
		//
		SOCKET LocalSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		if (INVALID_SOCKET == LocalSocket) {
			return SCKT_ERROR;
		}
		//
		// Connect the socket (pSocket) to a given remote socket represented by address (pServerAddr)
		//
		if (SOCKET_ERROR == connect(LocalSocket, (struct sockaddr *) &RemoteBthAddr, sizeof(SOCKADDR_BTH))) {
			return CNCT_ERROR;
		}
		//
		// send() call indicates winsock2 to send the given data
		// of a specified length over a given connection.
		//
		char pszData[1] = { 'ayyy' };
		if (SOCKET_ERROR == send(LocalSocket, (char *)pszData, (int)4, 0)) {
			return SEND_ERROR;
		}

		//
		// Close the socket
		//
		if (SOCKET_ERROR == closesocket(LocalSocket)) {
			return CXN_ERROR;
		}

		return (int)ulRetCode;
	}

	//
	// NameToBthAddr converts a bluetooth device name to a bluetooth address,
	// if required by performing inquiry with remote name requests.
	// This function demonstrates device inquiry, with optional LUP flags.
	//
/*	ULONG NameToBthAddr(_In_ const LPWSTR pszRemoteName, _Out_ PSOCKADDR_BTH pRemoteBtAddr)
	{
		INT             iResult = CXN_SUCCESS;
		BOOL            bContinueLookup = FALSE, bRemoteDeviceFound = FALSE;
		ULONG           ulFlags = 0, ulPQSSize = sizeof(WSAQUERYSET);
		HANDLE          hLookup = NULL;
		PWSAQUERYSET    pWSAQuerySet = NULL;

		ZeroMemory(pRemoteBtAddr, sizeof(*pRemoteBtAddr));

		pWSAQuerySet = (PWSAQUERYSET)HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			ulPQSSize);
		if (NULL == pWSAQuerySet) {
			iResult = STATUS_NO_MEMORY;
			wprintf(L"!ERROR! | Unable to allocate memory for WSAQUERYSET\n");
		}

		//
		// Search for the device with the correct name
		//
		if (CXN_SUCCESS == iResult) {

			for (INT iRetryCount = 0;
				!bRemoteDeviceFound && (iRetryCount < CXN_MAX_INQUIRY_RETRY);
				iRetryCount++) {
				//
				// WSALookupService is used for both service search and device inquiry
				// LUP_CONTAINERS is the flag which signals that we're doing a device inquiry.
				//
				ulFlags = LUP_CONTAINERS;

				//
				// Friendly device name (if available) will be returned in lpszServiceInstanceName
				//
				ulFlags |= LUP_RETURN_NAME;

				//
				// BTH_ADDR will be returned in lpcsaBuffer member of WSAQUERYSET
				//
				ulFlags |= LUP_RETURN_ADDR;

				if (0 == iRetryCount) {
					wprintf(L"*INFO* | Inquiring device from cache...\n");
				}
				else {
					//
					// Flush the device cache for all inquiries, except for the first inquiry
					//
					// By setting LUP_FLUSHCACHE flag, we're asking the lookup service to do
					// a fresh lookup instead of pulling the information from device cache.
					//
					ulFlags |= LUP_FLUSHCACHE;

					//
					// Pause for some time before all the inquiries after the first inquiry
					//
					// Remote Name requests will arrive after device inquiry has
					// completed.  Without a window to receive IN_RANGE notifications,
					// we don't have a direct mechanism to determine when remote
					// name requests have completed.
					//
					wprintf(L"*INFO* | Unable to find device.  Waiting for %d seconds before re-inquiry...\n", CXN_DELAY_NEXT_INQUIRY);
					Sleep(CXN_DELAY_NEXT_INQUIRY * 1000);

					wprintf(L"*INFO* | Inquiring device ...\n");
				}

				//
				// Start the lookup service
				//
				iResult = CXN_SUCCESS;
				hLookup = 0;
				bContinueLookup = FALSE;
				ZeroMemory(pWSAQuerySet, ulPQSSize);
				pWSAQuerySet->dwNameSpace = NS_BTH;
				pWSAQuerySet->dwSize = sizeof(WSAQUERYSET);
				iResult = WSALookupServiceBegin(pWSAQuerySet, ulFlags, &hLookup);

				//
				// Even if we have an error, we want to continue until we
				// reach the CXN_MAX_INQUIRY_RETRY
				//
				if ((NO_ERROR == iResult) && (NULL != hLookup)) {
					bContinueLookup = TRUE;
				}
				else if (0 < iRetryCount) {
					wprintf(L"=CRITICAL= | WSALookupServiceBegin() failed with error code %d, WSAGetLastError = %d\n", iResult, WSAGetLastError());
					break;
				}

				while (bContinueLookup) {
					//
					// Get information about next bluetooth device
					//
					// Note you may pass the same WSAQUERYSET from LookupBegin
					// as long as you don't need to modify any of the pointer
					// members of the structure, etc.
					//
					// ZeroMemory(pWSAQuerySet, ulPQSSize);
					// pWSAQuerySet->dwNameSpace = NS_BTH;
					// pWSAQuerySet->dwSize = sizeof(WSAQUERYSET);
					if (NO_ERROR == WSALookupServiceNext(hLookup,
						ulFlags,
						&ulPQSSize,
						pWSAQuerySet)) {

						//
						// Compare the name to see if this is the device we are looking for.
						//
						if ((pWSAQuerySet->lpszServiceInstanceName != NULL) &&
							(CXN_SUCCESS == _wcsicmp(pWSAQuerySet->lpszServiceInstanceName, pszRemoteName))) {
							//
							// Found a remote bluetooth device with matching name.
							// Get the address of the device and exit the lookup.
							//
							CopyMemory(pRemoteBtAddr,
								(PSOCKADDR_BTH)pWSAQuerySet->lpcsaBuffer->RemoteAddr.lpSockaddr,
								sizeof(*pRemoteBtAddr));
							bRemoteDeviceFound = TRUE;
							//return pRemoteBtAddr->btAddr;
							bContinueLookup = FALSE;
						}
					}
					else {
						iResult = WSAGetLastError();
						if (WSA_E_NO_MORE == iResult) { //No more data
														//
														// No more devices found.  Exit the lookup.
														//
							bContinueLookup = FALSE;
						}
						else if (WSAEFAULT == iResult) {
							//
							// The buffer for QUERYSET was insufficient.
							// In such case 3rd parameter "ulPQSSize" of function "WSALookupServiceNext()" receives
							// the required size.  So we can use this parameter to reallocate memory for QUERYSET.
							//
							HeapFree(GetProcessHeap(), 0, pWSAQuerySet);
							pWSAQuerySet = (PWSAQUERYSET)HeapAlloc(GetProcessHeap(),
								HEAP_ZERO_MEMORY,
								ulPQSSize);
							if (NULL == pWSAQuerySet) {
								wprintf(L"!ERROR! | Unable to allocate memory for WSAQERYSET\n");
								iResult = STATUS_NO_MEMORY;
								bContinueLookup = FALSE;
							}
						}
						else {
							wprintf(L"=CRITICAL= | WSALookupServiceNext() failed with error code %d\n", iResult);
							bContinueLookup = FALSE;
						}
					}
				}

				//
				// End the lookup service
				//
				WSALookupServiceEnd(hLookup);

				if (STATUS_NO_MEMORY == iResult) {
					break;
				}
			}
		}

		if (NULL != pWSAQuerySet) {
			HeapFree(GetProcessHeap(), 0, pWSAQuerySet);
			pWSAQuerySet = NULL;
		}

		if (bRemoteDeviceFound) {
			iResult = CXN_SUCCESS;
		}
		else {
			iResult = CXN_ERROR;
		}

		return iResult;
	} */

	//
	// RunClientMode runs the application in client mode.  It opens a socket, connects it to a
	// remote socket, transfer some data over the connection and closes the connection.
	//
	ULONG RunClientMode(_In_ SOCKADDR_BTH RemoteAddr, _In_ int iMaxCxnCycles)
	{
		ULONG           ulRetCode = CXN_SUCCESS;
		int             iCxnCount = 0;
		wchar_t         *pszData = NULL;
		SOCKET          LocalSocket = INVALID_SOCKET;
		SOCKADDR_BTH    SockAddrBthServer = RemoteAddr;
		HRESULT         res;

		pszData = (wchar_t *)HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			CXN_TRANSFER_DATA_LENGTH);
		if (NULL == pszData) {
			ulRetCode = STATUS_NO_MEMORY;
			wprintf(L"=CRITICAL= | HeapAlloc failed | out of memory, gle = [%d] \n", GetLastError());
		}

		if (CXN_SUCCESS == ulRetCode) {
			//
			// Setting address family to AF_BTH indicates winsock2 to use Bluetooth sockets
			// Port should be set to 0 if ServiceClassId is spesified.
			//
			SockAddrBthServer.addressFamily = AF_BTH;
			SockAddrBthServer.serviceClassId = g_guidServiceClass;
			SockAddrBthServer.port = 1;



			//
			// Create a static data-string, which will be transferred to the remote
			// Bluetooth device
			//
			res = StringCbCopyN(pszData, CXN_TRANSFER_DATA_LENGTH, CXN_TEST_DATA_STRING, CXN_TRANSFER_DATA_LENGTH);
			if (FAILED(res)) {
				wprintf(L"=CRITICAL= | Creating a static data string failed\n");
				ulRetCode = CXN_ERROR;
			}

		}

		if (CXN_SUCCESS == ulRetCode) {

			pszData[(CXN_TRANSFER_DATA_LENGTH / sizeof(wchar_t)) - 1] = 0;

			//
			// Run the connection/data-transfer for user specified number of cycles
			//
			for (iCxnCount = 0;
				(0 == ulRetCode) && (iCxnCount < iMaxCxnCycles || iMaxCxnCycles == 0);
				iCxnCount++) {

				wprintf(L"\n");

				//
				// Open a bluetooth socket using RFCOMM protocol
				//
				LocalSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
				if (INVALID_SOCKET == LocalSocket) {
					wprintf(L"=CRITICAL= | socket() call failed. WSAGetLastError = [%d]\n", WSAGetLastError());
					return SCKT_ERROR;
					break;
				}

				//
				// Connect the socket (pSocket) to a given remote socket represented by address (pServerAddr)
				//
				printf("Test: %zu\n", sizeof(SOCKADDR_BTH));
				if (SOCKET_ERROR == connect(LocalSocket,
					(struct sockaddr *) &SockAddrBthServer,
					sizeof(SOCKADDR_BTH))) {
					wprintf(L"=CRITICAL= | connect() call failed. WSAGetLastError=[%d]\n", WSAGetLastError());
					return SockAddrBthServer.port;
					break;
				}

				//
				// send() call indicates winsock2 to send the given data
				// of a specified length over a given connection.
				//
				wprintf(L"*INFO* | Sending following data string:\n%s\n", pszData);
				if (SOCKET_ERROR == send(LocalSocket,
					(char *)pszData,
					(int)CXN_TRANSFER_DATA_LENGTH,
					0)) {
					wprintf(L"=CRITICAL= | send() call failed w/socket = [0x%I64X], szData = [%p], dataLen = [%I64u]. WSAGetLastError=[%d]\n", (ULONG64)LocalSocket, pszData, (ULONG64)CXN_TRANSFER_DATA_LENGTH, WSAGetLastError());
					ulRetCode = CXN_ERROR;
					break;
				}

				//
				// Close the socket
				//
				if (SOCKET_ERROR == closesocket(LocalSocket)) {
					wprintf(L"=CRITICAL= | closesocket() call failed w/socket = [0x%I64X]. WSAGetLastError=[%d]\n", (ULONG64)LocalSocket, WSAGetLastError());
					ulRetCode = CXN_ERROR;
					break;
				}

				LocalSocket = INVALID_SOCKET;

			}
		}

		if (INVALID_SOCKET != LocalSocket) {
			closesocket(LocalSocket);
			LocalSocket = INVALID_SOCKET;
		}

		if (NULL != pszData) {
			HeapFree(GetProcessHeap(), 0, pszData);
			pszData = NULL;
		}

		return(ulRetCode);
	}
