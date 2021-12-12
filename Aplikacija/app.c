#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
	FILE *fp;
	char *option;
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
		printf("Za izlaz iz aplikacije unesite Q\n");
		
		size_t num_b = 1;
		option = (char*) malloc(num_b+1);
		getline(&option,&num_b, stdin);

		char *str;
	
		switch (*option)
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
			
				printf("%s\n",str);	
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
				
				//Sledeci deo je zato sto prazne karaktere ne prihvata na pocetku vec ih ignorise, pa ovako da prebroji
				//na pocetku koliko ih ima
				int i = 0;
				int prazni = 0;
				char *prazni_deo;
				char x;
				while (i==0)   //broji ' ' unesene
				{
					x = getchar();
					if (x == ' ')
						prazni++;
					else
						i = 1;
				}
				scanf("%[^\n]", str); //ucita ostatak stringa
				getchar(); //prezume \n koji je ostao

				for (i = 0; i < prazni; i++) //pravi string praznih karaktera za pocetak
				{
					prazni_deo[i] = ' ';
				}	
				prazni_deo[i] = '\0';

				fprintf(fp,"string=%s%c%s\n",prazni_deo, x, str);  //unosi broj praznih karaktera, zatim x (prvi karaker koji je ucitao), i na kraju ostatak stringa

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
				
				//Sledeci deo je zato sto prazne karaktere ne prihvata na pocetku vec ih ignorise, pa ovako da prebroji
				//na pocetku koliko ih ima
				int j = 0;
				int prazni_append = 0;
				char *prazni_deo_append;
				char x_append;
				while (j==0)   //broji ' ' unesene
				{
					x_append = getchar();
					if (x_append == ' ')
						prazni_append++;
					else
						j = 1;
				}

				scanf("%[^\n]", str); //ucita ostatak stringa
				getchar();				
				for (j = 0; j < prazni_append; j++) //pravi string praznih karaktera za pocetak
				{
					prazni_deo_append[j] = ' ';
				}	
				prazni_deo_append[j] = '\0';

				fprintf(fp,"append=%s%c%s\n",prazni_deo_append, x_append, str);  //unosi broj praznih karaktera, zatim x (prvi karaker koji je ucitao), i na kraju ostatak stringa

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
				scanf("\n");
				scanf("%[^\n]%*c", str);
				
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

		free(option);
	}

	return 0;
}
