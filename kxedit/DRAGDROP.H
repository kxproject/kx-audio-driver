#ifndef __DRAGDROP_H__
#define __DRAGDROP_H__

class CEdit;

class CDataObject : public IDataObject
{
	private:

	HGLOBAL m_hGlobal;

	class XEnumFORMATETC : public IEnumFORMATETC
	{
		public:
		XEnumFORMATETC() { m_pUnkOuter = NULL; m_nPos = 0; }
		void SetOuterUnknown( IUnknown *pUnkOuter ) { m_pUnkOuter = pUnkOuter; }

		STDMETHODIMP QueryInterface( REFIID, LPVOID * );
		STDMETHODIMP_( ULONG ) AddRef( void );
		STDMETHODIMP_( ULONG ) Release( void );
		STDMETHODIMP Next( ULONG celt, FORMATETC *rgelt, ULONG *pceltFetched );
		STDMETHODIMP Skip( ULONG celt );
		STDMETHODIMP Reset( void );
		STDMETHODIMP Clone( IEnumFORMATETC **ppenum );
		IUnknown *m_pUnkOuter;
		int m_nPos;

	} m_XEnumFORMATETC;

	public:

	CDataObject( HGLOBAL hGlobal );
	~CDataObject( void );

	// IUnknown
	STDMETHODIMP QueryInterface( REFIID, LPVOID * );
	STDMETHODIMP_( ULONG ) AddRef( void );
	STDMETHODIMP_( ULONG ) Release( void );

	// IDataObject
	STDMETHODIMP GetData( LPFORMATETC, LPSTGMEDIUM );
	STDMETHODIMP GetDataHere( LPFORMATETC, LPSTGMEDIUM );
	STDMETHODIMP QueryGetData( LPFORMATETC );
	STDMETHODIMP GetCanonicalFormatEtc( LPFORMATETC,LPFORMATETC );
	STDMETHODIMP SetData( LPFORMATETC, STGMEDIUM *, BOOL );
	STDMETHODIMP EnumFormatEtc( DWORD, LPENUMFORMATETC * );
	STDMETHODIMP DAdvise( FORMATETC FAR *, DWORD,  LPADVISESINK, DWORD * );
	STDMETHODIMP DUnadvise( DWORD );
	STDMETHODIMP EnumDAdvise( LPENUMSTATDATA * );
};

class CDropTarget : public IDropTarget
{
	public:

	CDropTarget();
	void SetCtrl( CEdit *pCtrl );
	void NormalizeDropEffect( DWORD &dwEffect ) const;

	protected:

	CEdit *m_pCtrl;
	LPDATAOBJECT m_pIDataObject;
	DWORD m_dwSourceEffect;

	//IDropTarget interface members

	public:

	STDMETHODIMP QueryInterface( REFIID, LPVOID * );
	STDMETHODIMP_( ULONG ) AddRef();
	STDMETHODIMP_( ULONG ) Release();

	protected:

	STDMETHODIMP DragEnter( LPDATAOBJECT, DWORD, POINTL, LPDWORD );
	STDMETHODIMP DragOver( DWORD, POINTL, LPDWORD );
	STDMETHODIMP DragLeave();
	STDMETHODIMP Drop( LPDATAOBJECT, DWORD, POINTL, LPDWORD );
};

class CDropSource : public IDropSource
{
	public:

	CDropSource();

	public:

	// IUnknown
	STDMETHODIMP QueryInterface( REFIID, LPVOID * );
	STDMETHODIMP_( ULONG ) AddRef();
	STDMETHODIMP_( ULONG ) Release();

	protected:

	//IDropSource interface members
	STDMETHODIMP QueryContinueDrag( BOOL, DWORD );
	STDMETHODIMP GiveFeedback( DWORD );
};


#endif