#include "Header.h"
#include "Common.h"

void exit_message(const char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
