# Цель работы
Реализовать приложение, запускающее произвольное приложение с получением его потоков и кода завершения на C++

# Ход работы
## Практическая часть

Shell скрипт, который заправшивает 2 аргумента:
- число, возводимое в степень (base)
- степень (power)<br>
и выводит результат возведения числа в степень.<br>
  также проверяет тип введенных аргументов и их количество.
```bash
#!/bin/bash

if [ $# -eq 2 ]
then
    re='^[0-9]+$'
    if ! [[ $1 =~ $re ]] || ! [[ $2 =~ $re ]]
    then
        echo "Wrong Arguments Type">&2;
        exit 1;
    fi

    res=1
    base=$1
    power=$2
    for ((i=0;i<power;i++))
    {
        res=$((res * base))
    }
    echo "$1^$2 = $res">&1
    exit 0
fi

echo "Wrong Arguments Amount">&2
exit 1;
```

C++ приложение, которое запускает вышеописанный скрипт, получает его стандартные потоки (stdout, stderr) и код завершения. 
Выводит полученные данные в соответствующие поток и файл.
Ошибки в ходе работы самого приложения выводятся в stderr и записываются в отдельный файл. 
```cpp
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
```

### Пример вызова Shell скрипта
![](https://raw.githubusercontent.com/ARAOvsepyan/BSBD/main/LR4/img/img.png)
### [Исходные коды](https://github.com/ARAOvsepyan/BSBD/tree/main/LR4/src)

# Выводы
Были получены знания о потоках и реализовано приложение для работы с потоками запускаемого приложения.
