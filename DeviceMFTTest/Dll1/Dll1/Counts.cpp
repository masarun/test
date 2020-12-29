#include "pch.h"
#include "Counts.h"

ULONG Counters::m_cLocks = 0;
ULONG Counters::m_cNumObjects = 0;

ULONG Counters::GetLockCount()
{
    return m_cLocks;
}

ULONG Counters::GetObjectCount()
{
    return m_cNumObjects;
}

void Counters::IncLockCount()
{
    InterlockedIncrement(&m_cLocks);
}

void Counters::DecLockCount()
{
    InterlockedDecrement(&m_cLocks);
}

void Counters::IncObjectCount()
{
    InterlockedIncrement(&m_cNumObjects);
}

void Counters::DecObjectCount()
{
    InterlockedDecrement(&m_cNumObjects);
}
