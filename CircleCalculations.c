#define _USE_MATH_DEFINES
#include<stdio.h>
#include<math.h>
float Cs(float r)
{
    return M_PI * r * r;
}
float Cc(float r)
{
    return 2 * M_PI * r;
}
int main()
{
    float r;
    printf("请输入圆的半径：");
    if(scanf("%f", &r) != 1)
    {
        printf("输入错误！");
        return 1;
    }
    if(r <= 0)
    {
        printf("半径必须大于0！");
        return 1;
    }
    printf("圆的面积为：%.2f\n", Cs(r));
    printf("圆的周长为：%.2f\n", Cc(r));
    return 0;
}
