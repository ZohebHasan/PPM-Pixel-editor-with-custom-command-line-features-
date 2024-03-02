#include "hw2.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h> 

#define PPM 100
#define SBU 200

// #define MISSING_ARGUMENT 1
// #define UNRECOGNIZED_ARGUMENT 2
// #define DUPLICATE_ARGUMENT 3
// #define INPUT_FILE_MISSING 4
// #define OUTPUT_FILE_UNWRITABLE 5
// #define C_ARGUMENT_MISSING 6
// #define C_ARGUMENT_INVALID 7
// #define P_ARGUMENT_INVALID 8
// #define R_ARGUMENT_INVALID 9


char *inputFilePath = NULL;
char *outputFilePath = NULL;
bool containsI = false, containsO = false, containsC = false, containsP = false, containsR = false; 
int elementsOfC[4] = {0};
int elementsOfP[2] = {0};
char *elementsOfRstr[3] = {NULL};
int elementsOfRint[2] = {0};


bool containsRarg(int argumentsLength, char **argumentsArray);
int checkArgs(int argumentsLength, char **argumentsArray);
int checkFileType(char *filePath);
bool splitArgument(char *argument, char *option);
bool validFile(char *filePath, char option);
FILE *getFile(char *filePath, char task );


bool splitArgument(char *argument, char *option){
    if (argument == NULL) {
        return false;
    }
    else{
      int index = 0;
      char *token;
      token = strtok(argument, ","); 

      if(strcmp(option, "r") == 0){
        int count = 0;
        while (token != NULL) {
            if (count < 3) {
                elementsOfRstr[count] = token; 
            } 
            else if (count < 5) {
                elementsOfRint[index] = atoi(token);
                index++;
            }
            token = strtok(NULL, ",");
            count++;
        }
        return count == 5;
      }
      else if(strcmp(option, "p") == 0){ 
          while (token != NULL) {
              if (index < 2) {
                  elementsOfP[index] = atoi(token); //changed
              }
              token = strtok(NULL, ",");
              index++;
          }
          return index == 2;
      }
      else{
          while (token != NULL) {
              if (index < 4) {
                  elementsOfC[index] = atoi(token); 
              }
              token = strtok(NULL, ",");
              index++;
          }
              if (index != 4) {
                  return false;
              }
        return index == 4;
      }
    }
    return true;
}

int checkFileType(char *filePath) {
    char *fileExtension = strrchr(filePath, '.');

    if ((fileExtension == NULL) || (fileExtension == filePath)) {
        return -1; 
    } 
    else if (strcmp(fileExtension, ".ppm") == 0) {
        return PPM;
    } 
    else if (strcmp(fileExtension, ".sbu") == 0) {
        return SBU;
    } 
    return -1; 
}

bool validFile(char *filePath, char option){
  FILE *file;
  if(filePath == NULL){
    return false;
  }
  char mode[2] = {option, '\0'};
  file = fopen(filePath, mode);

  if(file == NULL){
    return false;
  }
  else if(checkFileType(filePath) == -1){
    return false;
  }

  fclose(file);
  return true;
}

FILE *getFile(char *filePath, char task ) { //gimmick
    FILE *file = NULL;
    if(validFile(filePath, task) == true){ 
         file = fopen(filePath, task == 'r' ? "r" : "w");
    }
    return file;
}

bool containsRarg(int argumentsLength, char **argumentsArray) {
    for (int i = 1; i < argumentsLength; i++) { 
        if (strcmp(argumentsArray[i], "-r") == 0) {
            if ((i + 1 >= argumentsLength) || (argumentsArray[i + 1][0] == '-')) {
                return false;
            }
        }
    }
    return true; 
}

int checkArgs(int argumentsLength, char **argumentsArray) {
    int option;
    int argCount = 0;
    int unrecongnizedArg = false;
    int iCt = 0, oCt = 0; 
    bool validInputFilePath = false, validInputFile = false,  validOutputFilePath = false, validCargument = false, validPargument = false, containsRargument = false, validRargument = false;

    while ((option = getopt(argumentsLength, argumentsArray, "i:o:c:p:r")) != -1) {
        switch (option) {
            case 'i':
                inputFilePath = optarg;
                if (validFile(inputFilePath, 'r') == true) {
                    validInputFile = true;   
                }
                if(checkFileType(inputFilePath) != -1){
                    validInputFilePath = true;
                }
                containsI = true;
                argCount++;
                iCt++;
                break;
            case 'o':
                outputFilePath = optarg;
                if (validFile(outputFilePath, 'w') == true) {
                    validOutputFilePath = true;      
                }
                containsO = true;
                argCount++;
                oCt++;
                break;
            case 'c':        
                if(splitArgument(optarg, "c") == true){
                    validCargument = true;
                }
                containsC = true;
                argCount++;
                break;
            case 'p':
                containsP = true;
                validPargument = splitArgument(optarg, "p");
                argCount++;
                break;
            case 'r':     
                containsR = true; 
                containsRargument = containsRarg(argumentsLength, argumentsArray);
                if(optarg == NULL){
                    validRargument = false;
                    argCount++;
                    break;
                }  
                if(splitArgument(optarg, "r") == true){
                    validRargument = true;
                }
                argCount++;
                break;
            default:
                unrecongnizedArg = true;
                break;
        }
    }

    if (inputFilePath == NULL || outputFilePath == NULL || argCount < 2) {
        return MISSING_ARGUMENT;
    } 
    else if (iCt < 1 || oCt < 1) {
        return MISSING_ARGUMENT;
    }
    else if(containsI && !validInputFilePath){
        return MISSING_ARGUMENT;
    }
    else if(containsR && !containsRargument){
        return MISSING_ARGUMENT;
    }
    else if (unrecongnizedArg) {
        return UNRECOGNIZED_ARGUMENT;
    } 
    else if (iCt > 1 || oCt > 1) {
        return DUPLICATE_ARGUMENT;
    } 
    else if (containsI && !validInputFile) {
        return INPUT_FILE_MISSING;
    } 
    else if (containsO && !validOutputFilePath) {
        return OUTPUT_FILE_UNWRITABLE;
    } 
    else if (containsP && !containsC) {
        return C_ARGUMENT_MISSING;
    } 
    else if (containsC && !validCargument) {
        return C_ARGUMENT_INVALID;
    } 
    else if (containsP && !validPargument) {
        return P_ARGUMENT_INVALID;
    } 
    else if(containsR && !validRargument){
        return R_ARGUMENT_INVALID;
    }
    else {
        return 0;
    }
}



int main(int argc, char **argv) {
    int returnValue = checkArgs(argc, argv);
    if (returnValue != 0) {
        return returnValue;
    }
  


    return 0;
}