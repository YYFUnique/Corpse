
#ifndef __ICustomDestinationList_INTERFACE_DEFINED__
#define __ICustomDestinationList_INTERFACE_DEFINED__

/* interface ICustomDestinationList */
/* [unique][object][uuid] */ 

typedef /* [v1_enum] */ 
enum KNOWNDESTCATEGORY
{	KDC_FREQUENT	= 1,
KDC_RECENT	= ( KDC_FREQUENT + 1 ) 
} 	KNOWNDESTCATEGORY;

EXTERN_C const CLSID CLSID_DestinationList;

#ifdef __cplusplus
class DECLSPEC_UUID("77f10cf0-3db5-4966-b520-b7c54fd35ed6")
	DestinationList;
#endif

EXTERN_C const CLSID CLSID_EnumerableObjectCollection;

#ifdef __cplusplus
class DECLSPEC_UUID("2d3468c1-36a7-43b6-ac24-d3f02fd9607a")
	EnumerableObjectCollection;
#endif

EXTERN_C const IID IID_ICustomDestinationList;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("6332debf-87b5-4670-90c0-5e57b408a49e")
ICustomDestinationList : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE SetAppID( 
		/* [string][in] */ __RPC__in_string LPCWSTR pszAppID) = 0;

	virtual HRESULT STDMETHODCALLTYPE BeginList( 
		/* [out] */ __RPC__out UINT *pcMinSlots,
		/* [in] */ __RPC__in REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out_opt void **ppv) = 0;

	virtual HRESULT STDMETHODCALLTYPE AppendCategory( 
		/* [string][in] */ __RPC__in_string LPCWSTR pszCategory,
		/* [in] */ __RPC__in_opt IObjectArray *poa) = 0;

	virtual HRESULT STDMETHODCALLTYPE AppendKnownCategory( 
		/* [in] */ KNOWNDESTCATEGORY category) = 0;

	virtual HRESULT STDMETHODCALLTYPE AddUserTasks( 
		/* [in] */ __RPC__in_opt IObjectArray *poa) = 0;

	virtual HRESULT STDMETHODCALLTYPE CommitList( void) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetRemovedDestinations( 
		/* [in] */ __RPC__in REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out_opt void **ppv) = 0;

	virtual HRESULT STDMETHODCALLTYPE DeleteList( 
		/* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszAppID) = 0;

	virtual HRESULT STDMETHODCALLTYPE AbortList( void) = 0;

};

#else 	/* C style interface */

typedef struct ICustomDestinationListVtbl
{
	BEGIN_INTERFACE

		HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
		__RPC__in ICustomDestinationList * This,
		/* [in] */ __RPC__in REFIID riid,
		/* [annotation][iid_is][out] */ 
		__RPC__deref_out  void **ppvObject);

		ULONG ( STDMETHODCALLTYPE *AddRef )( 
			__RPC__in ICustomDestinationList * This);

		ULONG ( STDMETHODCALLTYPE *Release )( 
			__RPC__in ICustomDestinationList * This);

		HRESULT ( STDMETHODCALLTYPE *SetAppID )( 
			__RPC__in ICustomDestinationList * This,
			/* [string][in] */ __RPC__in_string LPCWSTR pszAppID);

		HRESULT ( STDMETHODCALLTYPE *BeginList )( 
			__RPC__in ICustomDestinationList * This,
			/* [out] */ __RPC__out UINT *pcMinSlots,
			/* [in] */ __RPC__in REFIID riid,
			/* [iid_is][out] */ __RPC__deref_out_opt void **ppv);

		HRESULT ( STDMETHODCALLTYPE *AppendCategory )( 
			__RPC__in ICustomDestinationList * This,
			/* [string][in] */ __RPC__in_string LPCWSTR pszCategory,
			/* [in] */ __RPC__in_opt IObjectArray *poa);

		HRESULT ( STDMETHODCALLTYPE *AppendKnownCategory )( 
			__RPC__in ICustomDestinationList * This,
			/* [in] */ KNOWNDESTCATEGORY category);

		HRESULT ( STDMETHODCALLTYPE *AddUserTasks )( 
			__RPC__in ICustomDestinationList * This,
			/* [in] */ __RPC__in_opt IObjectArray *poa);

		HRESULT ( STDMETHODCALLTYPE *CommitList )( 
			__RPC__in ICustomDestinationList * This);

		HRESULT ( STDMETHODCALLTYPE *GetRemovedDestinations )( 
			__RPC__in ICustomDestinationList * This,
			/* [in] */ __RPC__in REFIID riid,
			/* [iid_is][out] */ __RPC__deref_out_opt void **ppv);

		HRESULT ( STDMETHODCALLTYPE *DeleteList )( 
			__RPC__in ICustomDestinationList * This,
			/* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszAppID);

		HRESULT ( STDMETHODCALLTYPE *AbortList )( 
			__RPC__in ICustomDestinationList * This);

	END_INTERFACE
} ICustomDestinationListVtbl;

interface ICustomDestinationList
{
	CONST_VTBL struct ICustomDestinationListVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define ICustomDestinationList_QueryInterface(This,riid,ppvObject)	\
	( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICustomDestinationList_AddRef(This)	\
	( (This)->lpVtbl -> AddRef(This) ) 

#define ICustomDestinationList_Release(This)	\
	( (This)->lpVtbl -> Release(This) ) 


#define ICustomDestinationList_SetAppID(This,pszAppID)	\
	( (This)->lpVtbl -> SetAppID(This,pszAppID) ) 

#define ICustomDestinationList_BeginList(This,pcMinSlots,riid,ppv)	\
	( (This)->lpVtbl -> BeginList(This,pcMinSlots,riid,ppv) ) 

#define ICustomDestinationList_AppendCategory(This,pszCategory,poa)	\
	( (This)->lpVtbl -> AppendCategory(This,pszCategory,poa) ) 

#define ICustomDestinationList_AppendKnownCategory(This,category)	\
	( (This)->lpVtbl -> AppendKnownCategory(This,category) ) 

#define ICustomDestinationList_AddUserTasks(This,poa)	\
	( (This)->lpVtbl -> AddUserTasks(This,poa) ) 

#define ICustomDestinationList_CommitList(This)	\
	( (This)->lpVtbl -> CommitList(This) ) 

#define ICustomDestinationList_GetRemovedDestinations(This,riid,ppv)	\
	( (This)->lpVtbl -> GetRemovedDestinations(This,riid,ppv) ) 

#define ICustomDestinationList_DeleteList(This,pszAppID)	\
	( (This)->lpVtbl -> DeleteList(This,pszAppID) ) 

#define ICustomDestinationList_AbortList(This)	\
	( (This)->lpVtbl -> AbortList(This) ) 

#endif /* COBJMACROS */
#endif 	/* C style interface */
#endif 	/* __ICustomDestinationList_INTERFACE_DEFINED__ */
