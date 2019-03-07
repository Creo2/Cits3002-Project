//=============================================================================
//Project:		QuestionServer
//File:			UtilityFunction
//Author:		Rhys Collier (22222918)
//CoAuthor:		Alexander Slate (21498906)
//Created:		12/05/2018
//Description: 	
//
//Modified on:	25/05/2018
//Modified by:	Rhys
//=============================================================================

#include "BaseHeader.h"

void RemoveTabs(std::string* text) {

	for(int i = 0; i < text->length(); i++) {
		if (text->at(i) == '\t') {
			text->erase(i, 1);
			i--;
		}
	}

	return;
}


void AddForwardSlash(std::string* text) {

	for (int i = 0; i < text->length(); i++) {
		char insert[4];
		
		insert[0] = 92;
		insert[1] = 'n';
		insert[2] = 92;
		insert[3] = '\0';

		if (text->at(i) == '\n') {
			text->insert(i, insert);
			i+= 3;
		}
	}

	return;
}

std::vector<std::string> getXMLsInDirectory(std::string dir) {
	WIN32_FIND_DATA ffd;
	TCHAR cCharDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.
	if (dir.length() > (MAX_PATH - 3)) {
		DisplayErrorBox(TEXT("Error: Directory path is too long.\n"));
		return std::vector<std::string>();
	}

	StringCchCopy(cCharDir, MAX_PATH, dir.c_str());
	StringCchCat(cCharDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.

	hFind = FindFirstFile(cCharDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) {
		DisplayErrorBox(TEXT("FindFirstFile"));
		return std::vector<std::string>();
	}

	std::vector<std::string> files = {};
	
	do {
		if (GetLastError() != ERROR_NO_MORE_FILES) {
			if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0) {
				files.push_back(cCharDir);
				files[files.size() - 1].pop_back();
				files[files.size() - 1].append(ffd.cFileName);
				printf("%s\n", files[files.size() - 1].c_str());
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);


	FindClose(hFind);
	return files;
}

void ErrorExit(PTSTR lpszFunction)

// Format a readable error message, display a message box, 
// and exit from the application.
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(1);
}

void DisplayErrorBox(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and clean up

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

bool CreateAndWriteFile(const char* fileName, std::string* fileInput) {
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwBytesWritten = 0;
	BOOL fSuccess = FALSE;
	
	//create c file to store code
	hFile = CreateFile(fileName, // file name 
		GENERIC_WRITE,        // open for write 
		0,                    // do not share 
		NULL,                 // default security 
		CREATE_ALWAYS,        // overwrite existing
		FILE_ATTRIBUTE_NORMAL,// normal file 
		NULL);                // no template 
	if (hFile == INVALID_HANDLE_VALUE) {
		DisplayErrorBox(TEXT("code file CreateFile failed\n"));
		return false;
	}

	//write to the c file
	fSuccess = WriteFile(hFile, //handle
		fileInput->c_str(), //what to write
		fileInput->size(), //how much to write
		&dwBytesWritten, //how much is written
		(DWORD)NULL);
	if (!fSuccess || dwBytesWritten != fileInput->size()) {
		DisplayErrorBox(TEXT("code file WriteFile failed\n"));
		if (!CloseHandle(hFile)) {
			DisplayErrorBox(TEXT("CloseHandle() failed\n"));
			return false;
		}
		return false;
	}

	if (!CloseHandle(hFile)) {
		DisplayErrorBox(TEXT("code file CloseHandle() failed\n"));
		return false;
	}

	return true;
}

bool ReadFile(const char* fileName, std::string* Output) {
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwBytesRead = 0;
	char  chBuffer[BUFSIZ];
	
	//  Opens the existing file. 
	hFile = CreateFile(fileName,// file name 
		GENERIC_READ,          // open for reading 
		0,                     // do not share 
		NULL,                  // default security 
		OPEN_EXISTING,         // existing file only 
		FILE_ATTRIBUTE_NORMAL, // normal file 
		NULL);                 // no template 
	if (hFile == INVALID_HANDLE_VALUE) {
		DisplayErrorBox(TEXT("Could not open file\n"));
		return false;
	}

	do {
		if (ReadFile(hFile, chBuffer, BUFSIZ, &dwBytesRead, NULL)) {
			Output->append(chBuffer);
		} else {
			DisplayErrorBox(TEXT("ReadFile failed\n"));
			return false;
		}
		//  Continues until the whole file is processed.
	} while (dwBytesRead == BUFSIZ);

	return true;
}