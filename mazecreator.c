/* Maze creator in C */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Use char 219;

typedef struct Maze {
    int* data;
    int width;
    int height;
} Maze;

void addToArray(Maze array, int x, int y, int value);
int getArrayValue(Maze array, int x, int y);
Maze createMaze(int width, int height);
void printMaze(Maze maze);
int invertFacing(int x);
Maze generateMaze (Maze maze, int startX, int startY);
Maze createEmptyMaze (int width, int height);
Maze createMazeFromFile (char* fileName, int startX, int startY);
void writeMazeToFile(Maze m, char* fileName, int prettyBool);
char* getPrintCharacter(int value, int prettyBool);

int main(int argc, char *argv[]) {

    srand(time(NULL)); // Set random seed.

    if (argc == 2) {
        printMaze(createMazeFromFile(argv[1], 1, 1));
    } else if(argc == 4) {
        writeMazeToFile(createMazeFromFile(argv[1], atoi(argv[2]), atoi(argv[3])), "OutputMaze.txt", 0);
    } else {
        printMaze(createMaze(63, 17));
    }


    return 0;
}


Maze createMazeFromFile (char* fileName, int startX, int startY) {
    printf("Started Generating maze from file.\n");
    FILE* pFile = fopen(fileName, "r");

    char* doc = malloc(1000 * 1000 * sizeof(char));

    char line[1000];

    while(!feof(pFile)) {
        fgets(line, 1000, pFile);
        strcat(doc, line);
    }

    fclose(pFile);

    int width = 0;

    char* pDoc = doc;


    while(*pDoc != '\n') {
        width++;
        pDoc++;
    }
    int height = 1;
    pDoc = doc;
    while (*pDoc != '\0') {
        if (*pDoc == '\n') {
            height++;
        }
        pDoc++;
    }

    int size = width * height;

    printf("\tCalculated maze dimensions of %d by %d (Size of %d).\n", width, height, size);

    Maze maze = createEmptyMaze(width, height);

    int x = 0;
    int y = 0;

    while(*doc != '\0') {
        if (*doc == '#') {
            addToArray(maze, x, y, 1);
        } else if (*doc == '@'){
            addToArray(maze, x, y, 0);
        } else if(*doc == '\n') {
            y++;
            x = -1;
            if (y%15 == 0) {
            printf("\tUp to line %d of the maze file out of %d.\n", y+1, height);
        }
        }
        
        x++;
        doc++;
    }

    free(doc);

    printf("Done reading file. Now creating maze.\n");

    return generateMaze (maze, startX, startY);
}

Maze createEmptyMaze (int width, int height) {
    Maze maze;
    
    maze.data = malloc(width*height*sizeof(int));
    maze.width = width;
    maze.height = height;

    return maze;
}

Maze createMaze (int width, int height) {
    Maze maze = createEmptyMaze (width, height);

    // Sets all pixels to be a wall.
    for (int y=0; y < height; y++) {
        for(int x=0; x < width; x++) {

            addToArray(maze, x, y, 1);
        }
    }

    return generateMaze (maze, 1, 1);
}


Maze generateMaze (Maze maze, int startX, int startY) {

    int posX = startX;
    int posY = startY;

    // Add start to the maze.
    addToArray(maze, startX, startY, 2);

    int count = 0;


    do {

        // Validate moves.
        int possibleNumbers[4] = {-1, -1, -1, -1};
        int nextIndex = 0;

        if (posY-2 >= 0 && getArrayValue(maze, posX, posY-2) == 1) {
            possibleNumbers[nextIndex] = 4;
            nextIndex++;
        }
        if (posX+2 < maze.width && getArrayValue(maze, posX+2, posY) == 1) {
            possibleNumbers[nextIndex] = 5;
            nextIndex++;
        }
        if (posY+2 < maze.height && getArrayValue(maze, posX, posY+2) == 1) {
            possibleNumbers[nextIndex] = 6;
            nextIndex++;
        }
        if (posX-2 >= 0 && getArrayValue(maze, posX-2, posY) == 1) {
            possibleNumbers[nextIndex] = 7;
            nextIndex++;
        }

        int numberOfValidElements = 0;
        for (int i=0; i < 4; i++) {
            if (possibleNumbers[i] != -1) {
                numberOfValidElements++;
            }
        }

        // Dead End Check.
        if (numberOfValidElements == 0) {
            int goTo = getArrayValue(maze, posX, posY);

            addToArray(maze, posX, posY, 8);

            // Dead End.
            if (goTo == 4) {
                posY -= 2;
                //addToArray(maze, posX, posY-1, 0);
                
            } else if (goTo == 5) {
                posX += 2;
                //addToArray(maze, posX+1, posY, 0);
                
            } else if (goTo == 6) {
                posY += 2;
                //addToArray(maze, posX, posY+1, 0);
                
            } else if (goTo == 7) {
                posX -= 2;
                
            }

            continue;
        }


        int randDir = rand() % numberOfValidElements; // Random number between 4, 5, 6, 7.

        int facingValue = possibleNumbers[randDir];

        if (facingValue == 4) {
            // North
            addToArray(maze, posX, posY-1, 0);
            posY -= 2;
        } else if(facingValue == 5) {
            // East
            addToArray(maze, posX+1, posY, 0);
            posX += 2;
        } else if(facingValue == 6) {
            // South
            addToArray(maze, posX, posY+1, 0);
            posY += 2;
        } else if(facingValue == 7) {
            // West
            addToArray(maze, posX-1, posY, 0);
            posX -= 2;
        }

        int invertedFace = invertFacing(facingValue);


        addToArray(maze, posX, posY, invertedFace);

        
        count++;

        if (count % 1000 == 0) {
            printf("%d iterations done so far.\n", count);
            //printMaze(maze);
        }
        //printMaze(maze);

    } while (getArrayValue(maze, posX, posY) != 2);

    printf("Done in %d iterations.\n\n", count);

    // Replace all dead ends with a floor.
    for (int y=0; y < maze.height; y++){
        for (int x=0; x < maze.width; x++) {
            int value = getArrayValue(maze, x, y);
            if (value == 8) {
                addToArray(maze, x, y, 0);
            } else if (value == 2) {
                addToArray(maze, x, y, 0);
            } else if (value == 3) {
                addToArray(maze, x, y, 0);
            }
            
        }
    }

 
    return maze;
}

int invertFacing(int x) {
    int newNum = x + 2;

    if (newNum > 7) {
        newNum -= 4;
    }
    return newNum;
}

void addToArray(Maze maze, int x, int y, int value){

    int* arrayPointer = maze.data;

    arrayPointer += x;

    arrayPointer += y*maze.width;

    *arrayPointer = value;
}

int getArrayValue(Maze maze, int x, int y){

    int* arrayPointer = maze.data;

    arrayPointer += x;

    arrayPointer += y*maze.width;

    return *arrayPointer;
}

void writeMazeToFile(Maze m, char* fileName, int prettyBool) {
    FILE* pFile = fopen(fileName, "w");

    int value;

    for (int y=0; y < m.height; y++) {
        for (int x=0; x < m.width; x++) {
            value = getArrayValue(m, x, y);
            fputs(getPrintCharacter(value, prettyBool), pFile);
        }
        fputs("\n", pFile);
    }
    

    fclose(pFile);
}

void printMaze(Maze maze) {

    /*

        4
      7 + 5
        6

    8 = deadend.

    */

    int value;

    for (int y=0; y < maze.height; y++) {
        for(int x=0; x < maze.width; x++) {
            value = getArrayValue(maze, x, y);

            printf("%s", getPrintCharacter(value, 1));
            
        }
        printf("\n");
    }
    printf("\n");
}


char* getPrintCharacter (int value, int prettyBool) {

    char* result = malloc(3 * sizeof(char));

    if (prettyBool){
        if (value == 1) {
            result = "██";
        } else if (value == 0) {
            result = "  ";
        } else if (value == 2) {
            // Start
            result = " @";
        } else if (value == 3) {
            // End
            result = " &";
        } else if (value == 4) {
            printf(" ^");
        } else if (value == 5) {
            printf(" >");
        } else if (value == 6) {
            printf(" v");
        } else if (value == 7) {
            printf(" <");
        } else if (value == 8) {
            printf(" X");
        }
    } else { 
        if (value == 0) {
            result = "@";
        } else if (value == 1) {
            result = "#";
        }
    }

    return result;
}
