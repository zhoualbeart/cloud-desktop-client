#include <stdio.h>


int main(int argc, char **argv)
{
	int i;

	if (argc != 2)
	{
		printf("argc = %d\n", argc);
		printf("usage : %s filepath\n", argv[0]);

		for(i = 0; i < argc; i++)
		{
			printf("argc[%d] = %s\n", i, argv[i]);
		}
		getchar();
		return 1;
	}

	printf("filepath is %s\n", argv[1]);
	
	getchar();

	return 0;
}