#pragma once
#include <iostream> 
#include <string>
#include <string.h>
#include <winsock2.h> // include must before window.h
#include <iphlpapi.h>
#include <windows.h>  
#include <DXGI.h>  
#include <vector>  
#include <intrin.h>
#include <tchar.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "DXGI.lib")

#pragma warning(disable: 4996) // avoid GetVersionEx to be warned


#define _WIN32_DCOM
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

// ***** global macros ***** //
static const int kMaxInfoBuffer = 256;
#define  GBYTES  1073741824  
#define  MBYTES  1048576  
#define  KBYTES  1024  
#define  DKBYTES 1024.0  

#include<sstream>
#include<iomanip>
string convertDoubleToString(const long double value, const int precision = 0)
{
	stringstream stream{};
	stream << fixed << setprecision(precision) << value;
	return stream.str();
}

string getCpuType()
{
	int cpuInfo[4] = { -1 };
	char cpu_manufacture[32] = { 0 };
	char cpu_type[32] = { 0 };
	char cpu_freq[32] = { 0 };

	__cpuid(cpuInfo, 0x80000002);
	memcpy(cpu_manufacture, cpuInfo, sizeof(cpuInfo));

	__cpuid(cpuInfo, 0x80000003);
	memcpy(cpu_type, cpuInfo, sizeof(cpuInfo));

	string str = string(cpu_manufacture);
	return str;
}

string getMemorySize()
{
	std::string memorySize;
	MEMORYSTATUSEX statusex;
	statusex.dwLength = sizeof(statusex);
	if (GlobalMemoryStatusEx(&statusex))
	{
		unsigned long long total = 0, remain_total = 0, avl = 0, remain_avl = 0;
		double decimal_total = 0, decimal_avl = 0;
		remain_total = statusex.ullTotalPhys % GBYTES;
		total = statusex.ullTotalPhys / GBYTES;
		avl = statusex.ullAvailPhys / GBYTES;
		remain_avl = statusex.ullAvailPhys % GBYTES;
		if (remain_total > 0)
			decimal_total = (remain_total / MBYTES) / DKBYTES;
		if (remain_avl > 0)
			decimal_avl = (remain_avl / MBYTES) / DKBYTES;

		decimal_total += (double)total;
		decimal_avl += (double)avl;
		char  buffer[kMaxInfoBuffer];
		sprintf_s(buffer, kMaxInfoBuffer, "total %.2f GB (%.2f GB available)", decimal_total, decimal_avl);
		memorySize.append(buffer);
	}
	return memorySize;
}

string execCmd(const char* cmd)
{
	char buffer[128] = { 0 };
	std::string result;
	FILE* pipe = _popen(cmd, "r");
	if (!pipe) throw std::runtime_error("_popen() failed!");
	while (!feof(pipe))
	{
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	_pclose(pipe);

	return result;
}

string getHardDiskSerial()
{
	string hd_seiral = execCmd("wmic path win32_physicalmedia get SerialNumber");
	return hd_seiral.substr(44, 40);
}

string getMemoryUseR()
{
	std::string memorySize;
	MEMORYSTATUSEX statusex;
	statusex.dwLength = sizeof(statusex);
	double decimal_total = 0, decimal_avl = 0;
	if (GlobalMemoryStatusEx(&statusex))
	{
		unsigned long long total = 0, remain_total = 0, avl = 0, remain_avl = 0;
		remain_total = statusex.ullTotalPhys % GBYTES;
		total = statusex.ullTotalPhys / GBYTES;
		avl = statusex.ullAvailPhys / GBYTES;
		remain_avl = statusex.ullAvailPhys % GBYTES;
		if (remain_total > 0)
			decimal_total = (remain_total / MBYTES) / DKBYTES;
		if (remain_avl > 0)
			decimal_avl = (remain_avl / MBYTES) / DKBYTES;

		decimal_total += (double)total;
		decimal_avl += (double)avl;
		char  buffer[kMaxInfoBuffer];
		sprintf_s(buffer, kMaxInfoBuffer, "total %.2f GB (%.2f GB available)", decimal_total, decimal_avl);
		memorySize.append(buffer);
	}
	string str = convertDoubleToString((1 - decimal_avl / decimal_total) * 100, 3);
	return str + string("%");
}

string getDiskSpace()
{
	double all_Total = 0;
	DWORD dwSize = MAX_PATH;
	TCHAR szLogicalDrives[MAX_PATH] = { 0 };

	DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);
	if (dwResult > 0 && dwResult <= MAX_PATH)
	{
		TCHAR* szSingleDrive = szLogicalDrives;
		while (*szSingleDrive)
		{
			uint64_t available, total, free;
			if (GetDiskFreeSpaceEx(szSingleDrive, (ULARGE_INTEGER*)&available, (ULARGE_INTEGER*)&total, (ULARGE_INTEGER*)&free))
			{
				all_Total += total;
			}

			// 获取下一个驱动器号起始地址
			szSingleDrive += _tcslen(szSingleDrive) + 1;
		}
	}
	return convertDoubleToString(all_Total / 1024 / 1024 / 1024, 3) + "GB";
}

string getOsInfo()
{
	OSVERSIONINFO osver = { sizeof(OSVERSIONINFO) };
	GetVersionEx(&osver);
	std::string os_name;
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0)
		os_name = "Windows 2000";
	else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)
		os_name = "Windows XP";
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0)
		os_name = "Windows 2003";
	else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2)
		os_name = "windows vista";
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1)
		os_name = "windows 7";
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2)
		os_name = "windows 10";

	return os_name;
}

string getDriver() {

	HRESULT hres;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		cout << "Failed to initialize COM library. Error code = 0x"
			<< hex << hres << endl;
		return string("null");
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------

	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);


	if (FAILED(hres))
	{
		cout << "Failed to initialize security. Error code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return string("null");                    // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator* pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&pLoc);

	if (FAILED(hres))
	{
		cout << "Failed to create IWbemLocator object."
			<< " Err code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return string("null");                 // Program has failed.
	}

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	IWbemServices* pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);

	if (FAILED(hres))
	{
		cout << "Could not connect. Error code = 0x"
			<< hex << hres << endl;
		pLoc->Release();
		CoUninitialize();
		return string("null");                // Program has failed.
	}

	//cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		cout << "Could not set proxy blanket. Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return string("null");
	}

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----

	// For example, get the name of the operating system
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_VideoController"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		cout << "Query for operating system name failed."
			<< " Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return string("null");               // Program has failed.
	}

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
		&pclsObj, &uReturn);
	if (0 == uReturn)
	{
		return string("null");
	}

	VARIANT vtProp;

	// Get the value of the Name property
	hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);

	VariantClear(&vtProp);

	pclsObj->Release();

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return _com_util::ConvertBSTRToString(vtProp.bstrVal);
}

string getNetworkInfo()
{
	// PIP_ADAPTER_INFO struct contains network information
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long adapter_size = sizeof(IP_ADAPTER_INFO);
	int ret = GetAdaptersInfo(pIpAdapterInfo, &adapter_size);

	if (ret == ERROR_BUFFER_OVERFLOW)
	{
		// overflow, use the output size to recreate the handler
		delete pIpAdapterInfo;
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[adapter_size];
		ret = GetAdaptersInfo(pIpAdapterInfo, &adapter_size);
	}

	if (ret == ERROR_SUCCESS)
	{
		int card_index = 0;

		// may have many cards, it saved in linklist
		if (pIpAdapterInfo) return string(pIpAdapterInfo->AdapterName);
		else return string("null");
	}
}

string getScreen()
{
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	return to_string(cx) + string("*") + to_string(cy);
}