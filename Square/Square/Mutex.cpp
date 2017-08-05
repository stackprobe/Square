/*
	mutex
		int h = mutexOpen("ABC");
		waitForMillis(h, 1000);
		waitForever(h);
		mutexRelease(h);
		handleClose(h);

	event
		int h = eventOpen("ABC");
		eventSet(h);
		waitForMillis(h, 1000);
		waitForever(h);
		handleClose(h);
*/

#include "all.h"

int mutexOpen(char *mutexName)
{
	mutexName = xcout("Global\\%s", mutexName);
	HANDLE h = CreateMutexA(NULL, FALSE, mutexName);
	memFree(mutexName);

	errorCase(h == NULL); // ? ���s
	return (int)h;
}
void mutexRelease(int h)
{
	errorCase(!ReleaseMutex((HANDLE)h)); // ? ���s
}

int eventOpen(char *eventName)
{
	HANDLE h = CreateEventA(NULL, FALSE, FALSE, eventName);
	errorCase(h == NULL); // ? ���s
	return (int)h;
}
void eventSet(int h)
{
	errorCase(!SetEvent((HANDLE)h)); // ? ���s
}

int waitForMillis(int h, int millis) // ret: ? ���b�N���� || �V�O�i���擾
{
	return WaitForSingleObject((HANDLE)h, millis) == WAIT_OBJECT_0;
}
void waitForever(int h)
{
	errorCase(!waitForMillis(h, INFINITE));
}
void handleClose(int h)
{
	errorCase(!CloseHandle((HANDLE)h)); // ? ���s
}
