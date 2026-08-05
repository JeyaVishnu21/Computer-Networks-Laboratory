#include <stdio.h>
#include <string.h>

#define MAX 1000

int bits[MAX];
int stuffed[MAX];
int destuffed[MAX];
int dataBytes[MAX];
int stuffedBytes[MAX];
int framedBytes[MAX];
int destuffedBytes[MAX];
int flagBits[8] = {0,1,1,1,1,1,1,0};   // 01111110
int flagByte = 126;                    // '~'
int escByte = 35;                     // 00100011 or '#'

// Prints bits continuously with no spaces
void printBits(char *label, int arr[], int n)
{
    int i;
    printf("%s : ", label);
    for(i=0; i<n; i++)
    {
        printf("%d", arr[i]);
    }
    printf("\n");
}

void printByteBinary(int byte)
{
    int i;
    for(i=7; i>=0; i--)
        printf("%d", (byte>>i)&1);
}

void printBytes(char *label, int arr[], int n)
{
    int i;
    printf("%s : ", label);
    for(i=0; i<n; i++)
    {
        printByteBinary(arr[i]);
        printf(" ");
    }
    printf("\n");
}

void stringToBinary(char str[], int arr[], int *n)
{
    int i, j, k=0;
    for(i=0; str[i]!='\0'; i++)
    {
        unsigned char ch = str[i];
        for(j=7; j>=0; j--)
        {
            arr[k++] = (ch>>j)&1;
        }
    }
    *n = k;
}

void binaryToString(int arr[], int n, char str[])
{
    int i, j, index=0;
    for(i=0; i<n; i+=8)
    {
        int value=0;
        for(j=0; j<8; j++)
        {
            value = value*2 + arr[i+j];
        }
        str[index++] = value;
    }
    str[index]='\0';
}

void bitStuffing(char msg[])
{
    int n;
    int i, j=0, k=0;
    int ones=0;
    int framed[MAX];

    stringToBinary(msg, bits, &n);
    printBits("\nOriginal Data", bits, n);

    // Bit Stuffing Process
    for(i=0; i<n; i++)
    {
        stuffed[j++] = bits[i];
        if(bits[i]==1)
            ones++;
        else
            ones=0;

        if(ones==5)
        {
            stuffed[j++] = 0;
            ones=0;
        }
    }
    int stuffedLen=j;

    // Framing
    for(i=0; i<8; i++)
        framed[k++] = flagBits[i];

    for(i=0; i<stuffedLen; i++)
        framed[k++] = stuffed[i];

    for(i=0; i<8; i++)
        framed[k++] = flagBits[i];

    int framedLen=k;
    printBits("Stuffed Data  ", stuffed, stuffedLen);
    printBits("Framed Data   ", framed, framedLen);

    // Error Injection
    char corruptChoice;
    printf("\nDo you want to corrupt the framed data? (y/n): ");
    scanf(" %c", &corruptChoice);
    if(corruptChoice == 'y' || corruptChoice == 'Y')
    {
        int flipIndex;
        printf("Enter the index to flip (0 to %d): ", framedLen - 1);
        scanf("%d", &flipIndex);
        if(flipIndex >= 0 && flipIndex < framedLen)
        {
            framed[flipIndex] = !framed[flipIndex];
            printf("--- Data Corrupted successfully! ---\n");
            printBits("Corrupted Frame", framed, framedLen);
        }
        else
        {
            printf("Invalid index! No corruption injected.\n");
        }
    }

    // Receiver Check: Framing validation
    for(i=0; i<8; i++)
    {
        if(framed[i] != flagBits[i] || framed[framedLen - 8 + i] != flagBits[i])
        {
            printf("\n[ERROR] Receiver detected corruption: Framing flags are broken!\n");
            return;
        }
    }

    // Destuffing
    j=0;
    ones=0;
    for(i=8; i<framedLen-8; i++)
    {
        destuffed[j++] = framed[i];

        if(framed[i]==1)
            ones++;
        else
            ones=0;

        if(ones==5)
        {
            if(i + 1 >= framedLen - 8 || framed[i+1] != 0)
            {
                printf("\n[ERROR] Receiver detected corruption: Missing stuffed '0' bit!\n");
                return;
            }
            i++;
            ones=0;
        }
    }

    if(j % 8 != 0)
    {
        printf("\n[ERROR] Receiver detected corruption: Stream size (%d bits) isn't a multiple of 8!\n", j);
        return;
    }

    printBits("Destuffed Data", destuffed, j);
    char recovered[MAX];
    binaryToString(destuffed, j, recovered);
    printf("Recovered Message : %s\n", recovered);
}

void byteStuffing(char msg[])
{
    int i, j, k, m;
    int len = strlen(msg);
    for(i=0; i<len; i++)
        dataBytes[i] = (unsigned char)msg[i];
    printBytes("\nOriginal Data", dataBytes, len);

    j=0;
    for(i=0; i<len; i++)
    {
        if(dataBytes[i]==flagByte || dataBytes[i]==escByte)
        {
            stuffedBytes[j++] = escByte;
            stuffedBytes[j++] = dataBytes[i];
        }
        else
        {
            stuffedBytes[j++] = dataBytes[i];
        }
    }
    int stuffedLen = j;
    printBytes("Stuffed Data", stuffedBytes, stuffedLen);

    /* Framing */
    k = 0;
    framedBytes[k++] = flagByte;
    for(i = 0; i < stuffedLen; i++)
        framedBytes[k++] = stuffedBytes[i];
    framedBytes[k++] = flagByte;
    int framedLen = k;
    printBytes("Framed Data", framedBytes, framedLen);

    // Error Injection
    char corruptChoice;
    printf("\nDo you want to corrupt the framed data? (y/n): ");
    scanf(" %c", &corruptChoice);
    if(corruptChoice == 'y' || corruptChoice == 'Y')
    {
        int startIndex, endIndex;
        printf("Enter the starting and ending index to corrupt (0 to %d): ", framedLen - 1);
        scanf("%d %d", &startIndex, &endIndex);

        if(startIndex >= 0 && endIndex < framedLen && startIndex <= endIndex)
        {
            for(i = startIndex; i <= endIndex; i++)
            {
                framedBytes[i] = (unsigned char)(~framedBytes[i]); // Invert all 8 bits
            }
            printf("--- Data Corrupted successfully! ---\n");
            printBytes("Corrupted Frame", framedBytes, framedLen);
        }
        else
        {
            printf("Invalid index range! No corruption injected.\n");
        }
    }

    /* Receiver Verification */
    if (framedBytes[0] != flagByte || framedBytes[framedLen - 1] != flagByte)
    {
        printf("\n[ERROR] Receiver detected corruption: Framing flags are compromised!\n");
        return;
    }

    m = 0;
    for(i = 1; i < framedLen - 1; i++)
    {
        if(framedBytes[i] == escByte)
        {
            if(i + 1 >= framedLen - 1)
            {
                printf("\n[ERROR] Receiver detected corruption: Out-of-bounds trailing escape byte!\n");
                return;
            }
            i++;
            if (framedBytes[i] != flagByte && framedBytes[i] != escByte)
            {
                printf("\n[ERROR] Receiver detected corruption: Invalid byte following escape signature!\n");
                return;
            }
            destuffedBytes[m++] = framedBytes[i];
        }
        else if(framedBytes[i] == flagByte)
        {
            printf("\n[ERROR] Receiver detected corruption: Unescaped FLAG inside message body!\n");
            return;
        }
        else
        {
            destuffedBytes[m++] = framedBytes[i];
        }
    }

    printBytes("Destuffed Data", destuffedBytes, m);

    char recovered[MAX];
    for(i = 0; i < m; i++)
        recovered[i] = (char)destuffedBytes[i];
    recovered[m] = '\0';
    printf("Recovered Message : %s\n", recovered);
}

int main()
{
    int choice;
    char message[MAX];
    while(1)
    {
        printf("\n1. Bit Stuffing\n");
        printf("2. Byte Stuffing\n");
        printf("3. Exit\n");
        printf("\nEnter your choice : ");
        if(scanf("%d", &choice) != 1) return 0;
        getchar();   // Clear input stream trailing characters

        switch(choice)
        {
            case 1:
                printf("\nEnter the message : ");
                fgets(message, sizeof(message), stdin);
                message[strcspn(message, "\n")] = '\0';
                bitStuffing(message);
                break;
            case 2:
                printf("\nEnter the message : ");
                fgets(message, sizeof(message), stdin);
                message[strcspn(message, "\n")] = '\0';
                byteStuffing(message);
                break;
            case 3:
                printf("\nExiting...\n");
                return 0;
            default:
                printf("\nInvalid Choice!\n");
        }
    }
    return 0;
}
