// 引入time.h头文件
#include <iostream>
#include <time.h>

using namespace std;

int main()
{
    // 定义两个time_t变量，分别表示1970年1月1日和2023年9月20日
    time_t t1 = 0;
    time_t t2 = 1698009600;
    // 输出这两个time_t值
    cout << "t1: " << t1 << '\n';
    cout << "t2: " << t2 << '\n';
    // 使用difftime()函数计算它们之间相差的秒数
    double diff = difftime(t2, t1);
    // 输出结果
    cout << "t2 - t1: " << diff << " seconds\n";
    // 使用ctime()函数将t2转换为字符串
    char* s = ctime(&t2);
    // 输出结果
    cout << "t2 in string: " << s;
    // 使用gmtime()函数将t2转换为tm结构体（表示UTC时间）
    tm* utc = gmtime(&t2);
    // 输出结果
    cout << "t2 in UTC: " << asctime(utc);
    // 使用localtime()函数将t2转换为tm结构体（表示本地时间）
    tm* local = localtime(&t2);
    local->tm_gmtoff;
    // 输出结果
    cout << "t2 in local: " << asctime(local);
    // 使用mktime()函数将local转换回time_t值
    time_t t3 = mktime(local);
    // 输出结果
    cout << "t3: " << t3 << '\n';
}
