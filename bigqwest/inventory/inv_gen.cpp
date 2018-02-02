#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADRES(i) i ##.txt

int main ()
{
	int i, j, k;
	
	char* adres = (char*)malloc(100*sizeof(char));
	//char* char_i = (char*)malloc(5*sizeof(char));
	char* adres_i = (char*)malloc(100*sizeof(char));
	
	adres = ("/home/artyom/bigquest/tyomdimich/inventory/inv_");	
	
	printf("cikl zapuschen1\n");
	for (i = 1; i<10; i++)
	{	
		
		sprintf(adres_i, "%s%d%s", adres, i, ".txt");
		FILE* fp = fopen(adres_i, "w");
		
		printf("%s\n", adres_i);
		
		for (j = 0; j <10; j++)
		{
			for (k = 0; k <10; k++)
			{
					fprintf(fp, "%d%d%d\n", i, j, k);
			}
		}
		fclose(fp);
	}
	
}