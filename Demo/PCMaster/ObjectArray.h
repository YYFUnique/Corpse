
#ifndef __IObjectArray_FWD_DEFINED__
#define __IObjectArray_FWD_DEFINED__
typedef interface IObjectArray IObjectArray;
#endif 	/* __IObjectArray_FWD_DEFINED__ */


#ifndef __IObjectCollection_FWD_DEFINED__
#define __IObjectCollection_FWD_DEFINED__
typedef interface IObjectCollection IObjectCollection;
#endif 	/* __IObjectCollection_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IObjectArray_INTERFACE_DEFINED__
#define __IObjectArray_INTERFACE_DEFINED__

	/* interface IObjectArray */
	/* [unique][object][uuid][helpstring] */ 


	EXTERN_C const IID IID_IObjectArray;

#if defined(__cplusplus) && !defined(CINTERFACE)

	MIDL_INTERFACE("92CA9DCD-5622-4bba-A805-5E9F541BD8C9")
IObjectArray : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE GetCount( 
			/* [out] */ __RPC__out UINT *pcObjects) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetAt( 
			/* [in] */ UINT uiIndex,
			/* [in] */ __RPC__in REFIID riid,
			/* [iid_is][out] */ __RPC__deref_out_opt void **ppv) = 0;

	};

#else 	/* C style interface */

	typedef struct IObjectArrayVtbl
	{
		BEGIN_INTERFACE

			HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
			__RPC__in IObjectArray * This,
			/* [in] */ __RPC__in REFIID riid,
			/* [annotation][iid_is][out] */ 
			__RPC__deref_out  void **ppvObject);

			ULONG ( STDMETHODCALLTYPE *AddRef )( 
				__RPC__in IObjectArray * This);

			ULONG ( STDMETHODCALLTYPE *Release )( 
				__RPC__in IObjectArray * This);

			HRESULT ( STDMETHODCALLTYPE *GetCount )( 
				__RPC__in IObjectArray * This,
				/* [out] */ __RPC__out UINT *pcObjects);

			HRESULT ( STDMETHODCALLTYPE *GetAt )( 
				__RPC__in IObjectArray * This,
				/* [in] */ UINT uiIndex,
				/* [in] */ __RPC__in REFIID riid,
				/* [iid_is][out] */ __RPC__deref_out_opt void **ppv);

		END_INTERFACE
	} IObjectArrayVtbl;

	interface IObjectArray
	{
		CONST_VTBL struct IObjectArrayVtbl *lpVtbl;
	};

#ifdef COBJMACROS

#define IObjectArray_QueryInterface(This,riid,ppvObject)	\
	( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IObjectArray_AddRef(This)	\
	( (This)->lpVtbl -> AddRef(This) ) 

#define IObjectArray_Release(This)	\
	( (This)->lpVtbl -> Release(This) ) 


#define IObjectArray_GetCount(This,pcObjects)	\
	( (This)->lpVtbl -> GetCount(This,pcObjects) ) 

#define IObjectArray_GetAt(This,uiIndex,riid,ppv)	\
	( (This)->lpVtbl -> GetAt(This,uiIndex,riid,ppv) ) 

#endif /* COBJMACROS */

#endif 	/* C style interface */

#endif 	/* __IObjectArray_INTERFACE_DEFINED__ */


#ifndef __IObjectCollection_INTERFACE_DEFINED__
#define __IObjectCollection_INTERFACE_DEFINED__

	/* interface IObjectCollection */
	/* [unique][object][uuid] */ 


	EXTERN_C const IID IID_IObjectCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)

	MIDL_INTERFACE("5632b1a4-e38a-400a-928a-d4cd63230295")
IObjectCollection : public IObjectArray
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE AddObject( 
			/* [in] */ __RPC__in_opt IUnknown *punk) = 0;

		virtual HRESULT STDMETHODCALLTYPE AddFromArray( 
			/* [in] */ __RPC__in_opt IObjectArray *poaSource) = 0;

		virtual HRESULT STDMETHODCALLTYPE RemoveObjectAt( 
			/* [in] */ UINT uiIndex) = 0;

		virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;

	};

#else 	/* C style interface */

	typedef struct IObjectCollectionVtbl
	{
		BEGIN_INTERFACE

			HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
			__RPC__in IObjectCollection * This,
			/* [in] */ __RPC__in REFIID riid,
			/* [annotation][iid_is][out] */ 
			__RPC__deref_out  void **ppvObject);

			ULONG ( STDMETHODCALLTYPE *AddRef )( 
				__RPC__in IObjectCollection * This);

			ULONG ( STDMETHODCALLTYPE *Release )( 
				__RPC__in IObjectCollection * This);

			HRESULT ( STDMETHODCALLTYPE *GetCount )( 
				__RPC__in IObjectCollection * This,
				/* [out] */ __RPC__out UINT *pcObjects);

			HRESULT ( STDMETHODCALLTYPE *GetAt )( 
				__RPC__in IObjectCollection * This,
				/* [in] */ UINT uiIndex,
				/* [in] */ __RPC__in REFIID riid,
				/* [iid_is][out] */ __RPC__deref_out_opt void **ppv);

			HRESULT ( STDMETHODCALLTYPE *AddObject )( 
				__RPC__in IObjectCollection * This,
				/* [in] */ __RPC__in_opt IUnknown *punk);

			HRESULT ( STDMETHODCALLTYPE *AddFromArray )( 
				__RPC__in IObjectCollection * This,
				/* [in] */ __RPC__in_opt IObjectArray *poaSource);

			HRESULT ( STDMETHODCALLTYPE *RemoveObjectAt )( 
				__RPC__in IObjectCollection * This,
				/* [in] */ UINT uiIndex);

			HRESULT ( STDMETHODCALLTYPE *Clear )( 
				__RPC__in IObjectCollection * This);

		END_INTERFACE
	} IObjectCollectionVtbl;

	interface IObjectCollection
	{
		CONST_VTBL struct IObjectCollectionVtbl *lpVtbl;
	};



#ifdef COBJMACROS

#define IObjectCollection_QueryInterface(This,riid,ppvObject)	\
	( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IObjectCollection_AddRef(This)	\
	( (This)->lpVtbl -> AddRef(This) ) 

#define IObjectCollection_Release(This)	\
	( (This)->lpVtbl -> Release(This) ) 


#define IObjectCollection_GetCount(This,pcObjects)	\
	( (This)->lpVtbl -> GetCount(This,pcObjects) ) 

#define IObjectCollection_GetAt(This,uiIndex,riid,ppv)	\
	( (This)->lpVtbl -> GetAt(This,uiIndex,riid,ppv) ) 


#define IObjectCollection_AddObject(This,punk)	\
	( (This)->lpVtbl -> AddObject(This,punk) ) 

#define IObjectCollection_AddFromArray(This,poaSource)	\
	( (This)->lpVtbl -> AddFromArray(This,poaSource) ) 

#define IObjectCollection_RemoveObjectAt(This,uiIndex)	\
	( (This)->lpVtbl -> RemoveObjectAt(This,uiIndex) ) 

#define IObjectCollection_Clear(This)	\
	( (This)->lpVtbl -> Clear(This) ) 

#endif /* COBJMACROS */
#endif 	/* C style interface */
#endif 	/* __IObjectCollection_INTERFACE_DEFINED__ */
#ifdef __cplusplus
}
#endif
