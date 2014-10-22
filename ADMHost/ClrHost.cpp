#include "StdAfx.h"
#include "ClrHost.h"

const wchar_t *CClrHost::AppDomainManagerAssembly = L"ManagedHost, Version=1.0.0.0, PublicKeyToken=c0d4fd12d3adc8e5";
const wchar_t *CClrHost::AppDomainManagerType = L"ADMHost.ManagedHost";

/// <summary>
///     Initialize the host
/// </summary>
CClrHost::CClrHost() : m_started(false), m_pClr(NULL), m_pClrControl(NULL)
{
    return;
}

/// <summary>
///     Clean up the host
/// </summary>
CClrHost::~CClrHost()
{
    // free the AppDomainManagers
    for(AppDomainManagerMap::iterator iAdm = m_appDomainManagers.begin(); iAdm != m_appDomainManagers.end(); iAdm++)
        iAdm->second->Release();

    // release the CLR
    if(m_pClrControl != NULL)
        m_pClrControl->Release();
    if(m_pClr != NULL)
        m_pClr->Release();

    return;
}

/// <summary>
///     Finish setting up the CLR host
/// </summary>
HRESULT CClrHost::FinalConstruct()
{
    // load the CLR into the process
    return CorBindToRuntimeEx(  NULL,
                                NULL,
                                0,
                                CLSID_CLRRuntimeHost,
                                IID_ICLRRuntimeHost,
                                reinterpret_cast<LPVOID *>(&m_pClr));
}

/// <summary>
///     Create a host object, and bind to the CLR
/// </summary>
HRESULT CClrHost::BindToRuntime(__deref_in IUnmanagedHost **pHost)
{
    _ASSERTE(pHost != NULL);
    *pHost = NULL;

    CComObject<CClrHost> *pClrHost = NULL;
    HRESULT hrCreate = CComObject<CClrHost>::CreateInstance(&pClrHost);
    
    if(SUCCEEDED(hrCreate))
    {
        pClrHost->AddRef();
        *pHost = static_cast<IUnmanagedHost *>(pClrHost);
    }

    return hrCreate;
}

/// <summary>
///     Get a manager from the host
/// </summary>
STDMETHODIMP CClrHost::GetHostManager(const IID &, __deref_opt_out_opt void **ppvObject)
{
    if(ppvObject == NULL)
        return E_POINTER;
    
    *ppvObject = NULL;
    return E_NOINTERFACE;
}

/// <summary>
///     Register an AppDomain and its AppDomainManager
/// </summary>
/// <param name="dwAppDomainId">ID of the AppDomain being registered</param>
/// <param name="pUnkAppDomainManager">AppDomainManager for the domain</param>
STDMETHODIMP CClrHost::SetAppDomainManager(DWORD dwAppDomainId, __in IUnknown *pUnkAppDomainManager)
{
    // get the managed host interface
    IManagedHost *pAppDomainManager = NULL;
    if(FAILED(pUnkAppDomainManager->QueryInterface(__uuidof(IManagedHost), reinterpret_cast<void **>(&pAppDomainManager))))
    {
        _ASSERTE(!"AppDomainManager does not implement IManagedHost");
        return E_NOINTERFACE;
    }

    // register ourselves as the unmanaged host
    HRESULT hrSetUnmanagedHost = pAppDomainManager->raw_SetUnmanagedHost(static_cast<IUnmanagedHost *>(this));
    if(FAILED(hrSetUnmanagedHost))
        return hrSetUnmanagedHost;

    // save a copy
    m_appDomainManagers[dwAppDomainId] = pAppDomainManager;
    return S_OK;
}

/// <summary>
///     Start the CLR
/// </summary>
STDMETHODIMP CClrHost::raw_Start()
{
    // we should have bound to the runtime, but not yet started it upon entry
    _ASSERTE(m_pClr != NULL);
    _ASSERTE(!m_started);
	if(m_pClr == NULL)
		return E_FAIL;

    // get the CLR control object
    HRESULT hrClrControl = m_pClr->GetCLRControl(&m_pClrControl);
    if(FAILED(hrClrControl))
        return hrClrControl;

    // set ourselves up as the host control
    HRESULT hrHostControl = m_pClr->SetHostControl(static_cast<IHostControl *>(this));
    if(FAILED(hrHostControl))
        return hrHostControl;

    // setup the AppDomainManager
    HRESULT hrSetAdm = m_pClrControl->SetAppDomainManagerType(AppDomainManagerAssembly, AppDomainManagerType);
    if(FAILED(hrSetAdm))
        return hrSetAdm;

	// get the host protection manager
	ICLRHostProtectionManager *pHostProtectionManager = NULL;
	HRESULT hrGetProtectionManager = m_pClrControl->GetCLRManager(
		IID_ICLRHostProtectionManager,
		reinterpret_cast<void **>(&pHostProtectionManager));
	if (FAILED(hrGetProtectionManager))
		return hrGetProtectionManager;

	// setup host proctection
	HRESULT hrHostProtection = pHostProtectionManager->SetProtectedCategories(
		(EApiCategories)(eSynchronization | eSelfAffectingThreading));
	pHostProtectionManager->Release();

	if (FAILED(hrHostProtection))
		return hrHostProtection;

    // finally, start the runtime
    HRESULT hrStart = m_pClr->Start();
    if(FAILED(hrStart))
        return hrStart;

    // mark as started
    m_started = true;
    return S_OK;
}

/// <summary>
///     Stop the CLR
/// </summary>
STDMETHODIMP CClrHost::raw_Stop()
{
    _ASSERTE(m_started);

    // first send a Dispose call to the managed hosts
    for(AppDomainManagerMap::iterator iAdm = m_appDomainManagers.begin(); iAdm != m_appDomainManagers.end(); iAdm++)
        iAdm->second->raw_Dispose();

    // then, shut down the CLR
    return m_pClr->Stop();
}

/// <summary>
///     Get the AppDomainManager for the default AppDomain
/// </summary>
STDMETHODIMP CClrHost::get_DefaultManagedHost(__out IManagedHost **ppHost)
{
    // just get the AppDomainManager for the default AppDomain
    return raw_GetManagedHost(1, ppHost);
}

/// <summary>
///     Get the AppDomainManager for a specific AppDomain
/// </summary>
STDMETHODIMP CClrHost::raw_GetManagedHost(long appDomain, IManagedHost **ppHost)
{
    _ASSERTE(m_started);

    if(ppHost == NULL)
        return E_POINTER;

    // get the AppDomainManager for the specified domain
    AppDomainManagerMap::const_iterator iHost = m_appDomainManagers.find(appDomain);

    // see if we've got a host
    if(iHost == m_appDomainManagers.end())
    {
        *ppHost = NULL;
        return E_NOMANAGEDHOST;
    }
    else
    {
        *ppHost = iHost->second;
        (*ppHost)->AddRef();
        return S_OK;
    }
}