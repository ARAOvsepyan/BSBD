#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

template<typename... ARGS>
void print(FILE *file, int code, char *format, ARGS... args)
{
    fprintf(file, format, args...);
    if (code)
    {
        fprintf(stdout, format, args...);
    } else
    {
        fprintf(stderr, format, args...);
    }
}


int main(int argc, char *argv[])
{
    // file for logging app errors
    FILE *f_log;
    f_log = fopen("system", "w");

    // files for streams and exit code
    FILE *f_stdout;
    FILE *f_stderr;
    FILE *f_return;
    f_stdout = fopen("stdout", "w");
    f_stderr = fopen("stderr", "w");
    f_return = fopen("return", "w");

    if (argc < 2)
    {
        print(f_log, 1, "No App Path\n");
        return 1;
    }

    // full path to app with args
    std::string string_path = "./";
    for (int i = 1; i < argc; i++)
    {
        string_path += std::string(argv[i]) + " ";
    }
    const char *full_path = string_path.erase(string_path.length() - 1).c_str();

    int stdout_pipe[2], stderr_pipe[2];
    if (pipe(stdout_pipe) < 0 || pipe(stderr_pipe) < 0)
    {
        print(f_log, 1, "Pipe Error\n");
        return 1;
    }

    int exitcode;
    pid_t pid = fork();
    if (pid == 0)
    {
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        dup2(stdout_pipe[1], 1);
        dup2(stderr_pipe[1], 2);
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        exitcode = system(full_path);
        return WEXITSTATUS(exitcode);
    } else
    {
        waitpid(pid, &exitcode, 0);
    }
    exitcode = WEXITSTATUS(exitcode);
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    // store streams data to buffers
    char stdout_buffer[128], stderr_buffer[128];
    stdout_buffer[read(stdout_pipe[0], stdout_buffer, sizeof(stdout_buffer))] = '\0';
    stderr_buffer[read(stderr_pipe[0], stderr_buffer, sizeof(stderr_buffer))] = '\0';

    // print data
    print(f_stdout, 0, stdout_buffer);
    print(f_stderr, 1, stderr_buffer);
    print(f_return, 0, "exit code: %d\n", exitcode);

    return 0;
}
