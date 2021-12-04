#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
	FILE* fp;
	char option;
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
		usleep(10);
	
		char *str;
	
		switch (option)
		{
			case '1':
				fp = fopen ("/dev/stred", "r");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred\n");
					return -1;
				}

				str = (char *)malloc(num_of_bytes);		
				getdelim(&str, &num_of_bytes, (int)'\0', fp);

				if(fclose(fp))
				{
					puts("Problem pri zatvaranju /dev/stred\n");
					return -1;
				}

				printf("%s\n", str);
				
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
				scanf("\n");
				scanf("%[^\n]%*c", str);

				fprintf(fp,"string=%s\n", str);

				if(fclose(fp))
				{
					printf("Problem pri zatvaranju /dev/led\n");
					return -1;
				}
				
				free(str);
				break;
			case '3':
				fp = fopen("/dev/stred", "w");
				if(fp == NULL)
				{
					printf("Problem pri otvaranju /dev/stred\n");
					return -1;
				}

				printf("String koji zelite da konkatanirate: ");
				
				str = (char *)malloc(num_of_bytes+1);		
				scanf("\n");
				scanf("%[^\n]%*c", str);
		
				fprintf(fp,"append=%s\n", str);

				if(fclose(fp))
				{
					printf("Problem pri zatvaranju /dev/led\n");
					return -1;
				}
				
				free(str);
				break;
			case '4':
				fp = fopen("/dev/stred", "w");
				if(fp == NULL)
				{
					printf("Problem pri otvaranju /dev/stred\n");
					return -1;
				}
					
				fputs("clear\n", fp);
			
				if(fclose(fp))
				{
					printf("Problem pri zatvaranju /dev/led\n");
					return -1;
				}
				
				break;
			case '5':
				fp = fopen("/dev/stred", "w");
				if(fp == NULL)
				{
					printf("Problem pri otvaranju /dev/stred\n");
					return -1;
				}
					
				fputs("shrink\n", fp);
			
				if(fclose(fp))
				{
					printf("Problem pri zatvaranju /dev/led\n");
					return -1;
				}
				
				break;
			case '6':
				fp = fopen("/dev/stred", "w");
				if(fp == NULL)
				{
					printf("Problem pri otvaranju /dev/stred\n");
					return -1;
				}

				printf("String koji zelite da izbrisete: ");
				
				str = (char *)malloc(num_of_bytes+1);		
				scanf("%s", str);

				fprintf(fp,"remove=%s\n", str);

				if(fclose(fp))
				{
					printf("Problem pri zatvaranju /dev/led\n");
					return -1;
				}
				
				free(str);
				
				break;
			case '7':
				fp = fopen("/dev/stred", "w");
				if(fp == NULL)
				{
					printf("Problem pri otvaranju /dev/stred\n");
					return -1;
				}

				printf("Koliko karaktera brisete: ");
				
				str = (char *)malloc(num_of_bytes+1);		
				scanf("%s", str);
		
				fprintf(fp,"truncate=%s\n", str);

				if(fclose(fp))
				{
					printf("Problem pri zatvaranju /dev/led\n");
					return -1;
				}
				
				free(str);
				
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
