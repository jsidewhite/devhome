

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0626 */
/* at Mon Jan 18 19:14:07 2038
 */
/* Compiler settings for C:\Users\jwhites\AppData\Local\Temp\DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.idl-7dc774c8:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0626 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
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

#ifndef __DevHome2EQuietBackgroundProcesses2EQuietBackgroundProcessesSession_h_p_h__
#define __DevHome2EQuietBackgroundProcesses2EQuietBackgroundProcessesSession_h_p_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if _CONTROL_FLOW_GUARD_XFG
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

#if defined(__cplusplus)
#if defined(__MIDL_USE_C_ENUM)
#define MIDL_ENUM enum
#else
#define MIDL_ENUM enum class
#endif
#endif


/* Forward Declarations */ 

#ifndef ____x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_FWD_DEFINED__
#define ____x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_FWD_DEFINED__
typedef interface __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession;

#endif 	/* ____x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_FWD_DEFINED__ */


#ifndef ____x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_FWD_DEFINED__
#define ____x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_FWD_DEFINED__
typedef interface __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics;

#endif 	/* ____x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_FWD_DEFINED__ */


/* header files for imported files */
#include "inspectable.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_DevHome2EQuietBackgroundProcesses2EQuietBackgroundProcessesSession_0000_0000 */
/* [local] */ 






extern RPC_IF_HANDLE __MIDL_itf_DevHome2EQuietBackgroundProcesses2EQuietBackgroundProcessesSession_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DevHome2EQuietBackgroundProcesses2EQuietBackgroundProcessesSession_0000_0000_v0_0_s_ifspec;

#ifndef ____x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_INTERFACE_DEFINED__
#define ____x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_INTERFACE_DEFINED__

/* interface __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession */
/* [object][uuid] */ 


EXTERN_C const IID IID___x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b1325e0e-5df5-5309-980c-17f571eac35d")
    __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Start( 
            /* [retval][out] */ __int64 *result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsActive( 
            /* [retval][out] */ boolean *value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_TimeLeftInSeconds( 
            /* [retval][out] */ __int64 *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession * This);
        
        DECLSPEC_XFGVIRT(IInspectable, GetIids)
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        DECLSPEC_XFGVIRT(IInspectable, GetRuntimeClassName)
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession * This,
            /* [out] */ HSTRING *className);
        
        DECLSPEC_XFGVIRT(IInspectable, GetTrustLevel)
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession * This,
            /* [out] */ TrustLevel *trustLevel);
        
        DECLSPEC_XFGVIRT(__x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession, Start)
        HRESULT ( STDMETHODCALLTYPE *Start )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession * This,
            /* [retval][out] */ __int64 *result);
        
        DECLSPEC_XFGVIRT(__x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession, Stop)
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession * This);
        
        DECLSPEC_XFGVIRT(__x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession, get_IsActive)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsActive )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession * This,
            /* [retval][out] */ boolean *value);
        
        DECLSPEC_XFGVIRT(__x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession, get_TimeLeftInSeconds)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_TimeLeftInSeconds )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession * This,
            /* [retval][out] */ __int64 *value);
        
        END_INTERFACE
    } __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionVtbl;

    interface __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession
    {
        CONST_VTBL struct __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_Start(This,result)	\
    ( (This)->lpVtbl -> Start(This,result) ) 

#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_Stop(This)	\
    ( (This)->lpVtbl -> Stop(This) ) 

#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_get_IsActive(This,value)	\
    ( (This)->lpVtbl -> get_IsActive(This,value) ) 

#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_get_TimeLeftInSeconds(This,value)	\
    ( (This)->lpVtbl -> get_TimeLeftInSeconds(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession_INTERFACE_DEFINED__ */


#ifndef ____x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_INTERFACE_DEFINED__
#define ____x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_INTERFACE_DEFINED__

/* interface __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics */
/* [object][uuid] */ 


EXTERN_C const IID IID___x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("78c554cf-1450-5e28-97f2-04e02c524635")
    __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSingleton( 
            /* [retval][out] */ __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession **result) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStaticsVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics * This);
        
        DECLSPEC_XFGVIRT(IInspectable, GetIids)
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        DECLSPEC_XFGVIRT(IInspectable, GetRuntimeClassName)
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics * This,
            /* [out] */ HSTRING *className);
        
        DECLSPEC_XFGVIRT(IInspectable, GetTrustLevel)
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics * This,
            /* [out] */ TrustLevel *trustLevel);
        
        DECLSPEC_XFGVIRT(__x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics, GetSingleton)
        HRESULT ( STDMETHODCALLTYPE *GetSingleton )( 
            __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics * This,
            /* [retval][out] */ __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSession **result);
        
        END_INTERFACE
    } __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStaticsVtbl;

    interface __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics
    {
        CONST_VTBL struct __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStaticsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_GetSingleton(This,result)	\
    ( (This)->lpVtbl -> GetSingleton(This,result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CDevHome_CQuietBackgroundProcesses_CIQuietBackgroundProcessesSessionStatics_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


