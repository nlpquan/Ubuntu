#include <stdbool.h>
/**
 * Author: Quan Nguyen
 * resources: stackoverflow, Activity 4 guides
 **/

/**
 * Program to work with Decimal, Binary, and Hexidecimal
 */
int main(int argc, const char *argv[])
{
    int number = 0;
    bool ok = false;
    char bits[32] = {
        '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '0', '0', '0', '0', '0', '0', '0'};

    while (!ok)
    {
        printf("\nWelcome to Memory Management ");
        printf("\nEnter a page size (0 to 1000): ");
        scanf("%d", &number);
        if (number < 0 || number > 1000)
        {
            printf("This number needs to be between 0 and 1000\n");
        }
        else
        {
            ok = true;
        }
    }

    // Display number as a 32 bit binary
    printf("\nThe number in binary is: ");
    convertPrintBinary(number, &bits[0]);

    printf("The number in hexidecimal is: 0x%08X\n", number);

    int result = ((number << 10) & 0xFFFFFC00) | 0x03FF;

    printf("The result in decimal is: %d\n", result);
    printf("The result in hexidecimal is: 0x%08X\n", result);
    printf("The result in binary is: ");
    convertPrintBinary(result, &bits[0]);

    return 0;
}

void printBinary(char* bits)
{
    for(int x=31; x>=0; --x)
    {
        printf("%c", bits[x]);
    }
    printf("\n");
}

void clearBinaryBits(char* bits)
{
    decimalToBinary(0, &bits[0]);
}

void convertPrintBinary(int number, char* bits)
{
    clearBinaryBits(&bits[0]);
    decimalToBinary(number, &bits[0]);
    printBinary(&bits[0]);
}

void decimalToBinary(int number, char* bits)
{
    int index = 0;
    int result = number;
    while(result != 0)
    {
        int remainder = result % 2;

        if(remainder > 0)
        {
            bits[index] = '1';
        }
        result = result/2;
        ++index;
    }
}