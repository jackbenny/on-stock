/* A simple inventory tool written in C, by
 * Jack-Benny Persson (jack-benny@cyberinfo.se).
 * Released under GNU GPLv2.
*/

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
void search(struct myData *datap, int numRec, char *name);
void modify(struct myData *datap, int numRec, char *name, char *num);
void delete(struct myData *datap, int numRec, char *name);
int new(struct myData *datap, int numRec);
void printUsage(char *arg);
void printHeader(void);


int main(int argc, char* argv[])
{
    int numRec;
    int create;
    int choice;
    int opt;

    if (argc < 2)
    {
	printUsage(argv[0]);
        return 1;
    }
    
    /* Process command line arugments */
    while ((opt = getopt(argc, argv, "lsmdnf:")) != -1)
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
        case 'd':
	    choice = 'd';
	    break;
        case 'n':
	    choice = 'n';
	    break;
	case 'f':
	    strncpy(filename, optarg, FILEMAXLENGTH-1);
	    break;
        default:
	    printUsage(argv[0]);
            return 1;
	}
    }
    

    /* Check if file exists, if it dosen't, create it */
    if ( access(filename, R_OK|W_OK) != 0 )
    {
	fprintf(stderr, "Could not open %s\n", filename);
	printf("Create the file and start adding records? (y/n): ");
	create = getchar();
	if ( create == 'y' )
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

    /* The structure to contain our database */
    struct myData *data;

    /* Continue processesing the command-line arugments */
    if ( choice == 'l' || choice == 's' || choice == 'm' || choice == 'd' )
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
	else if ( choice == 's' && argc >= 2 )
	    search(data, numRec, argv[2]);
	else if ( choice == 's' && argc <= 2)
	    search(data, numRec, NULL);
	else if ( choice == 'm' && argc >= 3)
	    modify(data, numRec, argv[2], argv[3]);
	else if ( choice == 'm' && argc <= 4)
	    modify(data, numRec, NULL, NULL);
	else if ( choice == 'd' && argc >= 2 )
	    delete(data, numRec, argv[2]);
	else if ( choice == 'd' && argc <= 2 )
	    delete(data, numRec, NULL);

    }
    else if ( choice == 'n' )
    {
	data = calloc(1, sizeof(struct myData));
	if ( new(data, numRec) == 1 )
	    return 1;
    }
    else
    {
	printUsage(argv[0]);
	return 1;
    }

    free(data);
    return 0;
}


void list(struct myData *datap, int numRec)
{
    printHeader();
    for (int i = 0; i<numRec; i++)
    {
	printf("%-30s\t", datap[i].name);
	printf("%-10d\t", datap[i].quantity);
	printf("%.2f\t", datap[i].price);
	printf("\n");
    }
    printf("\n");
}

void search(struct myData *datap, int numRec, char *name)
{
    char searchword[NAMEMAXLENGTH];
    if (name != NULL)
    {
	strncpy(searchword, name, NAMEMAXLENGTH-1);
    }
    else
    {
	printf("Name: ");
	fgets(searchword, NAMEMAXLENGTH, stdin);
	/* Replace the newline character with a null character */
	searchword[strcspn(searchword, "\n")] = '\0';
    }
    
    for (int i = 0; i<numRec; i++)
    {
	if ( strcmp(searchword, datap[i].name) != 0 )
	    continue;
	printHeader();
	printf("%-30s\t", datap[i].name);
	printf("%-10d\t", datap[i].quantity);
	printf("%.2f\t", datap[i].price);
	printf("\n");	
    }
}

void modify(struct myData *datap, int numRec, char *name, char *num)
{
    char searchword[NAMEMAXLENGTH];
    char what[10];
    char quant[20];
    int interactive = 1;
    
    if (name != NULL && num != NULL)
    {
	strncpy(searchword, name, NAMEMAXLENGTH-1);
	strncpy(what, "quantity", 9);
	strncpy(quant, num, 19);
	interactive = 0;
    }
    else
    {
	printf("Name: ");
	fgets(searchword, NAMEMAXLENGTH, stdin);
	searchword[strcspn(searchword, "\n")] = '\0';
    }
    
    for (int i = 0; i<numRec; i++)
    {
	if ( strcmp(searchword, datap[i].name) == 0 )
	{
	    if (interactive == 1)
	    {
		printHeader();
		printf("%-30s\t", datap[i].name);
		printf("%-10d\t", datap[i].quantity);
		printf("%.2f\t", datap[i].price);
		printf("\n\n");    

		printf("What do you like to modify? (name, quantity, price): ");
		fgets(what, 10, stdin);
		what[strcspn(what, "\n")] = '\0';
	    }

	    if ( strcmp(what, "name") == 0 )
	    {
		printf("Name: "); scanf("%29s", datap[i].name);
	    }
	    else if ( strcmp(what, "quantity") == 0 )
	    {
		if (interactive == 1)
		{
		    printf("Quantity (absolute value or (a)dd/(s)ubtractNUMBER): ");
		    fgets(quant, 20, stdin);
		    quant[strcspn(quant, "\n")] = '\0';
		}
		/* Process the first character */
		if (quant[0] == 'a')
		{
		    quant[0] = ' '; /* We need to remove the character first */
		    datap[i].quantity = datap[i].quantity + atoi(quant);
		}
		else if (quant[0] == 's')
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
	else
	{
	    fprintf(stderr, "Could not find %s in database\n", searchword);
	    exit(1);
	}
	
    }
    FILE *newfp = fopen(filename, "wb");
    fwrite(datap, sizeof(struct myData), numRec, newfp);
    fclose(newfp);
}

void delete(struct myData *datap, int numRec, char *name)
{
    char searchword[NAMEMAXLENGTH];
    int answer;
    int interactive = 1;
    
    if (name != NULL)
    {
	strncpy(searchword, name, NAMEMAXLENGTH-1);
	interactive = 0;
	answer = 'y';
    }
    else
    {
	printf("Name: ");
	fgets(searchword, NAMEMAXLENGTH, stdin);
	searchword[strcspn(searchword, "\n")] = '\0';
    }
    for (int i = 0; i<numRec; i++)
    {
	if ( strcmp(searchword, datap[i].name) == 0 )
	{
	    if (interactive == 1)
	    {
		printHeader();
		printf("%-30s\t", datap[i].name);
		printf("%-10d\t", datap[i].quantity);
		printf("%.2f\t", datap[i].price);
		printf("\n\n");

		printf("Delete the record listed above? (y/n): ");
		answer = getchar();
	    }
	    if ( answer == 'y' )
	    {
		FILE *newfp = fopen(filename, "wb");
		/* Write out all records, except the one we cant to delete,
		   back to the file, one record at a time */
		for (int j = 0; j<numRec; j++)
		{
		    if ( strcmp(searchword, datap[j].name) == 0 )
			continue;
		    fwrite(&datap[j], sizeof(struct myData), 1, newfp);
		}
		fclose(newfp);
	    }
	}
	else
	{
	    fprintf(stderr, "Could not find %s in database\n", searchword);
	    exit(1);
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

    /* Ask for, and write out, one record at a time */
    for (;;)
    {
	/* Clear the input buffer (from the previous iteration) */
        setbuf(stdin, NULL);
	printf("Name ('done' when finished): ");
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
	    fprintf(stderr, "Could not write to the file!\n");
	    return 1;
	}
    }
}

void printUsage(char *arg)
{
    fprintf(stderr, "Usage: %s [-l] [-s] [-m [-d [-n] [-f filename]\n"
	    "-l = list the articles in the database\n"
	    "-s = search for an article in the database\n"
	    "-m = modify a article\n"
	    "-d = delete a article\n"
	    "-n = create a new article\n"
	    "-f = specifiy a filename for the database\n", arg);
}

void printHeader(void)
{
    printf("\n%-30s\t", "Name");
    printf("%s\t", "Quantity");
    printf("%s\t\n", "Price");
    for (int i = 0; i<=52; i++)
	printf("=");
    printf("\n");
}
