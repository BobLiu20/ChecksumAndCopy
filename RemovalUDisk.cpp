
#include "stdafx.h"
#include "RemovalUDisk.h"

#include <windows.h>
#include <windef.h>

typedef  long LONG_PTR;
typedef  unsigned long ULONG_PTR;
typedef  DWORD	DWORD_PTR;

#include "Setupapi.h"
#include "winioctl.h"
#include "cfgmgr32.h"
#include <initguid.h>

#pragma comment(lib,"Cfgmgr32.lib")
#pragma comment(lib,"hid.lib")  
#pragma comment(lib,"setupapi.lib")

DEFINE_GUID(GUID_DEVINTERFACE_DISK,   0x53f56307L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);
DEFINE_GUID(GUID_DEVINTERFACE_CDROM,  0x53f56308L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);

DEVINST GetDrivesDevInstByDiskNumber(long DiskNumber, UINT DriveType) 
{
	GUID* guid;

	switch (DriveType) 
	{
	case DRIVE_REMOVABLE:
		//break;
	case DRIVE_FIXED:
		guid = (GUID*)(void*)&GUID_DEVINTERFACE_DISK;
		break;
	case DRIVE_CDROM:
		guid = (GUID*)(void*)&GUID_DEVINTERFACE_CDROM;
		break;
	default:
		return 0;
	}

	// Get device interface info set handle for all devices attached to system
	HDEVINFO hDevInfo = SetupDiGetClassDevs(guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	// Retrieve a context structure for a device interface of a device
	// information set.
	
	DWORD dwIndex = 0;
	SP_DEVICE_INTERFACE_DATA devInterfaceData = {0};
	devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	BOOL bRet = FALSE;

	BYTE Buf[1024];
	PSP_DEVICE_INTERFACE_DETAIL_DATA pspdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)Buf;
	SP_DEVICE_INTERFACE_DATA         spdid;
	SP_DEVINFO_DATA                  spdd;
	DWORD                            dwSize;

	spdid.cbSize = sizeof(spdid);

	while ( true )
	{
		bRet = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, guid, dwIndex, &devInterfaceData);
		if (!bRet) 
		{
			break;
		}
		SetupDiEnumInterfaceDevice(hDevInfo, NULL, guid, dwIndex, &spdid);
		
		dwSize = 0;
		SetupDiGetDeviceInterfaceDetail(hDevInfo, &spdid, NULL, 0, &dwSize, NULL);
		if ( dwSize!=0 && dwSize<=sizeof(Buf) ) 
		{
			//pspdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
			//if ( pspdidd == NULL ) 
			//{
				//return 0; // damn
			//}
			pspdidd->cbSize = sizeof(*pspdidd); // 5 Bytes!
			ZeroMemory((PVOID)&spdd, sizeof(spdd));
			spdd.cbSize = sizeof(spdd);
			long res = SetupDiGetDeviceInterfaceDetail(hDevInfo, &spdid, pspdidd, dwSize, &dwSize, &spdd);
			if ( res ) 
			{
				// the device instance id string contains the serial number if the 
			    // device has one...
				// char szDevInstId[260] = {0};
				// SetupDiGetDeviceInstanceId(hDevInfo, &spdd, szDevInstId, 260, NULL);
				// printf("DevInstId=%s\n", szDevInstId);
				HANDLE hDrive = CreateFile(pspdidd->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
				if ( hDrive != INVALID_HANDLE_VALUE ) 
				{
					STORAGE_DEVICE_NUMBER sdn;
					DWORD dwBytesReturned = 0;
					res = DeviceIoControl(hDrive, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof(sdn), &dwBytesReturned, NULL);
					if ( res ) 
					{
						if ( DiskNumber == (long)sdn.DeviceNumber ) 
						{
							CloseHandle(hDrive);
							//HeapFree(GetProcessHeap(), 0, pspdidd);
							SetupDiDestroyDeviceInfoList(hDevInfo);
							return spdd.DevInst;
						}
					}
					CloseHandle(hDrive);
				}
			}
			//HeapFree(GetProcessHeap(), 0, pspdidd);
		}
		dwIndex++;
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	
	return 0;

}



int RemovalUDisk(TCHAR* disk)
{
	TCHAR DriveLetter = disk[0];

	DriveLetter &= ~0x20; // uppercase
	toupper(DriveLetter);
	if ( DriveLetter < 'A' || DriveLetter > 'Z' ) 
	{
		return 1;
	}
	
	TCHAR szRootPath[] = _T("X:\\");   // "X:\"
	
	szRootPath[0] = DriveLetter;
	TCHAR szVolumeAccessPath[] = _T("\\\\.\\X:");   // "\\.\X:"
	szVolumeAccessPath[4] = DriveLetter;
	
	long DiskNumber = -1;

	HANDLE hVolume = CreateFile(szVolumeAccessPath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (hVolume == INVALID_HANDLE_VALUE) 
	{
		return 1;
	}

	STORAGE_DEVICE_NUMBER sdn;
	DWORD dwBytesReturned = 0;

	long res = DeviceIoControl(hVolume, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof(sdn), &dwBytesReturned, NULL);
	if ( res ) 
	{
		DiskNumber = sdn.DeviceNumber;
	}
	CloseHandle(hVolume);

	if ( DiskNumber == -1 ) 
	{
		return 1;
	}
	
	UINT DriveType = GetDriveType(szRootPath);
	
	DEVINST DevInst = GetDrivesDevInstByDiskNumber(DiskNumber, DriveType);
	if ( DevInst == 0 ) 
	{
		return 1;
	}
	
	ULONG Status = 0;
	ULONG ProblemNumber = 0;
	PNP_VETO_TYPE VetoType = PNP_VetoTypeUnknown; 
	WCHAR VetoNameW[MAX_PATH];

	bool bSuccess = false;

	res = CM_Get_Parent(&DevInst, DevInst, 0);  // disk's parent, e.g. the USB bridge, the SATA port....
	res = CM_Get_DevNode_Status(&Status, &ProblemNumber, DevInst, 0);

	bool IsRemovable = ((Status & DN_REMOVABLE) != 0);

	for ( long tries=1; tries<=3; tries++ )  // sometimes we need some tries...
	{
		VetoNameW[0] = 0;
		if ( IsRemovable ) 
		{
			//res = CM_Request_Device_EjectW(DevInst, &VetoType, VetoNameW, sizeof(VetoNameW), 0); 
			res = CM_Request_Device_EjectW(DevInst, &VetoType, NULL, 0, 0);  // with MessageBox or 'bubble'
		}
		else 
		{
			res = CM_Query_And_Remove_SubTreeW(DevInst, &VetoType, VetoNameW, sizeof(VetoNameW), 0); // CM_Query_And_Remove_SubTreeA is not implemented under W2K!
		}
		bSuccess = (res==CR_SUCCESS && VetoType==PNP_VetoTypeUnknown);
		if ( bSuccess )  
		{ 
			break;
		}
		else
		{
			Sleep(200); // required to give the next tries a chance!
		}
	}

	if ( bSuccess ) 
	{
		return 0;
	}

	//printf("failed\n");
	//printf("Result=0x%2X\n", res);

	if ( VetoNameW[0] ) 
	{
		//printf("VetoName=%ws)\n\n", VetoNameW);
	}
	return 1;
}


