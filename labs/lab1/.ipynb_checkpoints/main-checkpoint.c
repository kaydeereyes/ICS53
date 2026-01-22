#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int i, count = 0;
	for (i=0; i < argc; i++){
		printf("argv[%d]: %s\n", i, argv[i]);
		count += strlen(argv[i]);
	}
	printf("Total ASCII chars for arguments: %d\n", count);
	return 0;
}
