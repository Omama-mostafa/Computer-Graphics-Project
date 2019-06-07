#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <stack>
#include <fstream>
#include "menu.h"
//
//#define NUM    1000
//#define TWOPI  (2 * 3.14159)
//
//#ifdef _APPLE_
//#include <GLUT/glut.h>
//#else
//#include <GL/glut.h>
//#endif

using namespace std;
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[ ] = "CodeBlocksWindowsApp";

bool flag1 = false, flag2 = false, flag3 = false, flag4 = false, flag5 = false;
int x1, y, x2, y2, x3, y3, x4, y4,xc, yc, A, B;
int menuItemID;

bool drawParametricLine = false;
bool drawDDALine = false;
bool drawMidPointLine = false;
bool drawDirectEllipse = false;
bool drawPolarEllipse = false;
bool drawMidPointEllipse = false;
bool clippingPolygon = false;
bool drawBezierCurve = false;
bool drawHermiteCurve = false;
bool drawSplinesGuided = false;
bool drawSplinesPass = false;
bool cameraModel = false;

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(MENU_ID);                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;


    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               "Graphics App",       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               900,                 /* The programs width */
               900,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

struct savePoints
{
    int type;
    double x1 = 0;
    double y1 = 0;
    double x2 = 0;
    double y2 = 0;
    double x3 = 0;
    double y3 = 0;
    double x4 = 0;
    double y4 = 0;
    double xc = 0;
    double yc = 0;
    double A = 0;
    double B = 0;
};

vector<savePoints> saveInFile;
vector<savePoints> loadFromFile;

void Swap(int& xs, int& ys, int& xe, int& ye)
{
    int temp = xs;
    xs = xe;
    xe = temp;
    temp = ys;
    ys = ye;
    ye = temp;
}

int Round(double number)
{
    return (int)(number + 0.5);
}

void LineParametric(HDC hdc, int x1, int y1, int x2, int y2,COLORREF c)
{
    savePoints point;
    point.type = 1;
    point.x1 = x1;
    point.y1 = y1;
    point.x2 = x2;
    point.y2 = y2;
    saveInFile.push_back(point);

    int dx = x2 - x1;
    int dy = y2 - y1;
    double dt = 1.0 / (max(abs(dx), abs(dy))+1);

    for(double t = 0; t <= 1; t += dt)
    {
        int x = Round(x1 + t*dx);
        int y = Round(y1 + t*dy);
        SetPixel(hdc, x, y, c);
    }
}


void LineDDA (HDC hdc, int x1, int y1, int x2, int y2, COLORREF c)
{
    savePoints point;
    point.type = 2;
    point.x1 = x1;
    point.y1 = y1;
    point.x2 = x2;
    point.y2 = y2;
    saveInFile.push_back(point);

    int dx = x2 - x1;
    int dy = y2 - y1;
    double m = (double) dy/dx;

    if(abs(dx) >= abs(dy))
    {
        if(x1 > x2)
        {
            Swap(x1, y1, x2, y2);
        }

        int x = x1;
        double y = y1;
        SetPixel(hdc, x1, y1, c);

        while(x < x2)
        {
            x++;
            y += m;
            SetPixel(hdc, x, Round(y), c);
        }
    }

    else
    {
        if(y1 > y2)
        {
            Swap(x1, y1, x2, y2);
        }

        double x = x1;
        int y = y1;
        SetPixel(hdc, x1, y1, c);
        while(y < y2)
        {
            y++;
            x += 1/m;
            SetPixel(hdc, Round(x), y, c);
        }
    }
}


void LineMidpoint (HDC hdc, int x1, int y1, int x2, int y2, COLORREF c)
{
    savePoints point;
    point.type = 3;
    point.x1 = x1;
    point.y1 = y1;
    point.x2 = x2;
    point.y2 = y2;
    saveInFile.push_back(point);

    int dx = x2 - x1;
    int dy = y2 - y1;
    int d, d1, d2;

    if(dx == 0 && dy == 0)
    {
        SetPixel(hdc, x1, y1, c);
        return;
    }
    if (abs(dy) < abs(dx))
    {
        if (x1 > x2)
        {
            Swap(x1, y1, x2, y2);
            dy *= -1;
            dx *= -1;
        }
        int x = x1, y = y1;
        SetPixel(hdc, x, y, c);

        if (dy > 0) //f(x+1,y+0.5)
        {
            d = dx - 2 * dy;
            d1 = -2 * dy;
            d2 = 2 * (dx - dy);
        }
        else //f(x-1,y+0.5)
        {
            d = dx + 2 * dy;
            d1 = 2 * dy;
            d2 = 2 * (dx + dy);
        }

        while (x < x2)
        {
            x++;
            if (d > 0)
            {
                d += d1;
            }
            else
            {
                d += d2;

                if (dy > 0) //the line is /.
                    y++;
                else    //the line is \.
                    y--;
            }
            SetPixel(hdc, x, y, c);
        }
    }
    else if (abs(dx) < abs(dy))
    {
        if (y1 > y2)
        {
            Swap(x1, y1, x2, y2);
            dy *= -1;
            dx *= -1;
        }
        int x = x1, y = y1;
        SetPixel(hdc, x, y, c);

        if (dx > 0)		//f(x+0.5,y+1)
        {
            d = dy - 2 * dx;
            d1 = -2 * dx;
            d2 = 2 * (dy - dx);
        }
        else //f(x-0.5,y+1)
        {
            d = dy + 2 * dx;
            d1 = 2 * dx;
            d2 = 2 * (dy + dx);
        }

        while (y < y2)
        {
            y++;
            if (d > 0)
            {
                d += d1;
            }
            else
            {
                d += d2;
                if (dx > 0) // line  /.
                    x++;
                else	// line  \.
                    x--;
            }
            SetPixel(hdc, x, y, c);
        }
    }
}


void polarEllipse(HDC hdc, int xc, int yc, int A, int B, COLORREF c)
{
    savePoints point;
    point.type = 4;
    point.xc = xc;
    point.yc = yc;
    point.A = A;
    point.B = B;
    saveInFile.push_back(point);

    for(double theta = 0 ; theta<6.28 ; theta += 0.001)
    {
        int x = Round(xc + A * cos(theta));
        int y = Round(yc + B * sin(theta));
        SetPixel(hdc, x, y, c);
    }
}


void drawfourpoints(int xc, int yc, int x, int y, HDC hdc, COLORREF c)
{
    SetPixel(hdc,xc-x,yc-y,c);
    SetPixel(hdc,xc+x,yc-y,c);
    SetPixel(hdc,xc-x,yc+y,c);
    SetPixel(hdc,xc+x,yc+y,c);
}
void drawdirectellipse(int xc, int yc, int A, int B, HDC hdc, COLORREF c)
{
    savePoints point;
    point.type = 5;
    point.xc = xc;
    point.yc = yc;
    point.A = A;
    point.B = B;
    saveInFile.push_back(point);

    float x=A;
    int y=0;
    while(x>=0)
    {
        drawfourpoints(xc,yc,x,y,hdc,c);
        x-=0.01;
        y=B*sqrt(1-((x*x)/(A*A)));
    }
}

void drawmidpointellipse(int xc, int yc, int A, int B, HDC hdc, COLORREF c)
{
    savePoints point;
    point.type = 6;
    point.xc = xc;
    point.yc = yc;
    point.A = A;
    point.B = B;
    saveInFile.push_back(point);

    int p=B*B-A*A*B+A*A/4;
    int x=0,y=B;
    while(2.0*B*B*x <= 2.0*A*A*y)
    {
        if(p < 0)
        {
            x++;
            p = p+2*B*B*x+B*B;
        }
        else
        {
            x++;
            y--;
            p = p+2*B*B*x-2*A*A*y-B*B;
        }
        drawfourpoints(xc,yc,x,y,hdc,c);
    }

    p=B*B*(x+0.5)*(x+0.5)+A*A*(y-1)*(y-1)-A*A*B*B;
    while(y > 0)
    {
        if(p <= 0)
        {
            x++;
            y--;
            p = p+2*B*B*x-2*A*A*y+A*A;
        }
        else
        {
            y--;
            p = p-2*A*A*y+A*A;
        }
        drawfourpoints(xc,yc,x,y,hdc,c);
    }
}


void HermitCurve(HDC hdc, int x1, int y1, int x2, int y2,int x3, int y3, int x4, int y4, COLORREF color)
{
    savePoints point;
    point.type = 7;
    point.x1 = x1;
    point.y1 = y1;
    point.x2 = x2;
    point.y2 = y2;
    point.x3 = x3;
    point.y3 = y3;
    point.x4 = x4;
    point.y4 = y4;
    saveInFile.push_back(point);

    for (double t = 0; t < 1; t += 0.001)
    {
        double x = x1 + (t * double(x2)) + (t*t* double(-3*x1 - 2*x2 + 3*x3 -x4) + (t*t*t* double(2*x1 + x2 - 2*x3 + x4)));
        double y = y1 + (t * double(y2)) + (t*t* double(-3*y1 - 2*y2 + 3*y3 -y4) + (t*t*t* double(2*y1 + y2 - 2*y3 + y4)));
        SetPixel(hdc, Round(x), Round(y), color);
    }

}

void BezierCurve(HDC hdc, int x1, int y1, int x2, int y2,int x3, int y3, int x4, int y4, COLORREF color)
{
    savePoints point;
    point.type = 8;
    point.x1 = x1;
    point.y1 = y1;
    point.x2 = x2;
    point.y2 = y2;
    point.x3 = x3;
    point.y3 = y3;
    point.x4 = x4;
    point.y4 = y4;
    saveInFile.push_back(point);

    for (double t = 0; t < 1; t += 0.001)
    {
        double x = x1 + (t * double(-3*x1 + 3*x2)) + (t*t* double(3*x1 - 6*x2 + 3*x3) + (t*t*t* double(-x1 + 3*x2 - 3*x3 + x4)));
        double y = y1 + (t * double(-3*y1 + 3*y2)) + (t*t* double(3*y1 - 6*y2 + 3*y3) + (t*t*t* double(-y1 + 3*y2 - 3*y3 + y4)));
        SetPixel(hdc, Round(x), Round(y), color);
    }

}

void splinePasss(HDC hdc, int x1, int y1, int x2, int y2, int x3, int y3, COLORREF color)
{
    savePoints point;
    point.type = 9;
    point.x1 = x1;
    point.y1 = y1;
    point.x2 = x2;
    point.y2 = y2;
    point.x3 = x3;
    point.y3 = y3;
    saveInFile.push_back(point);

    for (double t = 0; t < 1; t += 0.001)
    {
        double x = x1 + (t*double(- 3 * x1 + 4 * x2 - x3)) + (t*t*double( 2 * x1 - 4 * x2 + 2 * x3 ));
        double y = y1 + (t*double(- 3 * y1 + 4 * y2 - y3)) + (t*t*double( 2 * y1 - 4 * y2 + 2 * y3 ));
        //cout<<x<<" "<<y<<endl;
        SetPixel(hdc, Round(x), Round(y), color);
    }

}

void splineguided(HDC hdc, int x1, int y1, int x2, int y2, int x3, int y3, COLORREF color)
{
    savePoints point;
    point.type = 10;
    point.x1 = x1;
    point.y1 = y1;
    point.x2 = x2;
    point.y2 = y2;
    point.x3 = x3;
    point.y3 = y3;
    saveInFile.push_back(point);

    for (double t = 0; t < 1; t += 0.001)
    {
        double x = x1 + (t*double(- 2* x1 + 2 * x2)) + (t*t*double( x1 - 2 * x2 + x3));
        double y = y1 + (t*double(- 2* y1 + 2 * y2)) + (t*t*double( y1 - 2 * y2 + y3));
        //cout<<x<<" "<<y<<endl;
        SetPixel(hdc, Round(x), Round(y), color);
    }
}

struct point
{
    double x;
    double y;
};
vector<point> vecOfPoints;
vector<point> polygonPoints;
vector<point> clippoly;
vector<point> tempClipPoly;

struct Vertex
{
    double x,y;
    Vertex(int x1=0,int y1=0)
    {
        x=x1;
        y=y1;
    }
};
typedef vector<Vertex> VertexList;
typedef bool (*IsInFunc)(Vertex& v,int edge);
typedef Vertex (*IntersectFunc)(Vertex& v1,Vertex& v2,int edge);

bool InLeft(Vertex& v,int edge)
{
    return v.x >= edge;
}
bool InRight(Vertex& v,int edge)
{
    return v.x <= edge;
}
bool InTop(Vertex& v,int edge)
{
    return v.y >= edge;
}
bool InBottom(Vertex& v,int edge)
{
    return v.y <= edge;
}
Vertex VIntersect(Vertex& v1,Vertex& v2,int xedge)
{
    Vertex res;
    res.x=xedge;
    res.y=v1.y+(xedge-v1.x)*(v2.y-v1.y)/(v2.x-v1.x);
    return res;
}
Vertex HIntersect(Vertex& v1,Vertex& v2,int yedge)
{
    Vertex res;
    res.y=yedge;
    res.x=v1.x+(yedge-v1.y)*(v2.x-v1.x)/(v2.y-v1.y);
    return res;
}
VertexList ClipWithEdge(VertexList p,int edge,IsInFunc In,IntersectFunc Intersect)
{
    VertexList OutList;
    Vertex v1=p[p.size()-1];
    bool v1_in=In(v1,edge);
    for(int i=0; i<(int)p.size(); i++)
    {
        Vertex v2=p[i];
        bool v2_in=In(v2,edge);
        if(!v1_in && v2_in)
        {
            OutList.push_back(Intersect(v1,v2,edge));
            OutList.push_back(v2);
        }
        else if(v1_in && v2_in)
            OutList.push_back(v2);
        else if(v1_in)
            OutList.push_back(Intersect(v1,v2,edge));
        v1=v2;
        v1_in=v2_in;
    }
    return OutList;
}
void PolygonClip(vector<point>p,int n,int xleft,int ytop,int xright,int ybottom, HDC hdc)
{
//    savePoints point;
//    point.type = 11;
//    point.x1 = xleft;
//    point.y1 = ytop;
//    point.x2 = xright;
//    point.y2 = ybottom;
//    saveInFile.push_back(point);

    COLORREF color = RGB(0,0,0);
    VertexList vlist;
    for(int i = 0; i < n; i++)
    {
        vlist.push_back(Vertex(p[i].x,p[i].y));
    }
    vlist=ClipWithEdge(vlist, xleft, InLeft, VIntersect);
    Vertex v1 = vlist[vlist.size()-1];
    for(int i = 0; i < (int)vlist.size(); i++)
    {
        Vertex v2 = vlist[i];
        LineDDA( hdc, v1.x, v1.y, v2.x, v2.y,color);
        v1 = v2;
    }
    HWND hwnd;
    ReleaseDC(hwnd , hdc);
    hdc= GetDC(hwnd);
    Sleep(100);
    vlist=ClipWithEdge(vlist, ytop, InTop, HIntersect);
    v1 = vlist[vlist.size()-1];
    for(int i = 0; i < (int)vlist.size(); i++)
    {
        Vertex v2 = vlist[i];
        LineDDA( hdc, v1.x, v1.y, v2.x, v2.y,color);
        v1 = v2;
    }
    Sleep(10);
    vlist=ClipWithEdge(vlist, xright, InRight, VIntersect);
    v1 = vlist[vlist.size()-1];
    for(int i = 0; i < (int)vlist.size(); i++)
    {
        Vertex v2 = vlist[i];
        LineDDA( hdc, v1.x, v1.y, v2.x, v2.y,color);
        v1 = v2;
    }
    Sleep(10);
    vlist = ClipWithEdge(vlist, ybottom, InBottom, HIntersect);
    v1 = vlist[vlist.size()-1];
    for(int i = 0; i < (int)vlist.size(); i++)
    {
        Vertex v2 = vlist[i];
        LineDDA( hdc, v1.x, v1.y, v2.x, v2.y,color);
        v1 = v2;
    }
}


void DrawPolygon(HDC hdc,int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, COLORREF color)
{
    savePoints points;
    points.type = 11;
    points.x1 = x1;
    points.y1 = y1;
    points.x2 = x2;
    points.y2 = y2;
    points.x3 = x3;
    points.y3 = y3;
    points.x4 = x4;
    points.y4 = y4;
    saveInFile.push_back(points);

    point thePoints;
    thePoints.x = x1;
    thePoints.y = y1;
    polygonPoints.push_back(thePoints);

    thePoints.x = x2;
    thePoints.y = y2;
    polygonPoints.push_back(thePoints);

    thePoints.x = x3;
    thePoints.y = y3;
    polygonPoints.push_back(thePoints);

    thePoints.x = x4;
    thePoints.y = y4;
    polygonPoints.push_back(thePoints);

    LineDDA(hdc, x1, y1, x2, y2, color);
    LineDDA(hdc, x2, y2, x3, y3, color);
    LineDDA(hdc, x3, y3, x4, y4, color);
    LineDDA(hdc, x4, y4, x1, y1, color);
}

void SaveToFile()
{
    ofstream File;
    File.open("Points.txt", ios::trunc);
    for(int i = 0; i < saveInFile.size(); i++)
    {
        File.write((char*)&saveInFile[i], sizeof(saveInFile[i]));
    }
    File.close();
}

void LoadFromFile(HDC hdc, COLORREF c, HWND hwnd)
{
    ifstream File;
    File.open("Points.txt");
    File.seekg(0, ios::beg);
    while(!File.eof())
    {
        savePoints point;
        File.read((char*)&point, sizeof(point));
        cout << point.x1 << " ";
        cout << point.y1 << " ";
        cout << point.x2 << " ";
        cout << point.y2 << " ";
        cout << point.x3 << " ";
        cout << point.y3 << " ";
        cout << point.x4 << " ";
        cout << point.y4 << endl;
        loadFromFile.push_back(point);
    }
    File.close();

    for(int i = 0; i < loadFromFile.size(); i++)
    {
        if(loadFromFile[i].type == 1)
        {
            LineParametric(hdc, loadFromFile[i].x1, loadFromFile[i].y1,
                           loadFromFile[i].x2, loadFromFile[i].y2, c);
        }
        if(loadFromFile[i].type == 2)
        {
            LineDDA(hdc, loadFromFile[i].x1, loadFromFile[i].y1,
                    loadFromFile[i].x2, loadFromFile[i].y2, c);
        }
        if(loadFromFile[i].type == 3)
        {
            LineMidpoint(hdc, loadFromFile[i].x1, loadFromFile[i].y1,
                         loadFromFile[i].x2, loadFromFile[i].y2, c);
        }
        if(loadFromFile[i].type == 4)
        {
            polarEllipse(hdc, loadFromFile[i].xc, loadFromFile[i].yc,
                         loadFromFile[i].A, loadFromFile[i].B, c);
        }
        if(loadFromFile[i].type == 5)
        {
            drawdirectellipse(loadFromFile[i].xc, loadFromFile[i].yc,
                              loadFromFile[i].A, loadFromFile[i].B, hdc, c);
        }
        if(loadFromFile[i].type == 6)
        {
            drawmidpointellipse(loadFromFile[i].xc, loadFromFile[i].yc,
                                loadFromFile[i].A, loadFromFile[i].B, hdc, c);
        }
        if(loadFromFile[i].type == 7)
        {
            HermitCurve(hdc, loadFromFile[i].x1, loadFromFile[i].y1,
                        loadFromFile[i].x2, loadFromFile[i].y2,
                        loadFromFile[i].x3, loadFromFile[i].y3,
                        loadFromFile[i].x4, loadFromFile[i].y4, c);
        }
        if(loadFromFile[i].type == 8)
        {
            BezierCurve(hdc, loadFromFile[i].x1, loadFromFile[i].y1,
                        loadFromFile[i].x2, loadFromFile[i].y2,
                        loadFromFile[i].x3, loadFromFile[i].y3,
                        loadFromFile[i].x4, loadFromFile[i].y4, c);
        }
        if(loadFromFile[i].type == 9)
        {
            splinePasss(hdc, loadFromFile[i].x1, loadFromFile[i].y1,
                        loadFromFile[i].x2, loadFromFile[i].y2,
                        loadFromFile[i].x3, loadFromFile[i].y3, c);
        }
        if(loadFromFile[i].type == 10)
        {
            splineguided(hdc, loadFromFile[i].x1, loadFromFile[i].y1,
                         loadFromFile[i].x2, loadFromFile[i].y2,
                         loadFromFile[i].x3, loadFromFile[i].y3, c);
        }
        if(loadFromFile[i].type == 11)
        {
            //DrawPolygon(hdc, 300, 300, 500, 300, 500, 400, 300, 400, (0,0,0));

            vector<point> tempPoly;
            point p;
            p.x = loadFromFile[i].x1;
            p.y = loadFromFile[i].y1;
            tempPoly.push_back(p);
            p.x = loadFromFile[i].x2;
            p.y = loadFromFile[i].y2;
            tempPoly.push_back(p);
            p.x = loadFromFile[i].x3;
            p.y = loadFromFile[i].y3;
            tempPoly.push_back(p);
            p.x = loadFromFile[i].x4;
            p.y = loadFromFile[i].y4;
            tempPoly.push_back(p);
            //PolygonClip(tempPoly,tempPoly.size(),300,300,500,400,  hdc);
            DrawPolygon(hdc, loadFromFile[i].x1, loadFromFile[i].y1,
                        loadFromFile[i].x2, loadFromFile[i].y2,
                        loadFromFile[i].x3, loadFromFile[i].y3,
                        loadFromFile[i].x4, loadFromFile[i].y4,c);
        }
    }
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT p;
    COLORREF color = RGB(0, 0, 255);
    HDC hdc;

    switch (message)                  /* handle the messages */
    {
    case WM_DESTROY:
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        break;

    case WM_COMMAND:
        menuItemID = LOWORD(wParam);
        switch(menuItemID)
        {
        case EXIT_ID:
            PostQuitMessage(0);
            break;
        case PARAMETRIC_LINE_ID:
            drawParametricLine = true;
            drawDDALine = false;
            drawMidPointLine = false;
            drawDirectEllipse = false;
            drawPolarEllipse = false;
            drawMidPointEllipse = false;
            clippingPolygon = false;
            drawBezierCurve = false;
            drawHermiteCurve = false;
            drawSplinesGuided = false;
            drawSplinesPass = false;
            cameraModel = false;
            break;
        case DDA_LINE_ID:
            drawParametricLine = false;
            drawDDALine = true;
            drawMidPointLine = false;
            drawDirectEllipse = false;
            drawPolarEllipse = false;
            drawMidPointEllipse = false;
            clippingPolygon = false;
            drawBezierCurve = false;
            drawHermiteCurve = false;
            drawSplinesGuided = false;
            drawSplinesPass = false;
            cameraModel = false;
            break;
        case MIDPOINT_LINE_ID:
            drawParametricLine = false;
            drawDDALine = false;
            drawMidPointLine = true;
            drawDirectEllipse = false;
            drawPolarEllipse = false;
            drawMidPointEllipse = false;
            clippingPolygon = false;
            drawBezierCurve = false;
            drawHermiteCurve = false;
            drawSplinesGuided = false;
            drawSplinesPass = false;
            cameraModel = false;
            break;
        case DIRECT_ELLIPSE_ID:
            drawParametricLine = false;
            drawDDALine = false;
            drawMidPointLine = false;
            drawDirectEllipse = true;
            drawPolarEllipse = false;
            drawMidPointEllipse = false;
            clippingPolygon = false;
            drawBezierCurve = false;
            drawHermiteCurve = false;
            drawSplinesGuided = false;
            drawSplinesPass = false;
            cameraModel = false;
            break;
        case POLAR_ELLIPSE_ID:
            drawParametricLine = false;
            drawDDALine = false;
            drawMidPointLine = false;
            drawDirectEllipse = false;
            drawPolarEllipse = true;
            drawMidPointEllipse = false;
            clippingPolygon = false;
            drawBezierCurve = false;
            drawHermiteCurve = false;
            drawSplinesGuided = false;
            drawSplinesPass = false;
            cameraModel = false;
            break;
        case MIDPOINT_ELLIPSE_ID:
            drawParametricLine = false;
            drawDDALine = false;
            drawMidPointLine = false;
            drawDirectEllipse = false;
            drawPolarEllipse = false;
            drawMidPointEllipse = true;
            clippingPolygon = false;
            drawBezierCurve = false;
            drawHermiteCurve = false;
            drawSplinesGuided = false;
            drawSplinesPass = false;
            cameraModel = false;
            break;
        case POLYGON_CLIPPING_ID:
            drawParametricLine = false;
            drawDDALine = false;
            drawMidPointLine = false;
            drawDirectEllipse = false;
            drawPolarEllipse = false;
            drawMidPointEllipse = false;
            clippingPolygon = true;
            drawBezierCurve = false;
            drawHermiteCurve = false;
            drawSplinesGuided = false;
            drawSplinesPass = false;
            cameraModel = false;
            break;
        case BEZIER_CUBE_CURVE_ID:
            drawParametricLine = false;
            drawDDALine = false;
            drawMidPointLine = false;
            drawDirectEllipse = false;
            drawPolarEllipse = false;
            drawMidPointEllipse = false;
            clippingPolygon = false;
            drawBezierCurve = true;
            drawHermiteCurve = false;
            drawSplinesGuided = false;
            drawSplinesPass = false;
            cameraModel = false;
            break;
        case HERMITE_CUBE_CURVE_ID:
            drawParametricLine = false;
            drawDDALine = false;
            drawMidPointLine = false;
            drawDirectEllipse = false;
            drawPolarEllipse = false;
            drawMidPointEllipse = false;
            clippingPolygon = false;
            drawBezierCurve = false;
            drawHermiteCurve = true;
            drawSplinesGuided = false;
            drawSplinesPass = false;
            cameraModel = false;
            break;
        case SPLINES_GUIDED_ID:
            drawParametricLine = false;
            drawDDALine = false;
            drawMidPointLine = false;
            drawDirectEllipse = false;
            drawPolarEllipse = false;
            drawMidPointEllipse = false;
            clippingPolygon = false;
            drawBezierCurve = false;
            drawHermiteCurve = false;
            drawSplinesGuided = true;
            drawSplinesPass = false;
            cameraModel = false;
            break;
        case SPLINES_PASS_ID:
            drawParametricLine = false;
            drawDDALine = false;
            drawMidPointLine = false;
            drawDirectEllipse = false;
            drawPolarEllipse = false;
            drawMidPointEllipse = false;
            clippingPolygon = false;
            drawBezierCurve = false;
            drawHermiteCurve = false;
            drawSplinesGuided = false;
            drawSplinesPass = true;
            cameraModel = false;
            break;
        case CAMERA_COORDINATE_ID:
            drawParametricLine = false;
            drawDDALine = false;
            drawMidPointLine = false;
            drawDirectEllipse = false;
            drawPolarEllipse = false;
            drawMidPointEllipse = false;
            clippingPolygon = false;
            drawBezierCurve = false;
            drawHermiteCurve = false;
            drawSplinesGuided = false;
            drawSplinesPass = false;
            cameraModel = true;
            break;
        case SAVE_ID:
            SaveToFile();
            break;
        case LOAD_ID:
            InvalidateRect(hwnd, NULL, TRUE);
            hdc = BeginPaint(hwnd,&p);
            LoadFromFile(hdc, color, hwnd);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    case WM_PAINT:
        if(drawParametricLine && flag1 && flag2)
        {
            hdc = BeginPaint(hwnd, &p);
            LineParametric(hdc, x1, y, x2, y2, color);
            EndPaint(hwnd, &p);
            flag1 = false;
            flag2 = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if(drawDDALine && flag1 && flag2)
        {
            hdc = BeginPaint(hwnd, &p);
            LineDDA(hdc, x1, y, x2, y2, color);
            EndPaint(hwnd, &p);
            flag1 = false;
            flag2 = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if(drawMidPointLine && flag1 && flag2)
        {
            hdc = BeginPaint(hwnd, &p);
            LineMidpoint(hdc, x1, y, x2, y2, color);
            EndPaint(hwnd, &p);
            flag1 = false;
            flag2 = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if(drawDirectEllipse && flag1 && flag2 && flag5)
        {
            hdc = BeginPaint(hwnd, &p);
            A = sqrt(abs(((x1 - xc) * (x1 - xc)) - ((y - yc) * (y - yc))));
            B = sqrt(abs(((x2 - xc) * (x2 - xc)) - ((y2 - yc) * (y2 - yc))));
            drawdirectellipse(xc, yc, A, B, hdc, color);
            EndPaint(hwnd, &p);
            flag1 = false;
            flag2 = false;
            flag5 = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if(drawPolarEllipse && flag1 && flag2 && flag5)
        {
            hdc = BeginPaint(hwnd, &p);
            A = sqrt(abs(((x1 - xc) * (x1 - xc)) - ((y - yc) * (y - yc))));
            B = sqrt(abs(((x2 - xc) * (x2 - xc)) - ((y2 - yc) * (y2 - yc))));
            polarEllipse(hdc, xc, yc, A, B, color);
            EndPaint(hwnd, &p);
            flag1 = false;
            flag2 = false;
            flag5 = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if(drawMidPointEllipse && flag1 && flag2 && flag5)
        {
            hdc = BeginPaint(hwnd, &p);
            A = sqrt(abs(((x1 - xc) * (x1 - xc)) - ((y - yc) * (y - yc))));
            B = sqrt(abs(((x2 - xc) * (x2 - xc)) - ((y2 - yc) * (y2 - yc))));
            drawmidpointellipse(xc, yc, A, B, hdc, color);
            EndPaint(hwnd, &p);
            flag1 = false;
            flag2 = false;
            flag5 = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if(drawBezierCurve && flag1 && flag2 && flag3 && flag4)
        {
            hdc = BeginPaint(hwnd, &p);
            BezierCurve(hdc, x1, y, x2, y2,
                        x3, y3, x4, y4, color);
            EndPaint(hwnd, &p);
            flag1 = false;
            flag2 = false;
            flag3 = false;
            flag4 = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if(drawHermiteCurve && flag1 && flag2 && flag3 && flag4)
        {
            hdc = BeginPaint(hwnd, &p);
            BezierCurve(hdc, x1, y, x2, y2,
                        x3, y3, x4, y4, color);
            EndPaint(hwnd, &p);
            flag1 = false;
            flag2 = false;
            flag3 = false;
            flag4 = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if(drawSplinesGuided && flag1 && flag2 && flag3)
        {
            hdc = BeginPaint(hwnd, &p);
            splineguided(hdc, x1, y, x2, y2,
                         x3, y3, color);
            EndPaint(hwnd, &p);
            flag1 = false;
            flag2 = false;
            flag3 = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if(drawSplinesPass && flag1 && flag2 && flag3)
        {
            hdc = BeginPaint(hwnd, &p);
            splinePasss(hdc, x1, y, x2, y2,
                        x3, y3, color);
            EndPaint(hwnd, &p);
            flag1 = false;
            flag2 = false;
            flag3 = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if(clippingPolygon)
        {
            hdc = BeginPaint(hwnd, &p);
            DrawPolygon(hdc, 300, 300, 500, 300, 500, 400, 300, 400, (0,0,0));
            EndPaint(hwnd, &p);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if(clippingPolygon && flag1 && flag2 && flag3 && flag4)
        {
            hdc = BeginPaint(hwnd, &p);
            PolygonClip(clippoly,clippoly.size(),300,300,500,400,  hdc);
//            for(int i=0 ; i<clippoly.size() ; i++)
//            {
//                tempClipPoly.push_back(clippoly[i]);
//            }
            EndPaint(hwnd, &p);
            flag1 = false;
            flag2 = false;
            flag3 = false;
            flag4 = false;
            clippoly.clear();
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;

    case WM_LBUTTONDOWN:
        if(!flag1 && clippingPolygon)
        {
            x1 = LOWORD(lParam);
            y = HIWORD(lParam);
            point p;
            p.x = x1;
            p.y = y;
            clippoly.push_back(p);
            flag1 = true;
        }
        else if(!flag1)
        {
            x1 = LOWORD(lParam);
            y = HIWORD(lParam);
            flag1=true;

        }
        else if(!flag2 && clippingPolygon)
        {
            x2 = LOWORD(lParam);
            y2 = HIWORD(lParam);
            point p;
            p.x = x2;
            p.y = y2;
            clippoly.push_back(p);
            flag2 = true;
        }
        else if(!flag2)
        {

            x2 = LOWORD(lParam);
            y2 = HIWORD(lParam);
            flag2=true;
        }
        else if(!flag3 && clippingPolygon)
        {
            x3 = LOWORD(lParam);
            y3 = HIWORD(lParam);
            point p;
            p.x = x3;
            p.y = y3;
            clippoly.push_back(p);
            flag3 = true;
        }
        else if(!flag3)
        {

            x3 = LOWORD(lParam);
            y3 = HIWORD(lParam);
            flag3=true;
        }
        else if(!flag4 && clippingPolygon)
        {
            x4 = LOWORD(lParam);
            y4 = HIWORD(lParam);
            point p;
            p.x = x4;
            p.y = y4;
            clippoly.push_back(p);
            flag4 = true;
        }
        else if(!flag4)
        {
            x4 = LOWORD(lParam);
            y4 = HIWORD(lParam);
            flag4=true;
        }
        else if(!flag5)
        {
            xc = LOWORD(lParam);
            yc = HIWORD(lParam);
            flag5 = true;
        }
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
