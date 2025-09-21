// main.c
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define PI 3.14159226535897

// 배열/크기들은 컴파일 타임 상수(매크로)로 정의
#define MAX_STEPS 2000
#define ROWS 40
#define COLS 120

// x: column, y: row (ANSI: \x1b[row;colH)
void moveCursor(int x, int y)
{
    printf("\x1b[%d;%dH", y, x);
}

//매개변수 isExploded
//0: 폭발전 폭탄
//1: 폭발함
void printBomb(int isExploded)
{
    //폭탄은 항상 7칸
    if (isExploded)
    {
        printf("\x1b[A^^^^^^^");
        printf("\x1b[B\x1b[7D!!BAM!!");
        printf("\x1b[B\x1b[7D^^^^^^^");
    }
    else
        printf("(  b  )");
}

/* --- VT 모드 활성화 (Windows 콘솔에서 ANSI 이스케이프 사용 가능하도록) --- */
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

static void enable_vt_mode(void)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

static void clear_screen(void)
{
    printf("\x1b[2J");
    printf("\x1b[H");
    fflush(stdout);
}

int main()
{
    enable_vt_mode();

    // 시작 '*' 좌표 (문제에서 준 14,9 을 x=14, y=9 로 해석)
    const int start_x = 14; // column
    const int start_y = 9;  // row

    // 나선 경로 저장 (컴파일타임 상수 MAX_STEPS 사용)
    int xs[MAX_STEPS], ys[MAX_STEPS];
    int len = 0;

    xs[len] = start_x;
    ys[len] = start_y;
    len++;

    // 나선(spiral) 만들기: 위 -> 왼 -> 아래 -> 오른 (반시계), step_size 1,1,2,2,3,3...
    int dir = 0; // 0: up, 1: left, 2: down, 3: right
    int step_size = 1;
    int repeat = 0;

    while (len < MAX_STEPS - 1)
    {
        int dx = 0, dy = 0;
        if (dir == 0) { dx = 0; dy = -1; }    // up
        else if (dir == 1) { dx = -1; dy = 0; } // left
        else if (dir == 2) { dx = 0; dy = 1; }  // down
        else { dx = 1; dy = 0; }               // right

        for (int s = 0; s < step_size; ++s)
        {
            int newx = xs[len - 1] + dx;
            int newy = ys[len - 1] + dy;
            // 경계 벗어나면 나선 중단
            if (newx < 1 || newx > COLS || newy < 1 || newy > ROWS)
                goto spiral_done;
            xs[len] = newx;
            ys[len] = newy;
            len++;
            if (len >= MAX_STEPS - 1) break;
        }
        dir = (dir + 1) % 4;
        repeat++;
        if (repeat == 2) { repeat = 0; step_size++; }
    }

spiral_done:
    // 경로가 충분히 생성되지 않으면 종료
    if (len < 2) {
        return 0;
    }

    int bomb_idx = len - 1;
    int bomb_x = xs[bomb_idx];
    int bomb_y = ys[bomb_idx];

    // 화면 초기화
    clear_screen();

    // 심지 '#' 그리기 (첫 '*'과 마지막 폭탄 위치 제외)
    for (int i = 1; i < bomb_idx; ++i)
    {
        if (xs[i] >= 1 && xs[i] <= COLS && ys[i] >= 1 && ys[i] <= ROWS) {
            moveCursor(xs[i], ys[i]);
            printf("#"); // 규칙: 한 문자씩 출력
        }
    }

    // '*' 첫 위치 그리기
    moveCursor(xs[0], ys[0]);
    printf("*");
    fflush(stdout);

    // 폭탄(비폭발 모양) 출력:
    // printBomb는 폭탄 너비 7칸을 사용하므로, 출력 시작 위치는 bomb_x - 3 로 맞춘다.
    int bomb_print_x = bomb_x - 3;
    if (bomb_print_x < 1) bomb_print_x = 1;
    moveCursor(bomb_print_x, bomb_y);
    printBomb(0); // 폭발 전 모양 (printBomb 내부는 예외)

    fflush(stdout);

    // 애니메이션: '*'이 심지를 따라 이동 -> 이전 위치와 '#'을 지우고 '*' 찍기
    for (int i = 0; i < bomb_idx; ++i)
    {
        Sleep(200);

        // 이전 '*' 위치 지우기
        if (xs[i] >= 1 && xs[i] <= COLS && ys[i] >= 1 && ys[i] <= ROWS) {
            moveCursor(xs[i], ys[i]);
            printf(" "); // 규칙: 한 문자씩 출력
        }

        // 다음 위치에 있던 '#'을 지우기 (불이 탄다)
        if (xs[i + 1] >= 1 && xs[i + 1] <= COLS && ys[i + 1] >= 1 && ys[i + 1] <= ROWS) {
            moveCursor(xs[i + 1], ys[i + 1]);
            printf(" ");
        }

        // '*'을 다음 위치에 찍기
        if (xs[i + 1] >= 1 && xs[i + 1] <= COLS && ys[i + 1] >= 1 && ys[i + 1] <= ROWS) {
            moveCursor(xs[i + 1], ys[i + 1]);
            printf("*");
        }
        fflush(stdout);
    }

    // '*'이 폭탄에 도달 -> 잠시 대기 후 폭발
    Sleep(200);
    // 폭발 표시를 위해 폭탄 출력 시작 위치로 이동
    moveCursor(bomb_print_x, bomb_y);
    printBomb(1);

    // 커서를 안전한 위치로 옮김
    moveCursor(1, ROWS + 1);
    fflush(stdout);

    return 0;
}
