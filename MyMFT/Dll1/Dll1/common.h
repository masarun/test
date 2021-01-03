#pragma once
#include <Windows.h>

class CCritSec
{
private:
    CRITICAL_SECTION m_criticalSection;
public:
    CCritSec() { InitializeCriticalSection(&m_criticalSection); }
    ~CCritSec() { DeleteCriticalSection(&m_criticalSection); }
    _Requires_lock_not_held_(m_criticalSection) _Acquires_lock_(m_criticalSection)
        void Lock() { EnterCriticalSection(&m_criticalSection); }
    _Requires_lock_held_(m_criticalSection) _Releases_lock_(m_criticalSection)
        void Unlock() { LeaveCriticalSection(&m_criticalSection); }
};

class CAutoLock
{
protected:
    CCritSec* m_pCriticalSection;
public:
    _Acquires_lock_(this->m_pCriticalSection->m_criticalSection)
        CAutoLock(CCritSec& crit) 
    {
        m_pCriticalSection = &crit;
        m_pCriticalSection->Lock();
    }

    _Acquires_lock_(this->m_pCriticalSection->m_criticalSection)
        CAutoLock(CCritSec* crit)
    {
        m_pCriticalSection = crit;
        m_pCriticalSection->Lock();
    }

    _Releases_lock_(this->m_pCriticalSection->m_criticalSection)
        ~CAutoLock() { m_pCriticalSection->Unlock(); }
};
