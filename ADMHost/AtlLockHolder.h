#pragma once

/// <summary>
///     Utility class to lock/unlock an ATL class
/// </summary>
template<class T>
class CAtlLockHolder
{
private:
    T       *m_pAtlClass;
    bool    m_locked;

public:
    CAtlLockHolder(T *pAtlClass) : m_pAtlClass(pAtlClass), m_locked(true)
    {
        _ASSERTE(pAtlClass != NULL);
        pAtlClass->Lock();
    }

    ~CAtlLockHolder()
    {
        Unlock();
    }
    
    void Unlock()
    {
        if(m_locked)
        {
            m_pAtlClass->Unlock();
            m_locked = false;
        }

        return;
    }
};