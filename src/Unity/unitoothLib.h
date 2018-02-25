#pragma once

#ifdef UNITOOTHLIB_EXPORTS  
#define UNITOOTH_API __declspec(dllexport)   
#else  
#define UNITOOTH_API __declspec(dllimport)   
#endif  

		// Returns a + b
extern "C" UNITOOTH_API int uniConnect(ULONGLONG mac_address);
