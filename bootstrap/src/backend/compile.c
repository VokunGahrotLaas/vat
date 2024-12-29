#include "backend/compile.h"

// libc
#include <limits.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

bool compile_c(struct list const* sources, char const* dest)
{
	int pid = fork();
	if (pid < 0)
	{
		warn("fork() failed");
		return false;
	}
	else if (pid == 0)
	{
		struct list argv;
		list_ctor(&argv, &vlist_pchar, 16);
		char cc[] = "cc";
		char dash_o[] = "-o";
		char* ccp = cc;
		char* dop = dash_o;
		list_push_copy(&argv, &ccp);
		list_push_copy(&argv, &dop);
		list_push_copy(&argv, &dest);
		for (size_t i = 0; i < sources->size; ++i)
			list_push_copy(&argv, LIST_CGET(sources, char*, i));
		execvp(*(char**)argv.data, argv.data);
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
