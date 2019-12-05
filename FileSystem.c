/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FileSystem.c
 * Author: Owner
 *
 * Created on November 15, 2019, 1:27 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <string.h>
#define DISK_BLOCKS 16384      /* number of blocks on the disk */
#define BLOCK_SIZE 4096  
#define FAT_SIZE 8192
#include "disk.h"



/*
 * 
 */
//write
//save
//read
//delete vd file
//make another vd file
//try to see if the user can create a new text file
//save in fat of 8192 blocks
//write to the disk file
//read to the disk file

 //check if the virtual disk file is empty
//read the contents from the vd file if its not empty, parse into 2d array and blocks array
//read a single file
//create a directory
//changing directories and viewing contents of the directory
//get file size, lseek, truncate
int Change_Dir(char *name);
char *getTime_Date();
int fs_delete(char *name);
int fs_mkdir(char *name);
//int fs_read(int fildes, void *buf, size_t nbyte);
//int fs_write(int files, void *buf, size_t nbyte);
int fs_get_filesize(int fildes);
int fs_lseek(int fildes, off_t offset);
int fs_truncate(int fildes, off_t length);
int unmount_fs(char *disk_name);
int fs_open(char *name);
int mount_fs(char *disk_name);
int make_fs(char *disk_name);
int close_disk();
int block_write(int block, char *buf);
int block_read(int block, char *buf);
int open_disk(char *name);
int make_disk(char *name);
int fs_read(char *name);
void manual();
void TokenizedWords(char *line, char **TokenArray);
//void Exit();
int fs_close(char *name);
int fs_create(char *name);
//int make_disk(char *name);
int Blocks_Free();
int File_Exists(char *filename);

char *fat[FAT_SIZE][FAT_SIZE];
char *blocks[BLOCK_SIZE];
char *TokenArray[100];
char words[100];


static int active = 0;  /* is the virtual disk open (active) */
static int handle;  
//const char *buffer=malloc(BLOCK_SIZE);
int track=0;
char *directory="/"; 
int main(int argc, char** argv) {
    
    char *name="VirtualDisk";
    manual();
    make_fs(name);
    
    
    while(1){
        fgets (words, 100, stdin);
        TokenizedWords(words, TokenArray);
        
        if(strncmp(TokenArray[0], "create",6)==0){
            char user[50];
            strcpy(user, TokenArray[1]);
            user[strlen(TokenArray[1])-1]='\0';
            fs_create(user);
        }
        if(strncmp(TokenArray[0], "ls",2)==0){
        }
        if(strncmp(TokenArray[0], "newdir",6)==0){
            char user[50];
            strcpy(user, TokenArray[1]);
            user[strlen(TokenArray[1])-1]='\0';
            fs_mkdir(user);
        }
        
        if(strncmp(TokenArray[0], "exit",4)==0){
            char concat1[100];
            int i;
            for(i=track; i>0; i--){
                strcpy(concat1,fat[i][0]);
                strcat(concat1,"|");
                strcat(concat1,fat[i][1]);
                strcat(concat1,"|");
                strcat(concat1,fat[i][2]);
                strcat(concat1,"|");
                strcat(concat1, fat[i][3]);
                printf("%s\n", concat1);
                block_write(i, concat1);
        }
        /*
         for(int t=0; t<=track; t++){
            block_write(t+track, blocks[t]);
        }
        
        for(int t=0; t<FAT_SIZE; t++){
            block_write(t+FAT_SIZE, blocks[t]);
        }
         */
           printf("The program has exited\n");
           exit(0);
        //Exit();
         

    }
        if(strncmp(TokenArray[0], "del", 3)==0){
            char user[50];
            strcpy(user, TokenArray[1]);
            user[strlen(TokenArray[1])-1]='\0';
            printf("user=%s\n", user);
            fs_delete(user);
        }
        if(strncmp(TokenArray[0], "open", 4)==0){
            char user[50];
            strcpy(user, TokenArray[1]);
            user[strlen(TokenArray[1])-1]='\0';
            fs_open(user);
        }
        if(strncmp(TokenArray[0], "write", 5)==0){
            char user[50];
            strcpy(user, TokenArray[1]);
            user[strlen(TokenArray[1])-1]='\0';
            fs_write(user);
        }
        if(strncmp(TokenArray[0], "close", 5)==0){
            char user[50];
            strcpy(user, TokenArray[1]);
            user[strlen(TokenArray[1])-1]='\0';
            fs_close(user);
        }
        if(strncmp(TokenArray[0], "cd", 2)==0){
            char user[50];
            strcpy(user, TokenArray[1]);
            user[strlen(TokenArray[1])-1]='\0';
            Change_Dir(user);
        }
        if(strncmp(TokenArray[0], "read", 4)==0){
            char user[50];
            strcpy(user, TokenArray[1]);
            user[strlen(TokenArray[1])-1]='\0';
            fs_read(user);
        }
        
    }
    
}

void TokenizedWords(char *words, char **TokenArray){
    char *Token;
    Token = strtok(words, " ");
    while(Token != NULL){
        *TokenArray++ = Token;
        Token = strtok(NULL, " ");
        
    }
    *TokenArray = NULL;
}

int make_fs(char *disk_name){
    
    char *buffer=malloc(BLOCK_SIZE);
    make_disk(disk_name);
    if(open_disk(disk_name)==-1){
        
        return -1; 
    }
    else{
         //where I assume I check if the virtual file is empty or not.  
        for(int i=0; i<BLOCK_SIZE; i++){
        blocks[i]='\n';
        }
         for(int i=0; i<BLOCK_SIZE; i++){
            fat[i][0]="";
            fat[i][1]="";
            fat[i][2]="";
            fat[i][3]="";
        }
        fat[0][0]=malloc(20);
        strcpy(fat[0][0],"/");
        fat[0][1]=malloc(20);
        strcpy(fat[0][1],"N/A");
        fat[0][2]=malloc(100);
        strcpy(fat[0][2], getTime_Date());
        fat[0][3]=malloc(20);
        strcpy(fat[0][3], "Dir");
        return 0;
    }
}

int mount_fs(char *disk_name){
    if(open_disk(disk_name)==-1){
        return -1;
    }
    //mount()
    return 0;
}

int unmount_fs(char *disk_name){
    
}

int fs_open(char *name){
    if(File_Exists(name)==-1){
        return -1;
    }
    else{
        printf("The file %s has been opened. Below is it's metadata.\n",name);
        for(int i=0; i<=track; i++){
            if(strcmp(fat[i][0], name)==0){
                printf("%s\n", fat[i][0]);
                printf("%s\n", fat[i][1]);
                printf("%s\n", fat[i][2]);
                printf("%s\n", fat[i][3]);
            }
        }

    }
    
    return 0;
}

int fs_close(char *name){
    int file=File_Exists(name);
    if(file==-1){
        return -1;
    }
    else{
        printf("The file %s has been closed \n", name);
    }
    return 0;
}

int fs_create(char *name){
    char *input=malloc(200);
    char words[100];
    int freeblock= Blocks_Free();
    sprintf(words, "%d", freeblock);
    for(int i=0; i<track; i++){
        
        if(strcmp(fat[i][0], name)==0){
            return -1;
        }
    }
    if(strlen(name)>15){
        return -1;
    }
    track++;
    fat[track][0]=malloc(20);
    strcpy(fat[track][0], name);
    fat[track][1]=malloc(20);
    strcpy(fat[track][1], directory);
    fat[track][2]=malloc(70);
    strcpy(fat[track][2], getTime_Date());
    printf("%s", fat[track][2]);
    printf("\n");
    printf("What would you like to enter\n");
    scanf("%[^\n]%*c", input);
    if(freeblock!=-1){
        blocks[freeblock]= malloc(BLOCK_SIZE);
        strcpy(blocks[freeblock], input);
        fat[track][3]=malloc(100);
        strcpy(fat[track][3], words);
    }
    //remember to work on the condition if the file spans multiple blocks
   free(input);
   return 0;
}
int fs_delete(char *name){
    int file=File_Exists(name);
    int x;
    for(int i=0; i<=track; i++){
        if(file==-1 ){
            return -1;
        }
        
        else{
            if(strcmp(fat[i][0],name)==0){ 
                //puts("hello");
                sscanf(fat[i][3], "%d", &x);
                //puts("hello");
                strcpy(blocks[x], "\n");
                //puts("tello");
                fat[i][0]=malloc(20);
                //puts("fello");
                strcpy(fat[i][0]," ");
                //puts("sello");
                fat[i][0]=malloc(20);
                strcpy(fat[i][1]," ");
                fat[i][0]=malloc(20);
                strcpy(fat[i][2]," ");
                fat[i][3]=malloc(20);
                strcpy(fat[i][3],"N/A");
                
                
            }
        }
    }
            /*
            strcpy(fat[track][0],""); 
            strcpy(fat[track][1],"");
            strcpy(fat[track][2],"");
            strcpy(fat[track][3],"N/A");
            */
            track--;
    //}
    return 0;
}

int fs_mkdir(char *name){
    for(int i=0; i<track; i++){
        if(strcmp(fat[i][0], name)==0 ||strlen(name)>15){
            //||fat[i][0]>256
            return -1;
        }
    }
    track++;
    fat[track][0]=malloc(20);
    strcpy(fat[track][0],name); 
    fat[track][1]=malloc(20);
    strcpy(fat[track][1],"/");
    fat[track][2]=malloc(20);
    strcpy(fat[track][2],getTime_Date());
    fat[track][3]=malloc(20);
    strcpy(fat[track][3],"Dir");
    
    return 0;
}

int fs_read(char *name){
    int file=File_Exists(name);
    int x;
    //printf("%s\n", name);
    if(file!=-1){
        for(int i=0; i<=track; i++){
            if(strcmp(fat[i][0],name)==0){ 
                sscanf(fat[i][3], "%d", &x);
                printf("%s\n", blocks[x]);  
            }
        }
    }
    else{
        puts("The file you're trying to access does not exist");
    }
    return 0;
}
 
 


int fs_write(char *name){
    int file=File_Exists(name);
    int x;
    char input[250];
    if(file!=-1){
        for(int i=0; i<=track; i++){
            if(strcmp(fat[i][0],name)==0){
                printf("What would you like to write to %s\n", name);
                scanf("%[^\n]%*c", input);
                sscanf(fat[i][3], "%d", &x);
                strcat(blocks[x], " ");
                strncat(blocks[x], input, strlen(input));
            }
        }
    }
    return 0;
}



//int fs_read(int fildes, void *buf, size_t nbyte){
 
//}


//int fs_write(int files, void *buf, size_t nbyte){
    
//}

int fs_get_filesize(int fildes){
    
}

int fs_lseek(int fildes, off_t offset){
   
    return 0;
}

int fs_truncate(int fildes, off_t length){
    
    return 0;
}

int make_disk(char *name){ 
  int f, cnt;
  char buf[BLOCK_SIZE];

  if (!name) {
    fprintf(stderr, "make_disk: invalid file name\n");
    return -1;
  }

  if ((f = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
    perror("make_disk: cannot open file");
    return -1;
  }

  memset(buf, 0, BLOCK_SIZE);
  for (cnt = 0; cnt < DISK_BLOCKS; ++cnt)
    write(f, buf, BLOCK_SIZE);

  close(f);

  return 0;
}


void manual(){
    printf("create- creates a new file which also prompts you to write in it.\n");
    printf("cd - Change a directory.\n");
    printf("read- read from a file\n");
    printf("write- write to a file\n");
    //printf("ls - List the contents under a directory.\n");
    printf("newdir - Creates a new directory.\n");
    printf("exit - Exits the program.\n");
    printf("del - Deletes a directory or a file.\n");
    printf("Open - Call to open a file. Also, it will prompt you to either read or write to the file.\n");
    printf("Close - Call to close a file.\n");
}
/*
void Exit(){
    printf("\nThe program has exited\n");
    exit(0);
}
 */

int Change_Dir(char *name){
    for(int i=0; i<=track; i++){
        if(strcmp(fat[i][0],name)==0){
            printf("You are now under directory: %s\n", name);
            fs_open(name);
            
        }
    }
    return 0;
}

int Blocks_Free(){
    for(int i=0; i<BLOCK_SIZE; i++){
        if(blocks[i]=='\n'){
            return i;
        }
    }
    return -1;
}

int File_Exists(char *filename){
    for(int i=0; i<=track; i++){
        if(strcmp(fat[i][0], filename)==0){
            return i;
            }
    }
    return -1;
}

char* getTime_Date(){
    time_t dt;
    struct tm *dtholder ; 
    static char arr[50];
    time( &dt ); 
    dtholder=localtime(&dt);
    strftime(arr, sizeof(arr), "%x || %I:%M%p", dtholder);
    return arr;
}

int open_disk(char *name){
  int f;

  if (!name) {
    fprintf(stderr, "open_disk: invalid file name\n");
    return -1;
  }  
  
  if (active) {
    fprintf(stderr, "open_disk: disk is already open\n");
    return -1;
  }
  
  if ((f = open(name, O_RDWR, 0644)) < 0) {
    perror("open_disk: cannot open file");
    return -1;
  }

  handle = f;
  active = 1;

  return 0;
}


int block_read(int block, char *buf){
  if (!active) {
    fprintf(stderr, "block_read: disk not active\n");
    return -1;
  }

  if ((block < 0) || (block >= DISK_BLOCKS)) {
    fprintf(stderr, "block_read: block index out of bounds\n");
    return -1;
  }

  if (lseek(handle, block * BLOCK_SIZE, SEEK_SET) < 0) {
    perror("block_read: failed to lseek");
    return -1;
  }

  if (read(handle, buf, BLOCK_SIZE) < 0) {
    perror("block_read: failed to read");
    return -1;
  }

  return 0;
}

int block_write(int block, char *buf){
  if (!active) {
    fprintf(stderr, "block_write: disk not active\n");
    return -1;
  }

  if ((block < 0) || (block >= DISK_BLOCKS)) {
    fprintf(stderr, "block_write: block index out of bounds\n");
    return -1;
  }

  if (lseek(handle, block * BLOCK_SIZE, SEEK_SET) < 0) {
    perror("block_write: failed to lseek");
    return -1;
  }

  if (write(handle, buf, BLOCK_SIZE) < 0) {
    perror("block_write: failed to write");
    return -1;
  }

  return 0;
}

int close_disk(){
  if (!active) {
    fprintf(stderr, "close_disk: no open disk\n");
    return -1;
  }
  
  close(handle);

  active = handle = 0;

  return 0;
}

