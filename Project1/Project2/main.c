#include <stdio.h>

int main(void) {
    int n = 5;       // 정사각형 크기
    char buf[100];   // 충분히 큰 버퍼
    int r = 0, c, p = 0;

    while (r < n) {
        c = 0;
        while (c < n) {
            if (r == 0 || r == n - 1 || c == 0 || c == n - 1)
                buf[p++] = '#';   // 바로 '#' 사용
            else
                buf[p++] = ' ';
            c++;
        }
        buf[p++] = '\n';
        r++;
    }
    buf[p] = '\0';

    printf("%s", buf); // printf는 한 번만 사용
    return 0;
}
