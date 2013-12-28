#ifndef __UNDO_H__
#define __UNDO_H__

class CBuffer;

class CUndoAction
{
public:
	CUndoAction( BOOL bInsert );
	virtual ~CUndoAction() {};

	void FlagAsStartAction();
	void FlagAsEndAction();
	void EnableAppend();
	BOOL CanAppend() const;
	BOOL IsStartAction() const;
	BOOL IsEndAction() const;

	virtual void Undo( CBuffer *pBuffer ) const = 0;
	virtual void Redo( CBuffer *pBuffer ) const = 0;
	BOOL IsInsertAction() const;

	virtual BOOL Append( BOOL bInsertChar, TCHAR ch, int nRow, int nCol );
	virtual void SetCol( int nCol );

private:

	enum { BIT_START = 0x1, BIT_END = 0x2, BIT_INSERT = 0x4, BIT_APPEND = 0x8 };
	BYTE m_fFlags;

protected:

	int  m_nRow;  
};

class CUndoInsertDeleteString : public CUndoAction
{ 
public:
	CUndoInsertDeleteString( BOOL bInsert, LPCTSTR pszText, int cbText, int nRow, int nCol );
	~CUndoInsertDeleteString();
	virtual BOOL Append( BOOL bInsertChar, TCHAR ch, int nRow, int nCol );
	virtual void Undo( CBuffer *pBuffer ) const;
	virtual void Redo( CBuffer *pBuffer ) const;

private:

	LPTSTR m_pszText;
	int m_nCol;
};

class CUndoAddRemoveLine : public CUndoAction
{ 
public:
	CUndoAddRemoveLine( BOOL bInsert, LPCTSTR pszText, int cbText, int nRow );
	~CUndoAddRemoveLine();

	virtual void Undo( CBuffer *pBuffer ) const;
	virtual void Redo( CBuffer *pBuffer ) const;

private:

	LPTSTR m_pszText;
};

class CUndoStartEnd : public CUndoAction
{ 
public:
	CUndoStartEnd( int nCursorLine, int nCursorCol );

	virtual void Undo( CBuffer *pBuffer ) const;
	virtual void Redo( CBuffer *pBuffer ) const;

	virtual void SetCol( int nCol );

private:

	int m_nCol;
};

#endif