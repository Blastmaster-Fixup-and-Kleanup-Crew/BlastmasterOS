#include "freecell.h"

CardRegion* pFreeCell[NUM_FREECELLS];
CardRegion* pFoundation[NUM_FOUNDATIONS];
CardRegion* pTableau[NUM_TABLEAU];

void CreateFreeCell(void)
{
    int i;

    /*
     * Four free cells at the top-left.
     */
    for (i = 0; i < NUM_FREECELLS; i++)
    {
        pFreeCell[i] = FreeCellWnd.CreateRegion(
            FREECELL_ID + i,
            true,
            X_BORDER + i * (__cardwidth + X_STACK_BORDER),
            Y_BORDER,
            0,
            0);

        pFreeCell[i]->SetEmptyImage(CS_EI_SUNK);
        pFreeCell[i]->SetDragRule(CS_DRAG_TOP);
        pFreeCell[i]->SetDropRule(CS_DROP_CALLBACK, FreeCellDropProc);
        pFreeCell[i]->SetClickProc(FreeCellClickProc);
        pFreeCell[i]->SetDblClickProc(FreeCellDblClickProc);
    }

    /*
     * Four foundation piles at the top-right.
     */
    for (i = 0; i < NUM_FOUNDATIONS; i++)
    {
        pFoundation[i] = FreeCellWnd.CreateRegion(
            FOUNDATION_ID + i,
            true,
            0,
            Y_BORDER,
            0,
            0);

        pFoundation[i]->SetEmptyImage(CS_EI_SUNK);

        pFoundation[i]->SetPlacement(
            CS_XJUST_CENTER,
            0,
            (NUM_FREECELLS + 1 + i) *
                (__cardwidth + X_STACK_BORDER),
            0);

        pFoundation[i]->SetDragRule(CS_DRAG_TOP);
        pFoundation[i]->SetDropRule(
            CS_DROP_CALLBACK,
            FoundationDropProc);

        pFoundation[i]->SetClickProc(FreeCellClickProc);
    }

    /*
     * Eight tableau columns.
     */
    for (i = 0; i < NUM_TABLEAU; i++)
    {
        pTableau[i] = FreeCellWnd.CreateRegion(
            TABLEAU_ID + i,
            true,
            0,
            Y_BORDER + __cardheight + Y_TABLEAU_BORDER,
            0,
            (int)(__cardheight / 5));

        pTableau[i]->SetEmptyImage(CS_EI_SUNK);

        pTableau[i]->SetPlacement(
            CS_XJUST_CENTER,
            0,
            (i - NUM_TABLEAU / 2) *
                (__cardwidth + X_STACK_BORDER),
            0);

        pTableau[i]->SetFaceDirection(CS_FACE_UP, 0);

        pTableau[i]->SetDragRule(
            CS_DRAG_CALLBACK,
            TableauDragProc);

        pTableau[i]->SetDropRule(
            CS_DROP_CALLBACK,
            TableauDropProc);

        pTableau[i]->SetClickProc(FreeCellClickProc);
        pTableau[i]->SetDblClickProc(TableauDblClickProc);
    }
}
