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
    m_cLocks++;
}

void Counters::DecLockCount()
{
    m_cLocks--;
}

void Counters::IncObjectCount()
{
    m_cNumObjects++;
}

void Counters::DecObjectCount()
{
    m_cNumObjects--;
}
