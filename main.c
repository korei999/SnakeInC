#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define GoUp     0
#define GoRight  1
#define GoDown   2
#define GoLeft   3

//      ^
//      0
//  < 3 * 1 >
//      2
//      v

static int direction = 0;
static int length = 0;

struct section {
    int y;
    int x;
    struct section * next;
};

typedef struct section Piece;

void AddPiece(Piece * P);
Piece * MoveTail(Piece * P);
void MoveToFront(Piece ** P);
void RedrawBoard(Piece * P, int ay, int ax);
void DeleteSnake(Piece ** P);
void PrintList(Piece * P);
bool Collision(Piece * P);
void GenAppleCoords(Piece * P, int my, int mx, int * ay, int * ax);

void
AddPiece(Piece * P)
{
    Piece * last = P;

    while (last->next != NULL)
        last = last->next;

    Piece * new = malloc(sizeof(Piece));
    last->next = new;
    new->next = NULL;

    new->y = last->y;
    new->x = last->x;

    if (direction == GoUp) {
        new->y++;
    }
    else if (direction == GoRight) {
        new->x--;
    }
    else if (direction == GoDown) {
        new->y--;
    }
    else if (direction == GoLeft) {
        new->x++;
    }
    length++;
}

Piece *
MoveTail(Piece * P)
{
    Piece * last = P;

    while (last->next != NULL) {
        last = last->next;
    }

    if (direction == GoUp) {
        last->y = P->y - 1;
        last->x = P->x;
    }
    else if (direction == GoRight) {
        last->x = P->x + 1;
        last->y = P->y;
    }
    else if (direction == GoDown) {
        last->y = P->y + 1;
        last->x = P->x;
    }
    else if (direction == GoLeft) {
        last->x = P->x - 1;
        last->y = P->y;
    }

    return P;
}

void
MoveToFront(Piece ** P)
{
    if (*P == NULL || (*P)->next == NULL)
        return;

    Piece * preLast = NULL;
    Piece * last = *P;

    while (last->next != NULL) {
        preLast = last;
        last = last->next;
    }
    preLast->next = NULL;
    last->next = *P;
    *P = last;
}

void
RedrawBoard(Piece * P, int ay, int ax)
{
    Piece * tmp = P;
    while (tmp != NULL) {
        // mvprintw(0, 0, "y(%d)x(%d)\n", tmp->y, tmp->x);
        mvaddch(ay, ax, '@');
        mvaddch(tmp->y, tmp->x, '*');
        tmp = tmp->next;
    }
}

void
DeleteSnake(Piece ** P)
{
    Piece * current = *P;
    Piece * next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    *P = NULL;
}

void
PrintList(Piece * P)
{
    Piece * tmp = P;
    while (tmp->next != NULL) {
        printf("[%d,%d] ", tmp->y, tmp->x);
        tmp = tmp->next;
    }
    putchar('\n');
}

bool
Collision(Piece * P)
{
    Piece * tmp = P;
    while (tmp->next != NULL) {
        tmp = tmp->next;
        if (P->y == tmp->y && P->x == tmp->x)
            return false;
    }
    return true;
}

void
GenAppleCoords(Piece * P, int my, int mx, int * ay, int * ax)
{
    bool board[my][mx];
    Piece * tmp = P;
    
    while (tmp->next != NULL) {
        board[tmp->y][tmp->x] = false;
        tmp = tmp->next;
    }

    int y = 0, x = 0;
    while (1) {
        y = rand() % my; x = rand() % mx;
        if (!board[y][x])
            continue;
        else {
            *ay = y;
            *ax = x;
            break;
        }
    }
}

int
main(void)
{
    initscr(); 
    halfdelay(1);
    noecho();
    keypad(stdscr, true);
    curs_set(0);
    srand(time(NULL));

    int maxy, maxx;
    unsigned int ch;
    getmaxyx(stdscr, maxy, maxx);
    Piece * head = NULL;

    head = malloc(sizeof(Piece));
    head->y = rand() % maxy;
    head->x = rand() % maxx;
    head->next = NULL;
    mvaddch(head->y, head->x, '*');

    int ay, ax;
    GenAppleCoords(head, maxy, maxx, &ay, &ax);

    while ((ch = getch()) != 'Q') {
        clear();

        if (ch == KEY_UP && direction != GoDown)
            direction = GoUp;
        if (ch == KEY_RIGHT && direction != GoLeft)
            direction = GoRight;
        if (ch == KEY_DOWN && direction != GoUp)
            direction = GoDown;
        if (ch == KEY_LEFT && direction != GoRight)
            direction = GoLeft;

        head = MoveTail(head);
        MoveToFront(&head);

        // out of bounds checks
        if (head->y >= maxy)
            head->y = 0;
        if (head->y <= -1)
            head->y = maxy - 1;
        if (head->x >= maxx)
            head->x = 0;
        if (head->x <= -1)
            head->x = maxx - 1; 
        

        if (!Collision(head))
            break;

        if (head->y == ay && head->x == ax) {
            AddPiece(head);
            GenAppleCoords(head, maxy, maxx, &ay, &ax);
        }
        
        RedrawBoard(head, ay, ax);
        continue;
    }

    endwin();

    printf("Your score is %d.\nGit gud kid.\n", length);

    PrintList(head);
    DeleteSnake(&head);

    return 0;
}
