
#include "utils.h"
#include <dirent.h>
#include <string.h>

/*!
 *  \brief make_full_path concatenates path and basename and returns the result
 *  \param path the path to the database directory basename, can be NULL (i.e. path is current directory)
 *  Path may end with '/' but it is not required.
 *  \param basename the database name.
 *
 *  \return a pointer to the full path. Its content must be freed by make_full_path caller.
 */

char *make_full_path(char *path, char *basename) { // on concatène et on met un '/' en fin, ex : /home/user/bdd/
    if (*path[strlen(path)-1] == '/') {
        strcat(path , basename);

        if (*path[strlen(path)-1] != '/') {
            strcat(path , "/");
        }

    }
    else {
        strcat(path , "/");
        strcat(path , basename);

        if (*path[strlen(path)-1] != '/') {
            strcat(path , "/");
        }
    }
    return NULL;
}

bool directory_exists(char *path) {
    DIR *my_dir = opendir(path);
    if (my_dir) {
        closedir(my_dir);
        return true;
    }
    return false;
}

int table_exists(char *table_name, char *path) {


    DIR *my_dir = opendir(path + table_name);
    if (my_dir) {
        closedir(my_dir);
        return 0;
    }
    return 1;
}

long decimalToBinary(long long decimalnum) {  // conversion de integer en binaire
    long binarynum = 0, rem, temp = 1;

    while (decimalnum!=0)
    {
        rem = decimalnum%2;
        decimalnum = decimalnum / 2;
        binarynum = binarynum + rem*temp;
        temp = temp * 10;
    }
    return binarynum;
}

long floatToBinary(long long decimalnum) {  // conversion de float en binaire

}

long Reverse(long long n)
{
    long long sum=0;
    while (n!=0)
    {
        sum = sum*10 + n%10;
        n /= 10;
    }
    return sum;
}
