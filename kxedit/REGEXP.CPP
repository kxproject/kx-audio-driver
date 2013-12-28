#include "precomp.h"
#include "regexp.h"

void CRegExp::ConvertToFiniteAutomaton( LPCTSTR pszRegExp )
{
	// Traverses the input expression and constructs a finite 
	// automaton expressing it 
	BOOL bRetVal = TRUE;
	BOOL bBackslashFound = FALSE;
	BOOL bOrOperatorFound = FALSE;
	BOOL bInSet = FALSE;
	BOOL bFirstSetChar = TRUE;
	BOOL bInRange = FALSE;
	TCHAR chFirstInSet = _T( '\0' );
		
	int nOuterFromState = eINVALIDSTATE;
	int nInnerFromState = eINVALIDSTATE;
	int nInnerToState = eINVALIDSTATE;
	int nOuterToState = eINVALIDSTATE;
	int nTempState = eINVALIDSTATE;
		
	nInnerToState = eSTARTSTATE;
	nOuterToState = NewState();
		
	// For a regular expression, the empty event is assumed 
	// to be '\0'
	AddEmptyTransition( nInnerToState, nOuterToState );
		
	for ( TCHAR ch; ( ch = *pszRegExp ) != 0; pszRegExp++ ) 
	{
		if ( bBackslashFound || !SpecialChar( ch ) ) 
		{
			//if the previous character was a '\' then 
			//character may need translation.
			if ( bBackslashFound )
			{
				ch = TranslateCh( ch );
				bBackslashFound = FALSE;
			}
		
			if ( bInSet )
			{
				// Is the first character in the set? 
				if ( bFirstSetChar )
				{
					// Add new states for either side of set expression
					nInnerFromState = NewState();
					nInnerToState = NewState();
					bFirstSetChar = FALSE;
					chFirstInSet = ch;
				}
				if ( bInRange )
				{
					// fill out the range
					if ( ch > chFirstInSet )
					{
					for ( int i = chFirstInSet + 1; i <= ch; i++ )
						AddTransition( nInnerFromState, ( TCHAR ) i, nInnerToState );
					}
					bInRange = FALSE;
				}
				else
				{
					// Add transition for new character
					AddTransition( nInnerFromState, ch, nInnerToState );
				}
			}
			else
			{
				// Single character transition
				nInnerFromState = NewState();
				nInnerToState = NewState();
				AddTransition( nInnerFromState, ch, nInnerToState );
		
				// Connect new transition to rest of automaton
				ConnectSubExpr( 
					nOuterFromState, nInnerFromState, nInnerToState, 
					nOuterToState, bOrOperatorFound );
		
				bOrOperatorFound = FALSE;
				bBackslashFound = FALSE;
			}
		}
		else
		{
			// Handle special characters
			switch ( ch )
			{
				case _T( '|' ):	// OR operator
					bOrOperatorFound = TRUE;
					break;
		
				case _T( '\\' ):	// escape character
					bBackslashFound = TRUE;
					break;
		
				case _T( '[' ):	// opening a set
					if ( !bInSet )
					{
						bInSet = TRUE;
						bFirstSetChar = TRUE;
					}
					break;
		
				case _T( ']' ):	// closing a set
						if ( bInSet && !bFirstSetChar )
						{
							bInSet = FALSE;
							bFirstSetChar = TRUE;
							// Connect set transitions to rest of automaton
							ConnectSubExpr( 
									nOuterFromState, nInnerFromState, nInnerToState,
									nOuterToState, bOrOperatorFound );
						}
						break;
		
				case _T( '*' ):	// zero or more
					AddEmptyTransition( nInnerFromState, nInnerToState );
					AddEmptyTransition( nInnerToState, nInnerFromState );
					break;
		
				case _T( '+' ):	// one or more
					AddEmptyTransition( nInnerToState, nInnerFromState );
					continue;
		
				 case _T( '?' ):   // any character
					// add a character transition for anyEvent
					nInnerFromState = NewState();
					nInnerToState = NewState();
					AddTransition( nInnerFromState, ANYEVENT, nInnerToState );

					// Connect new transition to rest of automaton
					ConnectSubExpr( nOuterFromState, nInnerFromState, nInnerToState,
								   nOuterToState, bOrOperatorFound );

					bOrOperatorFound = FALSE;
				   continue;
		
				case _T( '(' ):	// opening parenthesised sub-exp
						nInnerFromState = NewState();
						Push( nInnerFromState, nOuterFromState, nOuterToState, 
							bOrOperatorFound );
						nOuterToState = nInnerFromState;
						bOrOperatorFound = FALSE;
						break;
		
				case _T( ')' ):	// closing parenthesised sub-exp
						nInnerToState = nOuterToState;
						Pop( nInnerFromState, nOuterFromState, nOuterToState, 
							bOrOperatorFound );
						ConnectSubExpr( 
							nOuterFromState, nInnerFromState, nInnerToState,
							nOuterToState, bOrOperatorFound );
						break;
				case _T( '-' ):
					bInRange = TRUE;
					break;
			}
	}
	}
	SetAcceptState( nOuterToState );
}
		
		
// Implementation for helper functions
TCHAR CRegExp::TranslateCh( TCHAR ch ) 
{
	TCHAR chRetVal = ch;
	switch ( ch )
	{
			case _T( 'n' ): chRetVal = _T( '\n' ); break;
			case _T( 't' ): chRetVal = _T( '\t' ); break;
	}
	return( chRetVal );
}
		
// Determines whether the passed character is in any way 'special', 
// that is whether or not it is a meta-character controlling the 
// meaning of the regular expression string
BOOL CRegExp::SpecialChar( TCHAR ch ) 
{
	BOOL bRetVal = FALSE;
	switch ( ch )
	{
		case _T( '\\' ):
		case _T( '(' ):
		case _T( ')' ):
		case _T( '|' ):
		case _T( '[' ):
		case _T( ']' ):
		case _T( '*' ):
		case _T( '+' ):
		case _T( '-' ):
		case _T( '?' ):
			bRetVal = TRUE;
			break;
	}
	return( bRetVal );
}
		
void CRegExp::Push( int nS1, int nS2, int nS3, BOOL b1 )
{
	m_SubExpStack.insert( m_SubExpStack.begin(), ( DWORD ) new CSubExpState( nS1, nS2, nS3, b1 ) );
}
		
void CRegExp::Pop( int &nS1, int &nS2, int &nS3, BOOL &b1 )
{
	ASSERT( !m_SubExpStack.empty() );
		
	CSubExpState *pSubExp = ( CSubExpState * )m_SubExpStack.at( 0 );
	nS1 = pSubExp->m_nS1;
	nS2 = pSubExp->m_nS2;
	nS3 = pSubExp->m_nS3;
	b1 = pSubExp->m_b1;
	m_SubExpStack.erase( m_SubExpStack.begin() );
	delete pSubExp;
}
		
void CRegExp::ConnectSubExpr( int& nOuterFromState, int nInnerFromState, int nInnerToState,
	int& nOuterToState, BOOL& bOrOperatorFound )
{
	// was the TCHAR before the subexpression an OR operator?
	if ( bOrOperatorFound )
	{
		// Yes, so connect it up accordingly 
		AddEmptyTransition( nOuterFromState, nInnerFromState );
		AddEmptyTransition( nInnerToState, nOuterToState );
	}
	else
	{
		// No, so concatenate the sub-exp onto the automaton
		nOuterFromState = nOuterToState;
		AddEmptyTransition( nOuterFromState, nInnerFromState );
		nOuterToState = NewState();
		AddEmptyTransition( nInnerToState, nOuterToState );
	}
	bOrOperatorFound = FALSE;
}
		
//////////////////////////////////////////////// 
// Constructor
CRegExp::CRegExp( CFABuild& faBuild ) 
	:m_pBuild( &faBuild )
{
}
		
//////////////////////////////////////////////// 
// Destructor
CRegExp::~CRegExp() 
{
	// clear out the sub-exp stack.
	while( !m_SubExpStack.empty() )
	{
		CSubExpState *pSubExp = ( CSubExpState * )m_SubExpStack.at( 0 );
		m_SubExpStack.erase( m_SubExpStack.begin() );
		delete pSubExp;
	}
}
		
		
//////////////////////////////////////////////// 
// envelope/letter delegation methods.
		
int CRegExp::NewState()
{
	return m_pBuild->NewState();
}
		
void CRegExp::AddTransition( int nSourceState, TCHAR chEvent, int nTargetState )
{
	m_pBuild->AddTransition( nSourceState, chEvent, nTargetState );
}
		
void CRegExp::AddEmptyTransition( int nSourceState, int nTargetState )
{
	m_pBuild->AddEmptyTransition( nSourceState, nTargetState );
}
		
void CRegExp::SetAcceptState( int nAcceptState )
{
	m_pBuild->SetAcceptState( nAcceptState );
}
		
//
////////////////////////////////////
		
void CRegExp::SetBuild( CFABuild* im_pBuild )
{
	m_pBuild = im_pBuild;
}
		
CFABuild* CRegExp::GetBuild() const 
{
	return m_pBuild;
}
		
CIntSet::CIntSet( int initDim )
{
	// Do presizing if real dimension passed
	if ( initDim > 0 )
		resize( initDim, 0 );
}
		
// Looks for given value from passed index 
// Returns index if found, -1 if not
int CIntSet::Find( int value, int start ) 
{
	if ( start >= 0 && start < size() ) 
	{
			for ( ; start < size(); start++ )
			{
				if ( at( start ) == value )
						return( start );
			}
	}
	return( -1 );
}
		
// Adds a new value into the set, 
// if not already present
void CIntSet::Append( int value )
{
	if ( Find( value ) < 0 )
	{
			push_back( ( DWORD ) value );
	}
}
		
// Append the set but ensuring there are 
// no repititions 
void CIntSet::Append( CIntSet& iset )
{
	for ( int i = 0;i < iset.size();i++ ) 
	{
	Append( iset.at( i ) );
	}
}
		
CIntSet::CIntSet( const CIntSet &src ) 
{
	*this = src;
}
		
CIntSet &CIntSet::operator=( const CIntSet &src ) 
{
	if ( this != &src )
	{
			clear();
			for ( int i = 0; i < src.size(); i++ )
				push_back( src.at( i ) );
	}
	return *this;
}

BOOL CFiniteAutomaton::ToStatesForEvent( int nFrom, TCHAR chOnInput, CIntSet& ToSet )
{
	return pFaQuery->ToStatesForEvent( nFrom, chOnInput, ToSet );
}
		
int CFiniteAutomaton::GetAcceptState() 
{
	return pFaQuery->GetAcceptState();
}
		
CFiniteAutomaton::CFiniteAutomaton( CFAQuery& iquery ) 
	:pFaQuery( &iquery )
{
}
		
///////////////////////////////////////////////// 
// CAutomatonState
// State interator for CFiniteAutomaton
		
// only to be used by CFiniteAutomaton::Create... Iterator methods
CAutomatonState::CAutomatonState( CFiniteAutomaton *pParent, int startState )
	: m_pParent( pParent )
{
	// Initialise state set with start state 
	m_CurrentState.Append( startState );
	// ... and traverse all empty states from here 
	TraverseEvents( m_CurrentState, m_CurrentState, EMPTYEVENT );
}
		
CAutomatonState::~CAutomatonState()
{ }
		
CAutomatonState::CAutomatonState( const CAutomatonState &src )
{
	*this = src;
}
		
CAutomatonState &CAutomatonState::operator=( const CAutomatonState &src )
{
	if ( this != &src )
	{
			m_pParent = src.m_pParent;
			m_CurrentState = src.m_CurrentState;
	}
		
	return( *this );
}
		
BOOL CAutomatonState::IsAccept()
{
	return( m_CurrentState.Find( m_pParent->GetAcceptState() )
													!= -1 );
}
		
BOOL CAutomatonState::IsValid()
{
	return( m_CurrentState.size() > 0 );
}
		
// Accepts an event and uses the state machine to move onto the 
// next state. Returns true if the state is non-terminal, ie 
// not accept, and valid. Note: we treat accept as being a 
// terminal state here for the sake of consistency with a 
// notional deterministic automaton interface, and also because 
// in many cases finding an accept state is sufficient reason to 
// halt further processing 
BOOL CAutomatonState::NextState( TCHAR chEvent )
{
	PerformTransitionSet( chEvent );
	return( IsValid() && !IsAccept() );
}
		
// Method to do the non-deterministic transitions. The algorithm is 
// to follow all transitions from the current set of states which 
// correspond to the passed event until there are no more 
// transitions to follow for the current event. Empty event 
// transitions are treated as always valid. Note: if this method is 
// called with an accept position in the m_CurrentState set, that 
// position may be lost.
void CAutomatonState::PerformTransitionSet( TCHAR chEvent )
{
	// Reject if m_CurrentState is not valid 
	if ( !IsValid() )
	{
		return;
	}
		
	// In a deterministic automaton, we would not perform any further 
	// traversal if we had reached an accept state. However, a non- 
	// deterministic automaton is not necessarily in a terminal state 
	// if the accept state has been found
		
	// Storage for new state set
	CIntSet NextState;
		
	TraverseEvents( m_CurrentState, NextState, chEvent );
	m_CurrentState = NextState;
		
	TraverseEvents( m_CurrentState, m_CurrentState, EMPTYEVENT );
}
		
// Helper function to handle traversal of available transitions for 
// given event from positions in source set. Adds states traversed 
// to onto target set. If source and target are the same, the 
// effect will be to traverse all adjacent transitions for the 
// passed event ( this is useful for closure of empty transitions ).
void CAutomatonState::TraverseEvents( CIntSet &Source, CIntSet &Target, TCHAR chEvent )
{
	for ( int i = 0; i < Source.size(); i++ )
	{
		CIntSet ToSet;
		int nFrom = Source.at( i );
		m_pParent->ToStatesForEvent( nFrom, chEvent, ToSet );
			Target.Append( ToSet );
	}
}

CSimpleFA::CSimpleFA() :
	m_nTrans( 0 ), 
	m_nAccState( eINVALIDSTATE ),
	m_nStates( 1 ) // start state always present
{
}
		
int CSimpleFA::NewState()
{
	return( m_nStates++ );
}
		
// Add new transition for a particular event 
// between two state nodes
void CSimpleFA::AddTransition( int nSourceState, TCHAR chEvent, int nTargetState )
{
	m_Trans[m_nTrans].m_nFrom = nSourceState; 
	m_Trans[m_nTrans].m_ch = chEvent;
	m_Trans[m_nTrans].m_nTo = nTargetState; 
	m_nTrans++;
}
		
// Add empty transition between two state nodes 
void CSimpleFA::AddEmptyTransition( int sourceState, int targetState )
{
	AddTransition( sourceState, EMPTYEVENT, targetState );
}
		
// Record what the accept state is.
void CSimpleFA::SetAcceptState( int nAcceptState ) 
{
	m_nAccState = nAcceptState;
}
		
// Searches for all state nodes reachable from the given from 
// state for the passed event. returns true if at least one 
// target state is found.
BOOL CSimpleFA::ToStatesForEvent( int nFrom, TCHAR chOnInput, CIntSet& ToSet )
{
	BOOL retval = FALSE;
	// do a simple linear search of the transitions array.
	for ( int i = 0;i < m_nTrans;i++ )
	{
		if ( ( m_Trans[i].m_nFrom == nFrom && m_Trans[i].m_ch == chOnInput ) ||
		    ( chOnInput && m_Trans[i].m_nFrom == nFrom && m_Trans[i].m_ch == ANYEVENT ) )
		{
			ToSet.Append( m_Trans[i].m_nTo );
			retval = TRUE;
		}
	}
	return retval;
}
		
int CSimpleFA::GetAcceptState()
{
	return m_nAccState;
}
		
BOOL RegExpStrCmp( CFiniteAutomaton &fa, LPCTSTR pszSearchText, int &nMatchLen )
{
	BOOL bRetVal = FALSE;
	CAutomatonState astate( &fa, 0 );
	LPCTSTR pszLeadByte = pszSearchText;
	for ( int nLen = 0; *pszSearchText && astate.IsValid(); pszSearchText++ )
	{
		astate.NextState( *pszSearchText );
		nLen++;

		if ( astate.IsAccept() )
		{
			bRetVal = TRUE;
			nMatchLen = nLen;
		}
	}

	return bRetVal;
}
