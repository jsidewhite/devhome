

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Mon Jan 18 19:14:07 2038
 */
/* Compiler settings for C:\Users\jwhites\AppData\Local\Temp\DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.idl-7bc965bc:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __DevHome2EQuietBackgroundProcesses2EQuietBackgroundProcessesSessionManager_p_h__
#define __DevHome2EQuietBackgroundProcesses2EQuietBackgroundProcessesSessionManager_p_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef ____x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_FWD_DEFINED__
#define ____x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_FWD_DEFINED__
typedef interface __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager;

#endif 	/* ____x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_FWD_DEFINED__ */


#ifndef ____x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_FWD_DEFINED__
#define ____x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_FWD_DEFINED__
typedef interface __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics;

#endif 	/* ____x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_FWD_DEFINED__ */


/* header files for imported files */
#include "inspectable.h"
#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_DevHome2EQuietBackgroundProcesses2EQuietBackgroundProcessesSessionManager_0000_0000 */
/* [local] */ 






extern RPC_IF_HANDLE __MIDL_itf_DevHome2EQuietBackgroundProcesses2EQuietBackgroundProcessesSessionManager_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DevHome2EQuietBackgroundProcesses2EQuietBackgroundProcessesSessionManager_0000_0000_v0_0_s_ifspec;

#ifndef ____x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_INTERFACE_DEFINED__
#define ____x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_INTERFACE_DEFINED__

/* interface __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager */
/* [object][uuid] */ 


EXTERN_C const IID IID___x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("812326e8-ebbc-5b6b-8a72-8840931e547a")
    __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInt( 
            /* [retval][out] */ int *result) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager * This);
        
        DECLSPEC_XFGVIRT(IInspectable, GetIids)
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        DECLSPEC_XFGVIRT(IInspectable, GetRuntimeClassName)
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager * This,
            /* [out] */ HSTRING *className);
        
        DECLSPEC_XFGVIRT(IInspectable, GetTrustLevel)
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager * This,
            /* [out] */ TrustLevel *trustLevel);
        
        DECLSPEC_XFGVIRT(__x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager, GetInt)
        HRESULT ( STDMETHODCALLTYPE *GetInt )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager * This,
            /* [retval][out] */ int *result);
        
        END_INTERFACE
    } __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerVtbl;

    interface __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager
    {
        CONST_VTBL struct __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_GetInt(This,result)	\
    ( (This)->lpVtbl -> GetInt(This,result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManager_INTERFACE_DEFINED__ */


#ifndef ____x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_INTERFACE_DEFINED__
#define ____x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_INTERFACE_DEFINED__

/* interface __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics */
/* [object][uuid] */ 


EXTERN_C const IID IID___x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e2335bd1-511c-572e-b204-e35b0878a9c7")
    __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSession( 
            /* [retval][out] */ __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession **result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TryGetSession( 
            /* [retval][out] */ __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession **result) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStaticsVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics * This);
        
        DECLSPEC_XFGVIRT(IInspectable, GetIids)
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        DECLSPEC_XFGVIRT(IInspectable, GetRuntimeClassName)
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics * This,
            /* [out] */ HSTRING *className);
        
        DECLSPEC_XFGVIRT(IInspectable, GetTrustLevel)
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics * This,
            /* [out] */ TrustLevel *trustLevel);
        
        DECLSPEC_XFGVIRT(__x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics, GetSession)
        HRESULT ( STDMETHODCALLTYPE *GetSession )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics * This,
            /* [retval][out] */ __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession **result);
        
        DECLSPEC_XFGVIRT(__x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics, TryGetSession)
        HRESULT ( STDMETHODCALLTYPE *TryGetSession )( 
            __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics * This,
            /* [retval][out] */ __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession **result);
        
        END_INTERFACE
    } __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStaticsVtbl;

    interface __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics
    {
        CONST_VTBL struct __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStaticsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_GetSession(This,result)	\
    ( (This)->lpVtbl -> GetSession(This,result) ) 

#define __x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_TryGetSession(This,result)	\
    ( (This)->lpVtbl -> TryGetSession(This,result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_DevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionManagerStatics_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


