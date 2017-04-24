#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
int file_list(char *path, char *word);
int isDirectory(char *path);
int findStringInAFile(char *fileName, char* string,FILE* out);
int main(int argc, char *argv[]) {
char ch;
int count=0;
int i;
FILE *out;	
	if (argc != 3) {
		printf("Usage : ./exe word directory");
		exit(0);
	}
	 
    file_list(argv[2],argv[1]);


	
	out=fopen("log.txt","r");
	while(!feof(out))
	{
	  fscanf(out,"%c",&ch);
	  if(ch == '\n')/*Counting lines to calculate total value*/
	  {
		count++;
	  }
	}
	fclose(out);
	out=fopen("log.txt","a");
	fprintf(out,"Total of %s found in all files : %d\n",argv[1],count-1);
	fclose(out);
}

int isDirectory(char *path) { 
	struct stat statbuf;
	if (stat(path, &statbuf) == -1)
		return 0;
	else
		return S_ISDIR(statbuf.st_mode);
}



int file_list(char *path, char *word) {
    int count = 0,i;
    DIR *dp ;
	pid_t pid;
	char newPath[256];/*MAX PATH LENGTH*/
	FILE *out;
    struct dirent *ep;
	 
	out=fopen("log.txt","a");
	

    dp = opendir(path);
    if(NULL == dp) {
        closedir(dp);
		return 0;
		
    }

   
   
    while( (ep = readdir(dp))!=NULL){

		snprintf(newPath,256,"%s/%s",path,ep->d_name);
		
		
        if ((pid = fork()) < 0)
        	printf("Fork error\n");

		if(pid==0)
		{
			
			count++;
		
/*If it is not a directory and not ending with ~ to prevent reading .txt~ files*/
        	if(isDirectory(newPath)==0 && ep->d_name[strlen(ep->d_name)-1] != '~'){
				
				findStringInAFile(newPath,word,out);
				closedir(dp);
				fclose(out);
				exit(0);
			}/*If it is a directory. Excluding the .. and .*/
            else if(strcmp(ep->d_name,"..")!=0 && strcmp(ep->d_name,".")!=0 ){
				file_list(newPath,word);
				closedir(dp);
				fclose(out);
                exit(0);
			}
			else{
				closedir(dp);
				fclose(out);
				exit(0);
							
			}
		}
	
		
    }
/*for(i=0; i<count; i++)
wait(NULL);*/

while (wait(NULL) != -1 || errno != ECHILD) {
    // nothing
}
	
	
	fclose(out);


	closedir(dp);
    return 1;
}

int findStringInAFile(char *fileName, char* string,FILE* out){

int size = strlen(string); //stores the size of the string

char letterRead;
int i,j,countColumn=0,countRow=1,flag,rowIndex,colIndex,total=0;
	
	
	
	

	FILE* fp;
	if((fp = fopen(fileName, "r")) == NULL) { //opens the file with the given
		printf("File Not Found \n");		  //name. If not found prints an 
		return 0;							  //error and returns 0.
	}



	while(fscanf(fp,"%c", &letterRead)!= EOF){ //reads till the EOF.
		
		if(letterRead == '\n'){ //if the char read is newline resets column
			countColumn=0;		//and increases row by 1
			countRow++;

		}
		else if(letterRead == string[0]){ //if the char read is equal to the
			countColumn++;				  //first character of the word
			rowIndex=countRow;			  //increases column and stores the indx
			colIndex=countColumn;
			flag =0;
			
			for(i=1,j=0; i<size && flag!=-1; j++) //j holds how many chars read
			{									  //i is to not to read more 
												  //size.
				
				if(fscanf(fp,"%c", &letterRead)!= EOF){ 
				
					

					if(letterRead == string[i]){//if it is equal to the 
					i++;						//corresponding letter of word
												//increases i
					
					
					}
					else if(letterRead != '\n' && letterRead != ' ' //ignores
							&& letterRead != '\t')
						flag=-1;

					
				}

				else
					break;
			}
			
			fseek(fp,(-j)*sizeof(char),SEEK_CUR); //pulls the fp j times back
			
			if(flag!=-1 && i==size){
			fprintf(out,"In file : %s [%d,%d] %s found\n",fileName,rowIndex,colIndex,string);
			total++;
			
			}

		}
		else
			countColumn++;

		


	}
		
fclose(fp);
return total;
}
