
#include "database.h"

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>

#include "utils.h"

void create_db_directory(char *name, char *path) {
    if (!directory_exists(path + name)) {
        mkdir(path + name, S_IRWXU);
        make_full_path(path, name);
    }
    else {
        make_full_path(path, name);
    }
}

void recursive_rmdir(char *dirname) {
    int remove_directory(char const *dirname) {
        DIR *directory;           // pointeur de répertoire
        struct dirent *dirent;     // représente une entrée dans un répertoire
        struct stat file_stat;    // informations sur un fichier


        char tab[1024] = {0}; // Ce tableau servira à stocker le chemin d'accès complet des fichiers et dossiers

        directory = opendir(dirname); // On ouvre le dossier.
        if ( directory == NULL ) {
            fprintf(stderr, "Impossible d'ouvrir le répertoire %s\n", dirname);
            return 0;
        }

        while ( (dirent = readdir(directory)) != NULL ) {  // On boucle sur les entrées du dossier

            /* On "saute" les répertoires "." et "..". */
            if ( strcmp(dirent->d_name, ".") == 0 ||
                 strcmp(dirent->d_name, "..") == 0 ) {
                continue;
            }


            /* Construction du chemin d'accès du fichier en
             * concaténant son nom avec le nom du dossier
             * parent. On intercale "/" entre les deux. */
            snprintf(tab, 1024, "%s/%s", dirname, dirent->d_name);

            stat(tab, &file_stat);  // Récuperation des informations sur le fichier.

            if ( S_ISREG(file_stat.st_mode) ) {
                remove(tab);  // On est sur un fichier, on le supprime.

            }
            else if ( S_ISDIR(file_stat.st_mode) ) {

                remove_directory(tab);  // On est sur un dossier, on appelle cette fonction.
            }
        }

        closedir(directory);  // Fermeture du dossier

        remove(dirname);  // dossier vide, on le supprime

        return 1;
    }
}
