#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main ()
{
	int i, j, k, l;
	
	char* adres = (char*)malloc(100*sizeof(char));
	char* char_i = (char*)malloc(5*sizeof(char));
	char* adres_i = (char*)malloc(100*sizeof(char));
	
	adres = "/home/artyom/bigquest/tyomdimich/inventory/inv2_";	
	char_i = "0.txt";
	
	printf("cikl zapuschen1\n");
	for (i = 1; i<10; i++)
	{	
		char_i[0] = (char)i;
		printf("strcat\n");
		adres_i = strcat(adres, char_i);
		printf("otkrivaem fp\n");
		FILE* fp = fopen(adres_i, "w");
		
		printf("cikl zapuschen2\n");
		for (j = 0; j <10; j++)
		{
			
			for (k = 0; k <10; k++)
			{
				for (l = 0; l < 10; l++)
				{
					fprintf(fp, "%d%d%d%d\n", i, j, k, l);
				}
			}
		}
	fclose(fp);
	}
	
}