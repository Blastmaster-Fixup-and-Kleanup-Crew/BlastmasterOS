#include "freecell.h"

bool fGameStarted = false;
int LastId = -1;

static bool IsRedCard(const Card& card)
{
    return card.IsRed();
}

static bool IsOppositeColor(const Card& first, const Card& second)
{
    return IsRedCard(first) != IsRedCard(second);
}

bool IsValidSequence(const CardStack& cards)
{
    int i;

    if (cards.NumCards() <= 1)
        return true;

    for (i = 0; i < cards.NumCards() - 1; i++)
    {
        const Card& upper = cards[i];
        const Card& lower = cards[i + 1];

        /*
         * A valid FreeCell sequence descends by one rank
         * and alternates colors.
         */
        if (upper.LoVal() != lower.LoVal() + 1)
            return false;

        if (!IsOppositeColor(upper, lower))
            return false;
    }

    return true;
}

int CountEmptyFreeCells(void)
{
    int i;
    int count = 0;

    for (i = 0; i < NUM_FREECELLS; i++)
    {
        if (pFreeCell[i]->NumCards() == 0)
            count++;
    }

    return count;
}

int CountEmptyTableauColumns(void)
{
    int i;
    int count = 0;

    for (i = 0; i < NUM_TABLEAU; i++)
    {
        if (pTableau[i]->NumCards() == 0)
            count++;
    }

    return count;
}

int GetMaximumMovableCards(
    CardRegion& source,
    CardRegion& destination)
{
    int emptyFreeCells;
    int emptyTableauColumns;

    emptyFreeCells = CountEmptyFreeCells();
    emptyTableauColumns = CountEmptyTableauColumns();

    /*
     * The destination column must not count as an empty
     * tableau column when it is empty.
     */
    if (destination.NumCards() == 0 &&
        destination.Id() >= TABLEAU_ID &&
        destination.Id() < TABLEAU_ID + NUM_TABLEAU)
    {
        if (emptyTableauColumns > 0)
            emptyTableauColumns--;
    }

    /*
     * A free cell source does not affect the number of
     * available free cells after removing its card.
     */
    if (source.Id() >= FREECELL_ID &&
        source.Id() < FREECELL_ID + NUM_FREECELLS)
    {
        emptyFreeCells++;
    }

    return (emptyFreeCells + 1) * (emptyTableauColumns + 1);
}

bool CanMoveToTableau(
    CardRegion& destination,
    const CardStack& cards)
{
    Card movingCard;

    if (cards.NumCards() == 0)
        return false;

    if (!IsValidSequence(cards))
        return false;

    movingCard = cards[cards.NumCards() - 1];

    if (destination.NumCards() == 0)
        return true;

    {
        const CardStack& destinationCards =
            destination.GetCardStack();

        const Card& destinationCard = destinationCards[0];

        if (destinationCard.LoVal() != movingCard.LoVal() + 1)
            return false;

        if (!IsOppositeColor(destinationCard, movingCard))
            return false;
    }

    return true;
}

bool CanMoveToFreeCell(
    CardRegion& destination,
    const CardStack& cards)
{
    if (destination.NumCards() != 0)
        return false;

    if (cards.NumCards() != 1)
        return false;

    return true;
}

bool CanMoveToFoundation(
    CardRegion& destination,
    const CardStack& cards)
{
    Card movingCard;
    int expectedValue;

    if (cards.NumCards() != 1)
        return false;

    movingCard = cards[0];

    if (destination.NumCards() == 0)
    {
        return movingCard.LoVal() == 1;
    }

    {
        const CardStack& foundation =
            destination.GetCardStack();

        const Card& topCard = foundation[0];

        if (topCard.Suit() != movingCard.Suit())
            return false;

        expectedValue = topCard.LoVal() + 1;
    }

    return movingCard.LoVal() == expectedValue;
}

bool CanMoveCards(
    CardRegion& source,
    CardRegion& destination,
    int numberOfCards)
{
    CardStack cards;
    int maximumCards;

    if (numberOfCards <= 0)
        return false;

    if (source.NumCards() < numberOfCards)
        return false;

    cards = source.GetCardStack().Top(numberOfCards);

    if (destination.Id() >= TABLEAU_ID &&
        destination.Id() < TABLEAU_ID + NUM_TABLEAU)
    {
        maximumCards =
            GetMaximumMovableCards(source, destination);

        if (numberOfCards > maximumCards)
            return false;

        return CanMoveToTableau(destination, cards);
    }

    if (destination.Id() >= FREECELL_ID &&
        destination.Id() < FREECELL_ID + NUM_FREECELLS)
    {
        return CanMoveToFreeCell(destination, cards);
    }

    if (destination.Id() >= FOUNDATION_ID &&
        destination.Id() < FOUNDATION_ID + NUM_FOUNDATIONS)
    {
        return CanMoveToFoundation(destination, cards);
    }

    return false;
}

void NewGame(void)
{
    CardStack deck;
    CardStack card;
    int i;
    int column;

    FreeCellWnd.EmptyStacks();

    deck.NewDeck();
    deck.Shuffle();

    /*
     * Deal seven cards to columns 0-3 and six cards
     * to columns 4-7.
     */
    for (column = 0; column < NUM_TABLEAU; column++)
    {
        int cardsToDeal;

        cardsToDeal = (column < 4) ? 7 : 6;
        card.Clear();

        for (i = 0; i < cardsToDeal; i++)
            card.Push(deck.Pop());

        pTableau[column]->SetFaceDirection(CS_FACE_UP, 0);
        pTableau[column]->SetCardStack(card);
    }

    fGameStarted = false;
    LastId = -1;

    FreeCellWnd.Redraw();
}

void CARDLIBPROC FreeCellClickProc(
    CardRegion& stackobj,
    int iNumClicked)
{
    LastId = stackobj.Id();
    fGameStarted = true;
}

bool CARDLIBPROC TableauDragProc(
    CardRegion& stackobj,
    int iNumCards)
{
    CardStack cards;
    int maximumCards;

    if (iNumCards <= 0)
        return false;

    if (iNumCards > stackobj.NumCards())
        return false;

    cards = stackobj.GetCardStack().Top(iNumCards);

    if (!IsValidSequence(cards))
        return false;

    /*
     * There is no destination available here, so apply
     * the basic source-side validation. The final limit
     * is checked again by TableauDropProc.
     */
    maximumCards =
        (CountEmptyFreeCells() + 1) *
        (CountEmptyTableauColumns() + 1);

    if (iNumCards > maximumCards)
        return false;

    return true;
}

bool CARDLIBPROC TableauDropProc(
    CardRegion& stackobj,
    CardStack& dragcards)
{
    if (LastId == stackobj.Id())
        return false;

    if (!CanMoveCards(
            *FreeCellWnd.GetRegion(LastId),
            stackobj,
            dragcards.NumCards()))
    {
        return false;
    }

    fGameStarted = true;
    FreeCellWnd.Redraw();

    return true;
}

bool CARDLIBPROC FreeCellDropProc(
    CardRegion& stackobj,
    CardStack& dragcards)
{
    CardRegion* source;

    if (LastId == stackobj.Id())
        return false;

    source = FreeCellWnd.GetRegion(LastId);

    if (source == NULL)
        return false;

    if (!CanMoveCards(source, stackobj, dragcards.NumCards()))
        return false;

    fGameStarted = true;
    FreeCellWnd.Redraw();

    return true;
}

bool CARDLIBPROC FoundationDropProc(
    CardRegion& stackobj,
    CardStack& dragcards)
{
    CardRegion* source;

    if (LastId == stackobj.Id())
        return false;

    source = FreeCellWnd.GetRegion(LastId);

    if (source == NULL)
        return false;

    if (!CanMoveCards(source, stackobj, dragcards.NumCards()))
        return false;

    fGameStarted = true;
    FreeCellWnd.Redraw();

    CheckForWin();

    return true;
}

void CARDLIBPROC TableauDblClickProc(
    CardRegion& stackobj,
    int iNumClicked)
{
    int i;
    CardStack cards;

    if (iNumClicked != 1)
        return;

    if (stackobj.NumCards() == 0)
        return;

    cards = stackobj.GetCardStack().Top(1);

    for (i = 0; i < NUM_FOUNDATIONS; i++)
    {
        if (CanMoveToFoundation(*pFoundation[i], cards))
        {
            LastId = stackobj.Id();
            stackobj.SimulateDrag(pFoundation[i], 1, true);
            CheckForWin();
            return;
        }
    }
}

void CARDLIBPROC FreeCellDblClickProc(
    CardRegion& stackobj,
    int iNumClicked)
{
    int i;
    CardStack cards;

    if (iNumClicked != 1)
        return;

    if (stackobj.NumCards() == 0)
        return;

    cards = stackobj.GetCardStack().Top(1);

    for (i = 0; i < NUM_FOUNDATIONS; i++)
    {
        if (CanMoveToFoundation(*pFoundation[i], cards))
        {
            LastId = stackobj.Id();
            stackobj.SimulateDrag(pFoundation[i], 1, true);
            CheckForWin();
            return;
        }
    }
}

void CheckForWin(void)
{
    int i;

    for (i = 0; i < NUM_FOUNDATIONS; i++)
    {
        if (pFoundation[i]->NumCards() != 13)
            return;
    }

    fGameStarted = false;

    MessageBox(
        hwndMain,
        MsgWin,
        szAppName,
        MB_OK | MB_ICONINFORMATION);
}
