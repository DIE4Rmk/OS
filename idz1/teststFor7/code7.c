#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void func(char *s);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Err\n");
        return 1;
    }

    char *fr = "fifo1";
    char *fw = "fifo2";
    char input[10000];

    mkfifo(fr, 0666);
    mkfifo(fw, 0666);

    pid_t is_reader = fork();

    if (is_reader) {
        printf("Reader is in online\n");
        int read_fd = open(argv[1], O_RDONLY);
        read(read_fd, input, sizeof(input));
        close(read_fd);
        func(input);
        int write_fd = open(fr, O_WRONLY);
        write(write_fd, input, strlen(input));
        close(write_fd);
        int read_fd2 = open(fw, O_RDONLY);
        read(read_fd2, input, sizeof(input));
        close(read_fd2);
        int write_fd2 = open(argv[2], O_WRONLY | O_CREAT, 0666);
        write(write_fd2, input, strlen(input));
        close(write_fd2);
        
    } else {
        printf("Processor is in online\n");
        int read_fd = open(fr, O_RDONLY);
        read(read_fd, input, sizeof(input));
        close(read_fd);
        printf("Processor read input: %s\n",input);
        int write_fd = open(fw, O_WRONLY);
        write(write_fd, input, strlen(input));
        close(write_fd);
    }

    unlink(fr);
    unlink(fw);

    return 0;
}

void func(char *s) {
    
    int len = strlen(s);
    for (int i = 0; i < len; i++) {
        char ch = s[i];
        if (ch < 65 || ch > 90) {
            s[i] = ' ';
        }
    }
}
