// Languagex.h : Declaration of the CLanguageX

#ifndef __LANGX_H_
#define __LANGX_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLanguageX
class ATL_NO_VTABLE CLanguageX : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLanguageX, &CLSID_Language>,
	public ISupportErrorInfo,
	public IDispatchImpl<ILanguage, &IID_ILanguage, &LIBID_CodeMax, CM_X_VERSION>
{
public:
	CLanguageX();
	~CLanguageX();
	
	CM_LANGUAGE m_cmLang;

public:

DECLARE_REGISTRY_RESOURCEID(IDR_LANGUAGEX)

BEGIN_COM_MAP(CLanguageX)
	COM_INTERFACE_ENTRY(ILanguage)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ILanguage
	virtual HRESULT STDMETHODCALLTYPE put_CaseSensitive(VARIANT_BOOL bVal);
	virtual HRESULT STDMETHODCALLTYPE get_CaseSensitive(VARIANT_BOOL __RPC_FAR *pbVal);
	virtual HRESULT STDMETHODCALLTYPE put_Keywords(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_Keywords(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_Operators(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_Operators(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_SingleLineComments(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_SingleLineComments(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_MultiLineComments1(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_MultiLineComments1(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_MultiLineComments2(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_MultiLineComments2(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_ScopeKeywords1(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_ScopeKeywords1(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_ScopeKeywords2(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_ScopeKeywords2(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_StringDelims(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_StringDelims(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_EscapeChar(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_EscapeChar(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_TerminatorChar(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_TerminatorChar(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_TagElementNames(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_TagElementNames(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_TagAttributeNames(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_TagAttributeNames(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_TagEntities(BSTR strVal);
	virtual HRESULT STDMETHODCALLTYPE get_TagEntities(BSTR __RPC_FAR *pstrVal);
	virtual HRESULT STDMETHODCALLTYPE put_Style( cmLangStyle Style );
	virtual HRESULT STDMETHODCALLTYPE get_Style( cmLangStyle __RPC_FAR *pStyle );

	void Copy( CM_LANGUAGE *pLang );

private:
	void DeleteMembers();

};

#endif
