#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

HANDLE hSlot;
LPTSTR lpszBuffer;
BOOL ReadSlot();
BOOL WINAPI MakeSlot(LPTSTR lpszSlotName);
BOOL WriteSlot(HANDLE hSlot, LPTSTR lpszMessage)