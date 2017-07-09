#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Macros */
#define FILEMAXLENGTH 50
#define NAMEMAXLENGTH 30

/* Globals */
struct myData
{
    char name[NAMEMAXLENGTH];
    int quantity;
    float price;
};

char filename[FILEMAXLENGTH] = "storage.bin";

/* Function prototype */
void list(struct myData *datap, int numRec);
void search(struct myData *datap, int numRec);
void modify(struct myData *datap, int numRec);
void delete(struct myData *datap, int numRec);
int new(struct myData *datap, int numRec);


int main(int argc, char* argv[])
{
    int numRec;
    int create;
    int choice;
    int opt;

    if (argc != 2)
    {
	fprintf(stderr, "Usage: %s [-l] [-s] [-m [-m [-n] [-f filename]\n"
		"-l = list the articles in the database\n"
		"-s = search for an article in the database\n"
		"-m = modify a article\n"
		"-r = remove a article\n"
		"-n = create a new article\n"
		"-f = specifiy a filename for the database\n", argv[0]);
        return 1;
    }
    
    /* Process command line arugments */
    while ((opt = getopt(argc, argv, "lsmrnf:")) != -1)
    {
        switch (opt)
	{
        case 'l':
	    choice = 'l';
	    break;
        case 's':
	    choice = 's';
	    break;
        case 'm':
	    choice = 'm';
	    break;
        case 'r':
	    choice = 'r';
	    break;
        case 'n':
	    choice = 'n';
	    break;
	case 'f':
	    strncpy(filename, optarg, FILEMAXLENGTH-1);
	    break;
        default:
            fprintf(stderr, "Usage: %s [-l] [-s] [-m [-m [-n] [-f filename]\n"
		    "-l = list the articles in the database\n"
		    "-s = search for an article in the database\n"
		    "-m = modify a article\n"
		    "-r = remove a article\n"
		    "-n = create a new article\n"
		    "-f = specifiy a filename for the database\n", argv[0]);
            return 1;
	}
    }
    

    /* Check if file exists, if it dosen't, create it */
    if ( access(filename, R_OK|W_OK) != 0 )
    {
	printf("Kan inte öppna %s\n", filename);
	printf("Skapa filen och lägg till artiklar? (j/n): ");
	create = getchar();
	if ( create == 'j' )
	{
	    numRec = 1;
	    struct myData *data;
	    data = calloc(numRec, sizeof(struct myData));
	    new(data, numRec);
	    free(data);
	}
	else
	    return 1;
    }

    struct myData *data;

    if ( choice == 'l' || choice == 's' || choice == 'm' || choice == 'r' )
    {
	/* Open file in read-mode */
	FILE *fp = fopen(filename, "rb");

	/* Read in the content from the file to the structure */
	fseek(fp, 0, SEEK_END);
	numRec = ftell(fp) / sizeof(struct myData);
	data = calloc(numRec, sizeof(struct myData));
	rewind(fp);
	fread(data, sizeof(struct myData), numRec, fp);
	fclose(fp);

	
	if ( choice == 'l' )
	    list(data, numRec);
	else if ( choice == 's' )
	    search(data, numRec);
	else if ( choice == 'm' )
	    modify(data, numRec);
	else if ( choice == 'r' )
	    delete(data, numRec);

    }
    else if ( choice == 'n' )
    {
	data = calloc(1, sizeof(struct myData));
	new(data, numRec);
    }

    free(data);
    return 0;
}

void list(struct myData *datap, int numRec)
{
    for (int i = 0; i<numRec; i++)
    {
	printf("Name: %s\n", datap[i].name);
	printf("Quantity: %d\n", datap[i].quantity);
	printf("Price: %.2f\n", datap[i].price);
	printf("\n");
    }
}

void search(struct myData *datap, int numRec)
{
    char searchword[NAMEMAXLENGTH];
    
    printf("Name: ");
    fgets(searchword, NAMEMAXLENGTH, stdin);
    searchword[strcspn(searchword, "\n")] = '\0';
    printf("\n");
    
    for (int i = 0; i<numRec; i++)
    {
	if ( strcmp(searchword, datap[i].name) != 0 )
	    continue;
	
	printf("Name: %s\n", datap[i].name);
	printf("Quantity: %d\n", datap[i].quantity);
	printf("Price: %f\n", datap[i].price);
	printf("\n");
    }
}

void modify(struct myData *datap, int numRec)
{
    char searchword[NAMEMAXLENGTH];
    char what[10];
    char quant[20];

    printf("Name: ");
    fgets(searchword, NAMEMAXLENGTH, stdin);
    searchword[strcspn(searchword, "\n")] = '\0';
    printf("\n");
    
    for (int i = 0; i<numRec; i++)
    {
	if ( strcmp(searchword, datap[i].name) == 0 )
	{
	    printf("Namn: %s\n", datap[i].name);
	    printf("Quantity: %d\n", datap[i].quantity);
	    printf("Price: %f\n", datap[i].price);
	    printf("\n");

	    printf("What do you like to modify? (name, quantity, price): ");
	    scanf("%9s", what);
	    if ( strcmp(what, "name") == 0 )
	    {
		printf("Name: "); scanf("%29s", datap[i].name);
	    }
	    else if ( strcmp(what, "quantity") == 0 )
	    {
		printf("Quantity (absolute value or +/-NUMBER: "); scanf("%19s", quant);
		printf("Sign: %c\n", quant[0]);
		if (quant[0] == '+')
		    datap[i].quantity = datap[i].quantity + atoi(quant);
		else if (quant[0] == '-')
		{
		    quant[0] = ' ';
		    datap[i].quantity = datap[i].quantity - atoi(quant);
		}
		else
		    datap[i].quantity = atoi(quant);
		
	    }
	    else if ( strcmp(what, "price") == 0 )
	    {
		printf("Price: "); scanf("%f", &datap[i].price);
	    } 
	}
    }
    FILE *newfp = fopen(filename, "wb");
    fwrite(datap, sizeof(struct myData), numRec, newfp);
    fclose(newfp);
}

void delete(struct myData *datap, int numRec)
{
    char searchword[NAMEMAXLENGTH];
    int answer;
    printf("Name: ");
    fgets(searchword, NAMEMAXLENGTH, stdin);
    searchword[strcspn(searchword, "\n")] = '\0';
    
    printf("\n");
    for (int i = 0; i<numRec; i++)
    {
	if ( strcmp(searchword, datap[i].name) == 0 )
	{
	    printf("Name: %s\n", datap[i].name);
	    printf("Quantity: %d\n", datap[i].quantity);
	    printf("Price: %f\n", datap[i].price);
	    printf("\n");

	    printf("Delete the record listed above? (y/n): ");
	    answer = getchar();
	    if ( answer == 'y' )
	    {
		FILE *newfp = fopen(filename, "wb");
		for (int j = 0; j<numRec; j++)
		{
		    if ( strcmp(searchword, datap[j].name) == 0 )
			continue;
		    fwrite(&datap[j], sizeof(struct myData), 1, newfp);
		}
		fclose(newfp);
	    }
	}
    }
}

int new(struct myData *datap, int numRec)
{
    int bytes;
    FILE *fp = fopen(filename, "ab");
    if (fp == NULL)
    {
	printf("Can't open file for writing\n");
	return 1;
    }

    for (;;)
    {
        setbuf(stdin, NULL);
	printf("Name: ('done' if finish) ");
	fgets(datap->name, NAMEMAXLENGTH, stdin);
	datap->name[strcspn(datap->name, "\n")] = '\0';
	
	if ( strcmp(datap->name, "done") == 0 )
	{
	    fclose(fp);
	    return 0;
	}
	printf("Quantity: "); scanf("%d", &datap->quantity);
	printf("Price: "); scanf("%f", &datap->price);
	bytes = fwrite(datap, sizeof(struct myData), 1, fp);
	if (bytes != 1)
	{
	    printf("Could not write to the file!\n");
	    return 1;
	}
    }
}

