
#ifndef _U_CORE_H_
#define _U_CORE_H_

/******************************* kxuCore *******************************/

// control's bitmaps
enum {
	UCB_BACK,
	UCB_BLACKBACK,
	UCB_TEXTURE,
	UCB_FRAMEBEVEL,
	UCB_CAPTIONBAR_RIGHT,
	UCB_CAPTIONBAR_LEFT,
	UCB_SCREW,
	UCB_OUTER_SUNKEN,
	UCB_OUTER_RAISED,
	UCB_BOX_WHITE,
	UCB_ARROW_SOUTH,
	UCB_ARROW_EAST,
	UCB_ARROW_NORTH,
	UCB_ARROW_WEST,
	UCB_BUTTON_RAISED,
	UCB_BUTTON_SUNKEN,
	UCB_LAMP_ON,
	UCB_LAMP_OFF,
	UCB_CLOSE,
	UCB_MINIMIZE,
	UCB_ONTOP,
	UCB_BYPASS,
	UCB_MUTE,
	UCB_PRESET,
	UCB_KNOB,
	UCB_KNOB_MASK,
	UCB_KNOB_SMALL,
	UCB_KNOB_SMALL_MASK,
	UCB_FADER_BOX,
	UCB_VFADER_BUTTON,
	UCB_VFADER_SWITCH,
	UCB_VFADER_LINE,
	UCB_VFADER_LINE_BSHADOW,
	UCB_VFADER_LINE_TSHADOW,
	UCB_HFADER_BUTTON,
	UCB_HFADER_LINE,
	UCB_HFADER_LINE_RSHADOW,
	UCB_HFADER_LINE_LSHADOW,
	UCB_PLGBOX,
	UCB_PLGBOX_HEADER,
	UCB_PLGBOX_PIN,
	UCB_SHADOW1,

	_uelements_count
};

// element data 
// indexes, used with GetElementData
#define UED_TYPE		1
#define UED_SIZERECT	2
#define UED_SIZEWIDTH	4
#define UED_SIZEHEIGHT	5	
#define UED_MARGINS		6
#define UED_BORDER		10
#define UED_NFRAMES		11

// uFonts
enum {
	UF_DEFAULT,
	UF_HEADER,
	UF_SMALL,
	UF_FATSMALL,
	UF_HEADERNARROW,
	UF_SMALLLABEL,

	_ufonts_count
};

// uColors
enum {
	_UC_WHITE,
	_UC_LIGHT,
	_UC_GREY,	// main
	_UC_DARK,
	_UC_BLACK,
	_UC_HOVER,	// default: ~yellow
	_UC_GOOD,	// default: ~green
	_UC_BAD,	// default: ~red

	_ucolors_count
};
//................................................................................

typedef const struct _pointfont{
	int height;
	int weight;
	const char* facename;
} PointFont;

typedef struct {
	RECT SrcRect;			
	RECT Margins;
	int Border;
	int nFrames;		// for film bitmap
	int Reserved[4];
} ElementData;

typedef struct {
	int Resource;
	int Type;
	ElementData Data;
} BitmapElement;

//................................................................................

#define BET_TYPE_MASK		0x00000FFF

#define BET_SOLID			0
#define BET_PLAINBITMAP		1
#define BET_MARGINBITMAP	2
#define BET_FILMBITMAP		3 	// fixme, horizontal film only?

#define BET_FLAGS_MASK		0xFFFFF000

#define BET_DEFAULT			0x0000
#define BET_FIXEDWIDTH		0x1000
#define BET_FIXEDHEIGHT		0x2000

/////////////////////////////////////////////////////////////////////////////

#define RESOURCELIB	"ufxrc.dll"

class kxuCore
{
// Attributes
public:

// Operations
public:
	kxuCore();
	virtual ~kxuCore();

// Overrides
public:
	int		IsOK();

	int		GetIcon(HICON* icon);
	CFont*	GetFont(int Id = UF_DEFAULT) {return &Font[Id];};
	int	GetAveCharWidth(int Id = UF_DEFAULT) {return AveCharWidth[Id];};
	COLORREF GetColor(int Id);
	int	GetElementData(int Id, int Index, int Size, LPVOID pData);
	int	GetElementData(int Id, int Index);
	int		GetControlBitmap(CBitmap* pDstBitmap, LONG Flags, int nItems, int* pId, RECT** ppDstRect);
	int		CopyCBitmap(CBitmap* pDstBitmap, CBitmap* pSrcBitmap);

// Implementation
protected:
	HMODULE ResourceLib;
	CFont Font[_ufonts_count];
	int AveCharWidth[_ufonts_count];

	void RenderPlainBitmap(CDC* pDstDC, LPRECT pDstRect, CBitmap* pSrcBitmap, ElementData* pRenderData);
	void RenderMarginBitmap(CDC* pDstDC, LPRECT pDstRect, CBitmap* pSrcBitmap, ElementData* pRenderData);	
};

// Globals
int kxuRegisterWindowClass(LPCSTR lpszClassName);

#endif // _U_CORE_H_
