#pragma comment(lib,"ComCtl32.lib")
#pragma comment(linker,"/opt:nowin98")
#include <windows.h>
#include <commctrl.h>
#include <math.h>

CHAR szClassName[]="window";
HWND hTrack;
STDAPI_(VOID) DrawPie(HDC hDC,LPCRECT lprcItem,UINT uPctX10,UINT uOffset);

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	static const RECT rect={10,50,265,154};
	switch(msg){
	case WM_HSCROLL:
		InvalidateRect(hWnd,&rect,FALSE);
		break;
	case WM_CREATE:
		InitCommonControls();
		hTrack=CreateWindow("msctls_trackbar32","",WS_CHILD|WS_VISIBLE|0x100,10,10,256,30,hWnd,NULL,((LPCREATESTRUCT)lParam)->hInstance,NULL);
		SendMessage(hTrack,TBM_SETRANGE,(WPARAM)TRUE,(LPARAM)MAKELPARAM(0,1000));
		break;
	case WM_PAINT:
		hdc=BeginPaint(hWnd,&ps);
		DrawPie(hdc,&rect,SendMessage((HWND)hTrack,TBM_GETPOS,0,0),23);
		EndPaint(hWnd,&ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return(DefWindowProc(hWnd,msg,wParam,lParam));
    }
    return(0L);
}

int WINAPI WinMain(HINSTANCE hinst,HINSTANCE hPreInst,
                   LPSTR pCmdLine,int nCmdShow)
{
    HWND hWnd;
    MSG msg;
    WNDCLASS wndclass;
    if(!hPreInst){
        wndclass.style=CS_HREDRAW|CS_VREDRAW;
        wndclass.lpfnWndProc=WndProc;
        wndclass.cbClsExtra=0;
        wndclass.cbWndExtra=0;
        wndclass.hInstance =hinst;
        wndclass.hIcon=NULL;
        wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
        wndclass.hbrBackground=(HBRUSH)(COLOR_3DFACE+1);
        wndclass.lpszMenuName=NULL;
        wndclass.lpszClassName=szClassName;
        if(!RegisterClass(&wndclass))
            return FALSE;
    }
    hWnd=CreateWindow(szClassName,
        "ƒ^ƒCƒgƒ‹",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hinst,
        NULL);
    ShowWindow(hWnd,nCmdShow);
    UpdateWindow(hWnd);
    while(GetMessage(&msg,NULL,0,0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return(msg.wParam);
}

int IntSqrt(unsigned long dwNum)
{
	DWORD dwSqrt=0,dwRemain=0,dwTry;
	int i;
	for(i=0;i<16;++i)
	{
		dwRemain=(dwRemain<<2)|(dwNum>>30);
		dwSqrt<<=1;
		dwTry=dwSqrt*2+1;
		if(dwRemain>=dwTry)
		{
			dwRemain-=dwTry;
			dwSqrt|=0x01;
		}
		dwNum<<=2;
	}
	return(dwSqrt);
}

STDAPI_(VOID) DrawPie(HDC hDC,LPCRECT lprcItem,UINT uPctX10,UINT uOffset)
{
	int cx,cy,rx,ry,x,y;
	int uQPctX10;
	RECT rcItem;
	HRGN hEllRect,hEllipticRgn,hRectRgn;
	HBRUSH hBrush,hOldBrush;
	HPEN hPen,hOldPen;
	rcItem=*lprcItem;
	rcItem.left=lprcItem->left;
	rcItem.top=lprcItem->top;
	rcItem.right=lprcItem->right-rcItem.left;
	rcItem.bottom=lprcItem->bottom-rcItem.top-uOffset;
	rx=rcItem.right/2;
	cx=rcItem.left+rx-1;
	ry=rcItem.bottom/2;
	cy=rcItem.top+ry-1;
	if(rx<=10||ry<=10)return;
	rcItem.right=rcItem.left+2*rx;
	rcItem.bottom=rcItem.top+2*ry;
	if(uPctX10>1000)uPctX10=1000;
	uQPctX10=(uPctX10 % 500)-250;
	if(uQPctX10<0)uQPctX10=-uQPctX10;
	if(uQPctX10<120)
	{
		x=IntSqrt(((DWORD)rx*(DWORD)rx*(DWORD)uQPctX10*(DWORD)uQPctX10)/((DWORD)uQPctX10*(DWORD)uQPctX10+(250L-(DWORD)uQPctX10)*(250L-(DWORD)uQPctX10)));
		y=IntSqrt(((DWORD)rx*(DWORD)rx-(DWORD)x*(DWORD)x)*(DWORD)ry*(DWORD)ry/((DWORD)rx*(DWORD)rx));
	}
	else
	{
		y=IntSqrt((DWORD)ry*(DWORD)ry*(250L-(DWORD)uQPctX10)*(250L-(DWORD)uQPctX10)/((DWORD)uQPctX10*(DWORD)uQPctX10+(250L-(DWORD)uQPctX10)*(250L-(DWORD)uQPctX10)));
		x=IntSqrt(((DWORD)ry*(DWORD)ry-(DWORD)y*(DWORD)y)*(DWORD)rx*(DWORD)rx/((DWORD)ry*(DWORD)ry));
	}
	switch(uPctX10/250)
	{
	case 1:
		y=-y;
		break;
	case 2:
		break;
	case 3:
		x=-x;
		break;
	default: 
		x=-x;
		y=-y;
		break;
	}
	x+=cx;
	y+=cy;	
	x=x<0?0:x;
	hEllipticRgn=CreateEllipticRgnIndirect(&rcItem);
	OffsetRgn(hEllipticRgn,0,uOffset);
	hEllRect=CreateRectRgn(rcItem.left,cy,rcItem.right,cy+uOffset);
	hRectRgn=CreateRectRgn(0,0,0,0);
	CombineRgn(hRectRgn,hEllipticRgn,hEllRect,RGN_OR);
	OffsetRgn(hEllipticRgn,0,-(int)uOffset);
	CombineRgn(hEllRect,hRectRgn,hEllipticRgn,RGN_DIFF);
	hBrush=CreateSolidBrush(RGB(0,0,128));
	if(hBrush)
	{
		FillRgn(hDC,hEllRect,hBrush);
		DeleteObject(hBrush);
	}
	if(uPctX10>500&&(hBrush=CreateSolidBrush(RGB(128,0,128)))!=NULL)
	{
		DeleteObject(hRectRgn);
		hRectRgn=CreateRectRgn(x,cy,rcItem.right,lprcItem->bottom);
		CombineRgn(hEllipticRgn,hEllRect,hRectRgn,RGN_AND);
		FillRgn(hDC,hEllipticRgn,hBrush);
		DeleteObject(hBrush);
	}
	DeleteObject(hRectRgn);
	DeleteObject(hEllipticRgn);
	DeleteObject(hEllRect);
	hPen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_WINDOWFRAME));
	hOldPen=(HPEN)SelectObject(hDC,hPen);
	if((uPctX10<=7)&&(cy==y))
	{
		hBrush=CreateSolidBrush(RGB(0,0,255));
		hOldBrush=(HBRUSH)SelectObject(hDC,hBrush);
		Ellipse(hDC,rcItem.left,rcItem.top,rcItem.right,rcItem.bottom);
		if(uPctX10!=0)
		{
			MoveToEx(hDC,cx,cy,NULL);
			LineTo(hDC,rcItem.left,cy);
		}
	}
	else if((uPctX10>=993)&&(cy==y))
	{
		hBrush=CreateSolidBrush(RGB(255,0,255));
		hOldBrush=(HBRUSH)SelectObject(hDC,hBrush);
		Ellipse(hDC,rcItem.left,rcItem.top,rcItem.right,rcItem.bottom);
		if(uPctX10!=1000)
		{
			MoveToEx(hDC,cx,cy,NULL);
			LineTo(hDC,rcItem.left,cy);
		}
	}
	else
	{
		hBrush=CreateSolidBrush(RGB(255,0,255));
		hOldBrush=(HBRUSH)SelectObject(hDC,hBrush);
		Ellipse(hDC,rcItem.left,rcItem.top,rcItem.right,rcItem.bottom);
		SelectObject(hDC,hOldBrush);
		DeleteObject(hBrush);
		hBrush=CreateSolidBrush(RGB(0,0,255));
		hOldBrush=(HBRUSH)SelectObject(hDC,hBrush);
		Pie(hDC,rcItem.left,rcItem.top,rcItem.right,rcItem.bottom,rcItem.left,cy,x,y);
	}
	SelectObject(hDC,hOldBrush);
	DeleteObject(hBrush);
	Arc(hDC,rcItem.left,rcItem.top+uOffset-1,rcItem.right,rcItem.bottom+uOffset-1,rcItem.left,cy+uOffset,rcItem.right,cy+uOffset-1);
	MoveToEx(hDC,rcItem.left,cy,NULL);
	LineTo(hDC,rcItem.left,cy+uOffset);
	MoveToEx(hDC,rcItem.right-1,cy,NULL);
	LineTo(hDC,rcItem.right-1,cy+uOffset);
	if(uPctX10>500)
	{
		MoveToEx(hDC,x,y,NULL);
		LineTo(hDC,x,y+uOffset);
	}
	SelectObject(hDC,hOldPen);
	DeleteObject(hPen);
}
