#ifndef _FREECELL_PCH_
#define _FREECELL_PCH_

#include <windows.h>
#include <tchar.h>
#include <cardlib.h>

#include "resource.h"

#define NUM_TABLEAU       8
#define NUM_FREECELLS     4
#define NUM_FOUNDATIONS   4

#define FREECELL_ID       1
#define FOUNDATION_ID     10
#define TABLEAU_ID        20

#define X_BORDER           20
#define X_STACK_BORDER     12
#define Y_BORDER           20
#define Y_TABLEAU_BORDER   28

extern CardWindow FreeCellWnd;

extern CardRegion* pFreeCell[NUM_FREECELLS];
extern CardRegion* pFoundation[NUM_FOUNDATIONS];
extern CardRegion* pTableau[NUM_TABLEAU];

extern HWND hwndMain;
extern HWND hwndStatus;
extern HMENU hGameMenu;
extern HINSTANCE hInstance;

extern TCHAR szAppName[128];
extern TCHAR MsgAbout[256];
extern TCHAR MsgQuit[256];
extern TCHAR MsgWin[256];

extern bool fGameStarted;
extern int LastId;

void CreateFreeCell(void);
void NewGame(void);

bool IsValidSequence(const CardStack& cards);
bool CanMoveToTableau(CardRegion& destination, const CardStack& cards);
bool CanMoveToFoundation(CardRegion& destination, const CardStack& cards);
bool CanMoveToFreeCell(CardRegion& destination, const CardStack& cards);
bool CanMoveCards(CardRegion& source,
                  CardRegion& destination,
                  int numberOfCards);

int CountEmptyFreeCells(void);
int CountEmptyTableauColumns(void);
int GetMaximumMovableCards(CardRegion& source, CardRegion& destination);

void CARDLIBPROC FreeCellClickProc(CardRegion& stackobj, int iNumClicked);
bool CARDLIBPROC TableauDragProc(CardRegion& stackobj, int iNumCards);
bool CARDLIBPROC TableauDropProc(CardRegion& stackobj, CardStack& dragcards);

bool CARDLIBPROC FreeCellDropProc(CardRegion& stackobj, CardStack& dragcards);
bool CARDLIBPROC FoundationDropProc(CardRegion& stackobj, CardStack& dragcards);

void CARDLIBPROC TableauDblClickProc(CardRegion& stackobj, int iNumClicked);
void CARDLIBPROC FreeCellDblClickProc(CardRegion& stackobj, int iNumClicked);

void CheckForWin(void);

LRESULT CALLBACK WndProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam);

#endif
