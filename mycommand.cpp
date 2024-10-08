#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>

#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <thread>
#include <vector>
#include <cstring>

using namespace std;

char *program_name = NULL;

void print_usage()
{
   fprintf(stderr, "Usage: %s [-a] [-o new filename] [-f filename] string1 [string2]\n", program_name);
   exit(EXIT_FAILURE);
}

int isDirectoryExists(const char *path)
{
   struct stat stats;

   stat(path, &stats);

   if (S_ISDIR(stats.st_mode))
      return 1;

   return 0;
}

bool file_exists(const char *filename)
{
   struct stat buffer;
   return stat(filename, &buffer) == 0 ? true : false;
}

void searchingFile(char *buffer, char *name, char *completeDirect, int bufSize)
{
   strncpy(strncpy(buffer, "/", bufSize), name, bufSize);

   if (file_exists(buffer))
   {
      printf("\e[0;32mFinding File with -R: %s exists in directory %s\n\e[0m", name, completeDirect);
   }
   else
   {
      printf("\e[0;31mFinding File with -R: %s doesn't exist in directory %s\n\e[0m", name, completeDirect);
   }
}

int main(int argc, char *argv[])
{
   int c;
   int error = 0;
   // int cOptionR = 0;

   program_name = argv[0];

   const size_t size = 1024;
   char buffer[size];
   char completeDirect[size];
   // char tmpCompleteDirect[size];
   vector<thread> tVect;

   while ((c = getopt(argc, argv, "R")) != EOF)
   {
      switch (c)
      {
      case 'R':
         printf("%s: [-R] Recursive Mode\n", program_name);

         strcpy(buffer, argv[optind]);
         optind++;

         if (isDirectoryExists(buffer) == 1)
         {

            strncpy(completeDirect, buffer, sizeof(completeDirect));

            while (optind < argc)
            {
               tVect.emplace_back(searchingFile, buffer, argv[optind], completeDirect, sizeof(buffer));
               optind++;
            }

            for (auto &t : tVect)
            {
               t.join();
            }
         }

         break;
      case '.':
         // error = 1;
         break;
      default:
         assert(0);
      }
   }
   if (error == 1)
   {
      print_usage();
   }
   if ((argc < optind + 1) || (argc > optind + 2))
   {

      strcpy(buffer, argv[optind]);
      optind++;

      if (isDirectoryExists(buffer) == 1)
      {
         strncpy(completeDirect, buffer, sizeof(completeDirect));

         while (optind < argc)
         {
            tVect.emplace_back(searchingFile, buffer, argv[optind], completeDirect, sizeof(buffer));
            optind++;
         }

         for (auto &t : tVect)
         {
            t.join();
         }
      }else{
         printf("directory doesnt exist\n");
         return EXIT_FAILURE;
      }
   }

   while (optind < argc)
   {

      print_usage();
      optind++;
   }
   return EXIT_SUCCESS;
}
