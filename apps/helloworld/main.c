#include <stdio.h>
#include <unistd.h>

//-----------------------------------------------------------------
// main:
//-----------------------------------------------------------------
int main(void)
{
    printf("helloworld!\n");

    while (1)
    {
        unsigned char ch = 0;
        int len = read(STDIN_FILENO, &ch, 1);
        if (len > 0)
            printf("You pressed '%c'\n", ch);
    }

    return 0;
}

