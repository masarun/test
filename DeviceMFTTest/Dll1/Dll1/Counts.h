#pragma once
#include <Windows.h>

class Counters
{
public:
	static ULONG GetLockCount();
	static ULONG GetObjectCount();

	static void IncLockCount();
	static void DecLockCount();
	static void IncObjectCount();
	static void DecObjectCount();

private:
	static ULONG m_cLocks;
	static ULONG m_cNumObjects;

};