#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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
    
    mknod("/tmp/readpipe", S_IFIFO | 0666, 0);
    mknod("/tmp/writepipe", S_IFIFO | 0666, 0);


    pid_t is_reader = fork();

    if (is_reader) {
        printf("Reader is in online\n");
        int read_fd = open(argv[1], O_RDONLY);
        read(read_fd, input, sizeof(input));
        int read_pipe = open("/tmp/readpipe", O_WRONLY);
        write(read_pipe, input, sizeof(input));

        close(read_fd);
        close(read_pipe);
    } else {
        pid_t is_writer = fork();

        if (is_writer) {
            printf("Writer is in online\n");
            int write_pipe = open("/tmp/writepipe", O_RDONLY);
            read(write_pipe, input, sizeof(input));

            int write_fd = open(argv[2], O_WRONLY | O_CREAT, 0666);

            write(write_fd, input, strlen(input));

            close(write_fd);
            close(write_pipe);
            unlink("/tmp/readpipe");
            unlink("/tmp/writepipe");
        } else {
            printf("Processor is in online\n");
            int read_pipe = open("/tmp/readpipe", O_RDONLY);
            int write_pipe = open("/tmp/writepipe", O_WRONLY);
            read(read_pipe, input, sizeof(input));
          
            printf("Processor read string: %s\n", input);
            func(input);
            write(write_pipe, input, sizeof(input));
            close(read_pipe);
            close(write_pipe);
        }
    }

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
