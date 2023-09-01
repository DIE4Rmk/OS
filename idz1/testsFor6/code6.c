#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void func(char *s);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Err\n");
        return 1;
    }

    int rp[2], wp[2];
    char input[10000] = {};

    pipe(rp);
    pipe(wp);

    pid_t is_reader = fork();

    if (is_reader) {
        printf("Reader is in online\n");
        int read_fd = open(argv[1], O_RDONLY);
        read(read_fd, input, sizeof(input));
        close(read_fd);
        func(input);
        write(rp[1], input, sizeof(input));
        read(wp[0], input, sizeof(input));
        int write_fd = open(argv[2], O_WRONLY | O_CREAT, 0666);
        write(write_fd, input, strlen(input));
        close(write_fd);
        
    } else {
        printf("Processor is in online\n");
        read(rp[0], input, sizeof(input));

        printf("Processor read input: %s\n",input);

        write(wp[1], input, sizeof(input));
    }

    close(rp[0]);
    close(rp[1]);
    close(wp[0]);
    close(wp[1]);

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
