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
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#define DISK_BLOCKS 16383
#define BLOCK_SIZE 4096

/*
 * 
 */
void manual();
void TokenizedWords(char *line, char **TokenArray);
void Exit();
int fs_create(char *name);
int make_disk(char *name);
const char *fat[][256];
int main(int argc, char** argv) {
    char *TokenArray[100];
    char words[100];
    int FD = open("VD", O_RDWR|O_CREAT, 0644);
    manual();
    while(1){
        fgets (words, 100, stdin);
        TokenizedWords(words, TokenArray);
        
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


int fs_create(char *name){
    
}

void manual(){
    printf("cd - Change a directory.\n");
    printf("ls - List the contents under a directory.\n");
    printf("newdir - Creates a new directory.\n");
    printf("exit - Exits the program.\n");
    printf("del - Deletes a directory or a file.\n");
    printf("Open - Call to open a file. Also, it will prompt you to either read or write to the file.\n");
    printf("Close - Call to close a file.\n");
}

void Exit(){
    exit(0);
}

int make_disk(char *name){   
    int f, cnt;  
    char buf[BLOCK_SIZE];  
    if (!name){    
        fprintf(stderr, "make_disk: invalid file name\n");    
        return -1;  
    }  
    if ((f = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0){    
        perror("make_disk: cannot open file");    
        return -1;  
    }  
    memset(buf, 0, BLOCK_SIZE);  
    for (cnt = 0; cnt < DISK_BLOCKS; ++cnt){    
        write(f, buf, BLOCK_SIZE);
    }
    close(f);  
    return 0;
}
