#include <stdio.h>

int min(int a, int b)
{
    return a < b ? a : b;
}

int main(int argc, char const *argv[])
{
    printf("argc: %s\n", argv[1]);
    printf("argc: %s\n", argv[2]);
    FILE *in_file = fopen(argv[1], "r");
    FILE *out_file = fopen(argv[2], "a+");

    if (!in_file || !out_file)
    {
        printf("Error opening file\n");
        return 1;
    }
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    fseek(in_file, 0, SEEK_END);
    printf("hi");

    for (long bytesLeft = ftell(in_file); bytesLeft > 0; fseek(in_file, -BUFFER_SIZE, SEEK_CUR))
    {
        printf("bytesLeft: %ld\n", bytesLeft);
        long bytesToRead = min(bytesLeft, BUFFER_SIZE);
        fseek(in_file, -bytesToRead, SEEK_CUR);

        size_t readBytes = fread(buffer, sizeof(char), bytesToRead, in_file);

        char c;
        for (int i = 0; i < readBytes / 2; i++)
        {
            c = buffer[i];
            buffer[i] = buffer[readBytes - i - 1];
            buffer[readBytes - i - 1] = c;
        }
        fwrite(buffer, sizeof(char), readBytes, out_file);

        bytesLeft -= readBytes;
    }

    fclose(in_file);
    fclose(out_file);
    return 0;
}
