// ADMHost.cpp : Implementation of WinMain


#include "stdafx.h"
// MySQL Preprocessor 
#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>
#include "ClrHost.h"

//class CADMHostModule : public CAtlExeModuleT<CADMHostModule> { };
//CADMHostModule _AtlModule;

/// <summary>
///		Do the work of running the application
/// </summary>
int RunApplication(IUnmanagedHostPtr &pClr)
{
	// Get the default managed host
    IManagedHostPtr pManagedHost = pClr->DefaultManagedHost;

    // create a new AppDomain
    _bstr_t name(L"Second AppDomain");
    DWORD id = pManagedHost->CreateAppDomain(name);

    // get its host, and print to the screen
    IManagedHostPtr pSecondManagedHost = pClr->GetManagedHost(id);
    //pSecondManagedHost->Write(_bstr_t(L"Hello, World."));
	pSecondManagedHost->Run(_bstr_t(L"Threading.exe"));

	return 0;
}



/// <summary>
///     Entry point
/// </summary>
int wmain(int, wchar_t **)
{
    int returnCode = 0;
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    
    try
    {
        // bind the to CLR
        IUnmanagedHostPtr pClr;
        HRESULT hrBind = CClrHost::BindToRuntime(&pClr.GetInterfacePtr());
        if(FAILED(hrBind))
            _com_raise_error(hrBind);

        // start it up
        pClr->Start();

		// run the application
		returnCode = RunApplication(pClr);

        // finally, dispose of the managed hosts and shut down the runtime
        pClr->Stop();
    }
    catch(const _com_error &e)
    {
        const wchar_t *message = (wchar_t *)e.Description() == NULL ?
            L"" :
            (wchar_t *)e.Description();
        std::wcerr << L"Error 0x" << std::hex << e.Error() << L") : " << message << std::endl;
        
        returnCode = e.Error();
    }

    CoUninitialize();
    return returnCode;
}