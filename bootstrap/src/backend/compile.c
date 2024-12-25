#include "backend/compile.h"

// libc
#include <limits.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

bool compile_c(char const* source, char const* dest)
{
	char src_path[PATH_MAX];
	char dst_path[PATH_MAX];
	strncpy(src_path, source, PATH_MAX - 1);
	strncpy(dst_path, dest, PATH_MAX - 1);
	char* argv[] = {
		"cc",
		"-o",
		dst_path,
		src_path,
	};
	int pid = fork();
	if (pid < 0)
	{
		warn("fork() failed");
		return false;
	}
	else if (pid == 0)
	{
		execvp(*argv, argv);
		err(1, "execvp() failed");
	}
	int status = 0;
	if (!waitpid(pid, &status, 0))
	{
		warn("waitpid() failed");
		return false;
	}
	return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

bool run(char const* file)
{
	char path[PATH_MAX];
	strncpy(path, file, PATH_MAX - 1);
	char* argv[] = { path };
	int pid = fork();
	if (pid < 0)
	{
		warn("fork() failed");
		return false;
	}
	else if (pid == 0)
	{
		execvp(*argv, argv);
		err(1, "execvp() failed");
	}
	int status = 0;
	if (!waitpid(pid, &status, 0))
	{
		warn("waitpid() failed");
		return false;
	}
	return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}
