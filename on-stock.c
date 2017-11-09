/* A simple inventory tool written in C, by
   Jack-Benny Persson (jack-benny@cyberinfo.se).
   Released under GNU GPLv2.
*/

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Macros */
#define NAMEMAXLENGTH 30

/* Globals */
struct myData
{
    char name[NAMEMAXLENGTH];
    int quantity;
    float price;
};

char filename[] = "storage.bin";

/* Function prototypes */
void list(struct myData *datap, int numRec);
void search(struct myData *datap, int numRec, char *name);
void modify(struct myData *datap, int numRec, char *name);
void delete(struct myData *datap, int numRec, char *name);
int new(struct myData *datap, int numRec);
void printUsage(char *arg);
void printHeader(void);


int main(int argc, char* argv[])
{
    FILE *fp; 
    int newart = 0;
    int numRec;
    int create;
    int opt;

    struct myData *data;

    /* Print help and exit if no arguments are given */
    if (argc < 2)
    {
        printUsage(argv[0]);
        return 1;
    }
    
    /* Check file access mode */
    if ( access(filename, R_OK|W_OK) != 0 )
    {
        fprintf(stderr, "Could not open %s\n", filename);
        printf("Create the file and start adding records? (y/n): ");
        create = getchar();
        if ( create == 'y' )
        {
        /* Start createing records */
            numRec = 1;
            data = calloc(numRec, sizeof(struct myData));
            new(data, numRec);
            free(data);
        }
        else
            return 1;
    }

    fp = fopen(filename, "rb");

    fseek(fp, 0, SEEK_END);
    numRec = ftell(fp) / sizeof(struct myData);
    data = calloc(numRec, sizeof(struct myData));
    rewind(fp);
    fread(data, sizeof(struct myData), numRec, fp);
    fclose(fp);

    /* Process arguments with getopt() */
    while ((opt = getopt(argc, argv, "hls:m:d:n")) != -1)
    {
        switch (opt)
        {
        case 'l':
            list(data, numRec);
            break;
        case 's':
            search(data, numRec, optarg);
            break;
        case 'm':
            modify(data, numRec, optarg);
            break;
        case 'd':
            delete(data, numRec, optarg);
            break;
        case 'n':
            newart = 1;
            break;
        case 'h':
            printUsage(argv[0]);
            return 0;
        default:
            printUsage(argv[0]);
            return 1;
        }
    }
    
    /* Continue here */
    if ( newart == 1 )
    {
        data = calloc(1, sizeof(struct myData));
        if ( new(data, numRec) == 1 )
            return 1;
    }

    /* Finished */
    free(data);
    return 0;
}

void list(struct myData *datap, int numRec)
{
    int i;
    printHeader();
    for (i = 0; i<numRec; i++)
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
    int i;
    /* Iterate over the database and search */
    for (i = 0; i<numRec; i++)
    {
        if ( strcmp(name, datap[i].name) != 0 )
            continue;
        printHeader();
        printf("%-30s\t", datap[i].name);
        printf("%-10d\t", datap[i].quantity);
        printf("%.2f\t", datap[i].price);
        printf("\n");   
    }
}

void modify(struct myData *datap, int numRec, char *name)
{
    FILE *newfp;
    int i;
    int match = 0;
    char what[10];
    char quant[20];

    /* Iterate over the database and search */
    for (i = 0; i<numRec; i++)
    {
        if ( strcmp(name, datap[i].name) == 0 )
        {
            match = 1;
            printHeader();
            printf("%-30s\t", datap[i].name);
            printf("%-10d\t", datap[i].quantity);
            printf("%.2f\t", datap[i].price);
            printf("\n\n");    

            printf("What do you like to modify? "
                   "(name, quantity, price): ");
            fgets(what, 10, stdin);
            what[strcspn(what, "\n")] = '\0';

            if ( strcmp(what, "name") == 0 )
            {
                printf("Name: ");
                fgets(datap[i].name, NAMEMAXLENGTH, stdin);
                datap[i].name[strcspn(datap[i].name, "\n")] = '\0';
            }
            else if ( strcmp(what, "quantity") == 0 )
            {
                printf("Quantity (absolute value or "
                       "(a)dd/(s)ubtractNUMBER): ");
                fgets(quant, 20, stdin);
                quant[strcspn(quant, "\n")] = '\0';
                
                if (quant[0] == 'a') /* a for add */
                {
                    quant[0] = ' '; /* Remove the sign */
                    datap[i].quantity = datap[i].quantity +
                        atoi(quant);
                }
                else if (quant[0] == 's') /* s for subtract */
                {
                    quant[0] = ' ';
                    datap[i].quantity = datap[i].quantity -
                        atoi(quant);
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
    if (match == 0)
    {
        fprintf(stderr, "Could not find %s in database\n", name);
        exit(1);
    }
    /* Write out the new content to file */
    if ( (newfp = fopen(filename, "wb")) == 0 )
    {
        fprintf(stderr, "Could not open file for writing\n");
        exit(1);
    }
    fwrite(datap, sizeof(struct myData), numRec, newfp);
    fclose(newfp);
}

void delete(struct myData *datap, int numRec, char *name)
{
    int i, j;
    int match = 0;
    FILE *newfp; 
    int answer;

    /* Iterate the database and search */
    for (i = 0; i<numRec; i++)
    {
        if ( strcmp(name, datap[i].name) == 0 )
        {
            match = 1;
            printHeader();
            printf("%-30s\t", datap[i].name);
            printf("%-10d\t", datap[i].quantity);
            printf("%.2f\t", datap[i].price);
            printf("\n\n");
            printf("Delete the record listed above? (y/n): ");
            
            answer = getchar();
            if ( answer == 'y' )
            {
                if ( (newfp = fopen(filename, "wb")) == 0 )
                {
                    fprintf(stderr,
                            "Could not open file for writing\n");
                    exit(1);
                }
        /* Iterate the database and write out every
                   record except those that matched! */
                for (j = 0; j<numRec; j++)
                {
                    if ( strcmp(name, datap[j].name) == 0 )
                        continue;
                    fwrite(&datap[j], sizeof(struct myData), 1,
                           newfp);
                }
                fclose(newfp);
            }
        }
    }
    if (match == 0)
    {
        fprintf(stderr, "Could not find %s in database\n", name);
        exit(1);
    }
}

int new(struct myData *datap, int numRec)
{
    FILE *fp;
    int bytes;
    if ( (fp = fopen(filename, "ab")) == 0 )
    {
        fprintf(stderr, "Could not open file for writing\n");
        return 1;
    }

    /* End loop when user types 'done' */
    for (;;)
    {
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
    fprintf(stderr, "Usage: %s [-l] [-s name]"
            "[-m name \n" 
            "[-d name] [-n] [-h] [-f filename]\n\n"
        "-l      = list the articles in the database\n"
        "-s name = search for an 'name' in the database\n"
        "-m name = modify the article named 'name' in the database\n"
        "     You'll then have the choice to change name, quantity "
            "and price.\n"
        "-d name = delete the article named 'name'\n"
        "-n      = create new articles (interactive mode only)\n"
        "-h      = display this help message\n", arg);
}

void printHeader(void)
{
    int i;
    printf("\n%-30s\t", "Name");
    printf("%s\t", "Quantity");
    printf("%s\t\n", "Price");
    for (i = 0; i<=52; i++)
        printf("=");
    printf("\n");
}
