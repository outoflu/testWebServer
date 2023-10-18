#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    // 打开一个文件
    int fd = open("test.txt", O_RDONLY);
    if (fd == -1) {
        // 错误处理
        return 1;
    }

    // 使用文件描述符读取文件
    char buffer[1024];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);

    if (bytesRead == -1) {
        // 错误处理
        close(fd);
        return 1;
    }

    // 确保字符串以null结尾
    buffer[bytesRead] = '\0';

    // 打印读取的内容
    printf("%s\n", buffer);

    // 关闭文件描述符
    close(fd);

    return 0;
}
