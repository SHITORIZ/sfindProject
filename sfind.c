#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
/*Function to find '.' and '..'*/
int Dots(char const* name)
{
   return (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 );
}
/*This is the compare function for the qsort() function which will compare the file names and return proper one*/
int compare(const void *node1, const void *node2)
{
   struct dirent *ptr1 = *(struct dirent * const *)node1;
   struct dirent *ptr2 = *(struct dirent * const *)node2;
   return strcmp(ptr1->d_name, ptr2->d_name);
}
/*This function is my scandir() function basically that iterates through the files/directories and store them into a array of character pointers*/
/*It builds my data structure and returns the number of files/directories found */
int List(char const *dirname, struct dirent ***current, DIR* dirp)
{
   
   int i;
   int limit;
   struct dirent* list;	
   limit = 50;
   current[0] = (struct dirent**)malloc(sizeof(struct dirent*)*limit);

   i = 0;
   while((list = readdir(dirp)) != NULL)
   {
         if(i == limit)
         {
            limit += 50;
            current[0] = (struct dirent**)realloc(current[0], sizeof(struct dirent*)*limit);
				if(current[0] == NULL)
				{
					perror("Realloc error\n");
					exit(1);
				}

            current[0][i] = list;
         }
         else
         {
            current[0][i] = list;
         }
         
         i++;
   }
 
   current[0][i] = NULL;
   qsort(current[0], i, sizeof(struct dirent*), compare);
  
   return i;

}
/*This function prints out the paths for the file when the -print flag is zero*/
void Print_Paths(struct dirent *current, char const *dirname)
{
   char *subdir2;
   if(!(Dots(current->d_name)))
         {
            
            
            if(current->d_type != DT_DIR || current->d_type == DT_DIR)
            {
               subdir2 = malloc(strlen(dirname) + strlen(current->d_name) + 2);

              strcpy(subdir2, dirname);
              strcat(subdir2, "/");
              strcat(subdir2, current->d_name);
         
               printf("%s\n", subdir2);
               free(subdir2);
            }
            
         }
}
/*This function prints out the file paths that have the specific substring passed in through command line*/
void Print_with_substr(struct dirent *current, char const *dirname, char const *substr)
{
   char *subdir2;
   if(!(Dots(current->d_name)) && strstr(current->d_name, substr) != NULL)
         {
            
            if(current->d_type != DT_DIR || current->d_type == DT_DIR)
            {
               subdir2 = malloc(strlen(dirname) + strlen(current->d_name) + 2);

              strcpy(subdir2, dirname);
              strcat(subdir2, "/");
              strcat(subdir2, current->d_name);
         
               printf("%s\n", subdir2);
               free(subdir2);
            }
            
         }
}
/*This function goes through and executes the commands for all files with a certain substring*/
void Execution_with_substr(struct dirent *current, char *dirname, char **argv, int argc, char *substr)
{
   int status;
   pid_t pid2;
   int i;
	int start;
	char *subdir2;
   if(strcmp(argv[2], "-exec") == 0)
   {
      start = 3;
   }
   else if(strcmp(argv[4], "-exec") == 0)
   {
      start = 5;
   }
   
   argv[argc - 1] = NULL;
   
   if(!(Dots(current->d_name)) && strstr(current->d_name, substr) != NULL)
         {
            
            if(current->d_type != DT_DIR || current->d_type == DT_DIR)
            {

               subdir2 = malloc(strlen(dirname) + strlen(current->d_name) + 2);

              strcpy(subdir2, dirname);
              strcat(subdir2, "/");
              strcat(subdir2, current->d_name);
              i = 0;
              while(argv[i] != NULL)
              {
                  if(strcmp(argv[i], "{}") == 0)
                  {
                     argv[i] = subdir2;
                  }
                  i++;
              }
              
				   if((pid2 = fork()) == 0)
					{
						execvp(argv[start], &argv[start]);	
					}
               else 
               {
                  waitpid(pid2, &status, 0);
                  i = 0;
                  while(argv[i] != NULL)
                  {
                     if(strcmp(argv[i], subdir2) == 0)
                     {
                        argv[i] = "{}";
                     }
                     i++;
                  }
                  
                  free(subdir2);

               }
               
            }
            
         }
   

}
/*This function executes the commands for all files from root directory*/
void Execution(struct dirent *current, char *dirname, char **argv, int argc)
{
   int status;  
   pid_t pid;
   int i;
	int start;
	char *subdir2;

	


   if(strcmp(argv[2], "-exec") == 0)
   {
      start = 3;
   }
   else if(strcmp(argv[4], "-exec") == 0)
   {
      start = 5;
   }
   
   argv[argc - 1] = NULL;
   
   if(!(Dots(current->d_name)))
         {
            
            if(current->d_type != DT_DIR || current->d_type == DT_DIR)
            {

               subdir2 = malloc(strlen(dirname) + strlen(current->d_name) + 2);

              strcpy(subdir2, dirname);
              strcat(subdir2, "/");
              strcat(subdir2, current->d_name);
				/*
             i = 0;
			 	while(strcmp(argv[i], ";") != 0)
				{
					i++;
				}
				argv[i] = NULL;
				*/
				
				
				
					
				  i = 0;
              while(argv[i] != NULL)
              {
                  if(strcmp(argv[i], "{}") == 0)
                  {
                     argv[i] = subdir2;
						
                  }
                  i++;
              }
              
               if((pid = fork()) == 0)
               { 
                     execvp(argv[start], &argv[start]);
               }
               else 
               {
                  waitpid(pid, &status, 0);
                  i = 0;
                  while(argv[i] != NULL)
                  {
                     if(strcmp(argv[i], subdir2) == 0)
                     {
                        argv[i] = "{}";
                     }
                     i++;
                  }
                  
                  free(subdir2);

               }
               
            }
            
         }
 	
}
/*This function is the main one, opens up the directories and depending on the flag will call certian functions for printing and executing commands*/
void PrintTree(char *dirname, int P_FLAG, int SUB_FLAG, int EXEC_FLAG, char *substr, char **argv, int argc)
{
	char* subdir;
   int i;
   int n;
   DIR* dirp;
   
   struct dirent **current;
  
 if(!(dirp = opendir(dirname)))
	{

		printf("%s\n", dirname);	
		exit(1);
		
	}

   n = List(dirname, &current, dirp);

   for(i = 0; i < n; i++)
   {
         
      if(P_FLAG == 0)
      {
         if(SUB_FLAG == 0)
         {
            Print_with_substr(current[i], dirname, substr);
         }
         else
         {
            Print_Paths(current[i], dirname);
         }
         
      }
      else if(EXEC_FLAG == 0)
      {
         if(SUB_FLAG == 0)
         {
               Execution_with_substr(current[i], dirname, argv, argc, substr);
         }
         else
         {
              Execution(current[i], dirname, argv, argc);
               
         }
      }
	
      
      if (current[i]->d_type == DT_DIR && !(Dots(current[i]->d_name)) )
      {
         
         subdir = malloc(strlen(dirname) + strlen(current[i]->d_name) + 2);

         strcpy(subdir, dirname);
         strcat(subdir, "/");
         strcat(subdir, current[i]->d_name);

         
         PrintTree(subdir, P_FLAG ,SUB_FLAG, EXEC_FLAG, substr, argv, argc);
         free(subdir);
         
      }
      
   }
	free(current);
 	closedir(dirp);
}
/*Main function that will go through all command line arguments and set flags accordinly before calling the main functin*/
/*Is also responsible for the error handling to make sure user inputs proper command line arguments format*/
int main(int argc, char* argv[])
{
	char *dirname;
   char *substr;
	DIR *dirp;  
   int P_FLAG;
   int SUB_FLAG;
   int EXEC_FLAG;
   P_FLAG = 1;
   SUB_FLAG = 1;
   EXEC_FLAG = 1;


	if(argc < 2)
	{
      
      fprintf(stderr, "FORMAT ERROR: ./sfind filename [-name str] -print | -exec cmd ;\n");
      return 1;
	}
	else
	{
      if(argc == 3)
      {
         if(strcmp(argv[2], "-print") == 0)
         {
           P_FLAG = 0;
         }
         else
         {
            fprintf(stderr,"FORMAT ERROR: ./sfind filename [-name str] -print | -exec cmd ;\n");
            return 1;
         }
      }
      else if(argc > 3)
      {
         
         if(strcmp(argv[2], "-name") == 0)
         {
            substr = argv[3];
            SUB_FLAG = 0;
            if(strcmp(argv[4], "-print") == 0)
            {
               P_FLAG = 0;
            }
            else if(strcmp(argv[4], "-exec") == 0)
            {

               EXEC_FLAG = 0;
               if(strcmp(argv[argc - 1], ";") != 0)
               {
                  fprintf(stderr,"FORMAT ERROR: No ; at end of commands\n");
                  return 1;
               }
            }
         }
         else if(strcmp(argv[2], "-exec") == 0)
         {
            EXEC_FLAG = 0;
            
            if(strcmp(argv[argc - 1], ";") != 0)
               { 
                  fprintf(stderr, " FORMAT ERROR: No ; at end of commands\n");
                  return 1;
               }
      
         }
         
      }
      
			dirname = argv[1];
         if(P_FLAG == 0 || EXEC_FLAG == 0)
         {
				if(dirp = opendir(dirname))
				{
					printf("%s\n", dirname);
				}
				closedir(dirp);
            PrintTree(dirname,P_FLAG, SUB_FLAG, EXEC_FLAG, substr, argv, argc);
         }
			else
			{
				fprintf(stderr, "FORMAT ERROR: ./sfind filename [-name str] -print | -exec cmd ;");
			}
         

	}
   


	return 0;
}
