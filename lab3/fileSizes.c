#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char **argv)
{
    char *dirPath = ".";

    DIR *dirHandle = opendir(dirPath);
    if (dirHandle == NULL)
    {
        printf("Could not open directory at path: %s \n", dirPath);
        return -1;
    }

    // construct beginning of the path file buffer
    size_t dirPathLen = strlen(dirPath);

    int PATH_BUFFER_SIZE = 1024;
    // check if path will fit in the buffer
    if (dirPathLen > PATH_BUFFER_SIZE)
    {
        printf("Path to the file doesn't fit in 1024-bytes buffer!");
        return -1;
    }
    char path_buffer[PATH_BUFFER_SIZE];

    memcpy(path_buffer, dirPath, dirPathLen);

    if (path_buffer[dirPathLen - 1] != '/')
        path_buffer[dirPathLen++] = '/';

    long long totalSize = 0;
    struct stat file_status;

    for (
        struct dirent *currFile = readdir(dirHandle); currFile != NULL; currFile = readdir(dirHandle))
    {
        size_t filenameLen = strlen(currFile->d_name);

        if (dirPathLen + filenameLen > PATH_BUFFER_SIZE)
        {
            printf("File path doesn't fit in 1024-bytes buffer!");
            return -1;
        }

        memcpy(&path_buffer[dirPathLen], currFile->d_name, filenameLen);
        path_buffer[dirPathLen + filenameLen] = 0;

        stat(path_buffer, &file_status);

        if (!S_ISDIR(file_status.st_mode))
        {
            totalSize += file_status.st_size;
            printf("File: %s, Size: %ld bytes\n", currFile->d_name, file_status.st_size);
        }
    }

    // print out total count of bytes
    printf("Total Size: %lld bytes\n", totalSize);

    // remember to close directory
    closedir(dirHandle);
    return 0;
}