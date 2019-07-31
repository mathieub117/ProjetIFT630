#include <Windows.h>

void startupProcess(LPCSTR lpApplicationName, LPSTR lpCommandLine)
{
	// additional information
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	CreateProcessA
	(
		lpApplicationName,   // the path
		lpCommandLine,                // Command line
		NULL,                   // Process handle not inheritable
		NULL,                   // Thread handle not inheritable
		FALSE,                  // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,     // Opens file in a separate console
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi           // Pointer to PROCESS_INFORMATION structure
	);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}