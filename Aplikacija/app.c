#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
	FILE* fp;
	char option;
	char *str;
	size_t num_of_bytes = 100;

	while(1)
	{
		printf("1: Pročitaj trenutno stanje stringa\n");
		printf("2: Upiši novi string\n");
		printf("3: Konkataniraj string na trenutni\n");
		printf("4: Izbriši čitav string\n");
		printf("5: Izbriši vodeće i prateće space karaktere\n");
		printf("6: Izbriši izraz iz stringa\n");
		printf("7: Izbriši poslednjih n karaktera iz stringa\n");

		scanf("%c",&option);
		
		switch (option)
		{
			case '1':
				fp = fopen ("/dev/stred", "r");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred\n");
					return -1;
				}
				str = (char *)malloc(num_of_bytes+1);		
				getline(&str, &num_of_bytes, fp);

				if(fclose(fp))
				{
					puts("Problem pri zatvaranju /dev/stred\n");
					return -1;
				}

				printf("String je: %s\n", str);
				
				free(str);
				
				break;
			case '2':
				fp = fopen("/dev/stred", "w");
				if(fp == NULL)
				{
					printf("Problem pri otvaranju /dev/stred\n");
					return -1;
				}

				printf("String koji zelite da unesete: ");
				
				str = (char *)malloc(num_of_bytes+1);		
				scanf("%s", str);
		
				printf("String je: %s\n", str);
				fprintf(fp,"string=%s\n", str);

				if(fclose(fp))
				{
					printf("Problem pri zatvaranju /dev/led\n");
					return -1;
				}
				
				free(str);
				break;
			case '3':

				break;
			case '4':

				break;
			case '5':

				break;
			case '6':

				break;
			case '7':

				break;
			case 'Q':
				printf("Kraj aplikacije\n");
				return 0;
				break;
			default:
				break;
		}

	}

	return 0;
}
