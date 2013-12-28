#pragma once
typedef vector<DWORD> CDWordVector;

class CIntSet : CDWordVector
{
public:
	CIntSet(int initDim = 0);	// starting size
	int Find(int value, int start = 0);
	void Append(int value);
	void Append(CIntSet& iset);
	int size() const 
		{ return CDWordVector::size(); }	// this takes care of bogus compiler error in VC++ (fixed in SP3)
	int at( size_type i ) const
		{ return ( int ) CDWordVector::at( i ); }	// this takes care of bogus compiler error in VC++ (fixed in SP3)
		
	CIntSet(const CIntSet &src);
	CIntSet &operator=(const CIntSet &src);
};
		
class CFAQuery 
{
public: 
	virtual BOOL ToStatesForEvent( int nFrom, TCHAR chOnInput, CIntSet& ToSet) = 0; 
	virtual int GetAcceptState() = 0; 
};

class CFABuild 
{
public: 
	virtual int NewState() = 0;
	virtual void AddTransition( int nSourceState, TCHAR chEvent, int nTargetState ) = 0;
	virtual void AddEmptyTransition( int nSourceState, int nTargetState ) = 0;
	virtual void SetAcceptState( int nAcceptState ) = 0;
}; 

enum { eSTARTSTATE = 0,
		eINVALIDSTATE = -1 };
		
#define EMPTYEVENT _T('\0')
#define ANYEVENT   _T('\1')

// Use of the envelope/letter idiom -- methods overridden by this 
// class are implemented by delegating to the letter class. 
class CFiniteAutomaton : protected CFAQuery
{
public:
	CFiniteAutomaton(CFAQuery&);
	virtual ~CFiniteAutomaton() {;}
		
	//methods overridden from base class. 
	virtual BOOL ToStatesForEvent(int nFrom, TCHAR chOnInput, CIntSet& ToSet);
	virtual int GetAcceptState(); 
		
private:
	CFAQuery *pFaQuery;
};
		
// the CAutomatonState class acts as an iterator for tracking 
// the state in input matching functions.
class CAutomatonState
{
	friend class CFiniteAutomaton;
		
public:
	virtual ~CAutomatonState();
		
	CAutomatonState(CFiniteAutomaton *parent, int startState);
		
	// Users are allowed to copy and assign iterators, though
	CAutomatonState(const CAutomatonState &src); 
	CAutomatonState &operator=(const CAutomatonState &src);
		
	virtual BOOL IsAccept();
	virtual BOOL IsValid();
		
	// Accepts an event and uses the state machine to move onto the 
	// next state Returns true if the state is non-terminal, ie not 
	// accept, and valid.
	virtual BOOL NextState(TCHAR chEvent);
		
private:
		
	CFiniteAutomaton *m_pParent;
	// current state is a set of states in a 
	// non-deterministic automaton
	CIntSet m_CurrentState;
		
	// Helper method to do the non-deterministic transitions
	void PerformTransitionSet(TCHAR chEvent); 
	void TraverseEvents(CIntSet &Source, CIntSet &Target, TCHAR chEvent);
};

#define MAXTRANS	4096
		
class CSimpleFA : public CFAQuery, public CFABuild 
{
public: 
	//Constructor
	CSimpleFA();
		
	//overridden from CFAQuery 
	virtual BOOL ToStatesForEvent(int nFrom, TCHAR chOnInput, CIntSet& ToSet);
	virtual int GetAcceptState(); 
		
	//overridden from CFABuild
	virtual int NewState();
	virtual void AddTransition(int nSourceState, TCHAR chEvent, int nTargetState);
	virtual void AddEmptyTransition(int nSourceState, int nTargetState);
	virtual void SetAcceptState(int nAcceptState);
		
private:
	//the tuple that defines a single transition. 
	struct STrans
	{
		int m_nFrom;
		int m_nTo;
		TCHAR m_ch;
	};
		
	//the number of states in the FA.
	int m_nStates;
		
	//the array of transitions.
	STrans m_Trans[MAXTRANS];
		
	//the number of transitions in the FA. 
	int m_nTrans;
		
	// the accepting state.
	int m_nAccState;
		
};
		
// uses the envelope/letter idiom to hide implementation -- the 
// letter never changes. methods overridden from base class are
// implemented by delegating them to a contained letter class.
class CRegExp : protected CFABuild 
{
public:
	CRegExp( CFABuild &Build ); 
	~CRegExp();
		
	void SetBuild( CFABuild *pBuild ); 
	CFABuild* GetBuild() const;
		
	// converts the given regular expression to a finite automaton. 
	// The finite automaton data is stored in the CFABuild instance 
	// owned by this object.
	void ConvertToFiniteAutomaton( LPCTSTR regexp );
		
protected:
	//methods overridden from CFABuild
	virtual int NewState();
	virtual void AddTransition( int nSourceState, TCHAR chEvent, int nTargetState );
	virtual void AddEmptyTransition( int nSourceState, int nTargetState );
	virtual void SetAcceptState( int nAcceptState );
		
private:
		
	// Converts a TCHARacter from an escape code to corresponding 
	// ASCII value
	TCHAR TranslateCh( TCHAR ch );
		
	// Determines whether the passed TCHARacter is in any way 
	// 'special', that is whether or not it is a meta-TCHARacter 
	// controlling the meaning of the regular expression string 
	BOOL SpecialChar( TCHAR ch );
		
	// Connects subexpression automaton to main automaton
	void ConnectSubExpr( int& outerFromState, int innerFromState, int innerToState, int& outerToState, BOOL& orOperatorFound );
		
	// Sub-expression stack members -- the structure used to hold 
	// the parent sub-expression information
	struct CSubExpState
	{
			int m_nS1;
			int m_nS2;
			int m_nS3;
			BOOL m_b1;
		
			CSubExpState( int i, int j, int k, BOOL b )
				: m_nS1( i ), m_nS2( j ), m_nS3( k ), m_b1( b ) {}
	};
	//	- save the parent sub-expression 
	void Push( int nS1, int nS2, int nS3, BOOL b1 );
	//	- restore the parent sub-expression 
	void Pop( int &nS1, int &nS2, int &nS3, BOOL &b1 );
	//	- the stack storage
	CDWordVector m_SubExpStack;
		
private:
	//the contained letter class instance. 
	CFABuild* m_pBuild;
};		

BOOL RegExpStrCmp( CFiniteAutomaton &fa, LPCTSTR pszSearchText, int &nMatchLen );
