#include <stdio.h>

int main(void) {
    int n = 5;       // ���簢�� ũ��
    char buf[100];   // ����� ū ����
    int r = 0, c, p = 0;

    while (r < n) {
        c = 0;
        while (c < n) {
            if (r == 0 || r == n - 1 || c == 0 || c == n - 1)
                buf[p++] = '#';   // �ٷ� '#' ���
            else
                buf[p++] = ' ';
            c++;
        }
        buf[p++] = '\n';
        r++;
    }
    buf[p] = '\0';

    printf("%s", buf); // printf�� �� ���� ���
    return 0;
}
