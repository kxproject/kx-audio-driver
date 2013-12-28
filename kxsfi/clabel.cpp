//-----------------------------------------------------------------------------
// Clabel.cpp - // custom label object
//-----------------------------------------------------------------------------
class CLabel : public CParamDisplay	
{
public:
	CLabel (CRect &size, char *text, int align);

	void draw (CDrawContext *pContext);
	void setLabel (char *text);
	void setStatus (int status);

	int status;		// 1 - selected, 0 - not selected

protected:
	int t;			// text align; 0 - kLeftText, 1 - kCenterText,	2 - kRightText
	char label[256];
};

//-----------------------------------------------------------------------------
// CLabel implementation
//-----------------------------------------------------------------------------
CLabel::CLabel (CRect &size, char *text, int align) : CParamDisplay (size)
{
	setLabel(text);
	t = align;		// set text align
}
//----------------------------------------------
void CLabel::setStatus (int value)
{
	status = value;

}
//------------------------------------------------------------------------
void CLabel::setLabel (char *text)
{
	if (text)
		strncpy (label, text, sizeof(label));
	else
		label[0]=0;
	setDirty ();
}
//------------------------------------------------------------------------
void CLabel::draw (CDrawContext *pContext)
{
	pContext->setFillColor (backColor);
	pContext->fillRect (size);
	pContext->setFrameColor (backColor);
	pContext->drawRect (size);
	pContext->setFont (fontID);
	pContext->setFontColor (fontColor);

	//	0 - kLeftText, 1 - kCenterText,	2 - kRightText
	switch (t)
	{
	case 0:
		pContext->drawString (label, size, false, kLeftText);
		break;
	case 1:
		pContext->drawString (label, size, false, kCenterText);
		break;
	case 2:
		pContext->drawString (label, size, false, kRightText);
		break;
	}
}

//-----------------------------------------------------------------------------
