#include <iostream>

using namespace std;

/**
 * @brief 递归版本的汉诺塔问题
 * 
 * 在每一个递归的子过程中 总是有一个柱子 a/b 是空的
 * 可以用来作为辅助
 * 
 * Issue: 递归层次很容易过深 有可能爆栈
 * 
 * @param n 圆盘的数量
 * @param a 当前要移动的圆盘
 * @param b 辅助柱子
 * @param c 目标柱子
 */
void Hanoi(int n, char a, char b, char c)
{
    if (n == 1)
    {
        // terminate state
        printf("move %c to %c\n", a, c);
    }
    else
    {
        //move n - 1 of a to b
        Hanoi(n - 1, a, c, b);
        //move the last one in a  to c
        printf("move %c to %c\n", a, c);
        // at this moment we move the largest one in a to c and leave the rest to b
        // since then the b is actually the new a logically
        // we call hanoi recursively
        // now a is used as the helper.
        Hanoi(n - 1, b, a, c);
    }
}

int main(int argc, char const *argv[])
{
    Hanoi(3, 'a', 'b', 'c');

    return 0;
}
