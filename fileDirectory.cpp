#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>
#include <stack>
using namespace std;

#define MAX 50

struct Directory {

	struct Directory *parent;
	
	struct Directory *folder[MAX];
	char folderNames[MAX][20];
	int folderCount;

	char *fileContent[MAX];
	char fileNames[MAX][20];
	int fileCount;
};

struct Directory *root = NULL;
struct Directory *currDir = root;

void help(){
    printf("\n\
EXIT:\t\texit\n\
MKDIR:\t\tmkdir <new directory name> || mkdir <path>/<new directory name> || mkdir root:/<new directory name>\n\t\tmkdir root:/<path>/<new directory name> || mkdir <new directory name> <new directory name> ... \n\
RMDIR:\t\trmdir <directory name> || rmdir <path>/<directory name>\n\
CD:\t\tcd <directory name> || cd <path>\n\
LS:\t\tls <path>\n\
TOUCH:\t\ttouch <file name> || touch <path>/file name\n\
RM:\t\trm <file name> || rm <path>/<file name>]\n\
RETOUCH:\tretouch <path>\n\
CAT:\t\tcat <file name> || cat <path>/<file name> || cat <new file content> > <file name>\n\n\
In order to go to the root directory mention \"root:\" specifically.\n");
}

char *mystrlower (char *str) // custom function to change all the letters of a command into lower case.
{
	char *res = (char *)malloc((strlen(str) + 1) * sizeof(char));
	int i = 0;
	while (str[i] != '\0')
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
			res[i] = str[i] + 32;
		else
			res[i] = str[i];
		i++;
	}
	res[i] = '\0';
	return res;
}

char *mystrrev (char *str)
{
	char *res = (char *)malloc((strlen(str) + 1) * sizeof(char));
	for (int i = 0; i < strlen(str); i++)
		res[strlen(str)-i-1] = str[i];
	res[strlen(str)] = '\0';
	return res;
}

string findDirectoryName (struct Directory *dirToFind)
{
	if (dirToFind->parent == NULL)
		return "root";
	
	for (int i = 0; i < dirToFind->parent->folderCount; i++)
		if (dirToFind == dirToFind->parent->folder[i])
			return dirToFind->parent->folderNames[i];
	
	return "";
}

struct Directory *findDirectory(char* dirName)
{
	for (int i = 0; i < currDir->folderCount; i++)
		if (strcmp(mystrlower(currDir->folderNames[i]), mystrlower(dirName)) == 0)
			return currDir->folder[i];
	return NULL;
}

int findDirectoryIndex (struct Directory *dirToFind)
{
	struct Directory *parent = dirToFind->parent;
	int index = 0;
	for (int i = 0; i < parent->folderCount; i++)
	{
		if (dirToFind == parent->folder[i])
			return index;
		index++;
	}
	return -1;
}

int findFileIndex (char *fileName)
{
	for (int index = 0; index < currDir->fileCount; index++)
	{
		if (strcmp(currDir->fileNames[index], fileName) == 0)
			return index;
	}
	return -1;
}

bool SpecialCharacter(char *name)
{
	int i = 0;
	while(name[i] != '\0')
	{
		if((name[i] == '/' && name[i+1] == '/') || name[i] == '.' || name[i] == ':')
			return true;
		i++;
	}
	return false;
}

void printPrompt()
{
	struct Directory *temp = currDir;
	if (temp->parent == NULL)
	{
		printf("\nroot:/> ");
		return;
	}

	stack <string> dirPath;
	while (temp->parent)
	{
		dirPath.push(findDirectoryName(temp));
		temp = temp->parent;
	}

	printf("\nroot:");
	while (dirPath.empty() != true)
	{
		cout << "/" << dirPath.top();
		dirPath.pop();
	}
	printf("> ");
}

bool folderExists(char *name)
{
	for (int i = 0; i < currDir->folderCount; i++)
		if (strcmp(mystrlower(name), mystrlower(currDir->folderNames[i])) == 0)
			return true;
	return false;
}

bool fileExists(char *name)
{
	for (int i = 0; i < currDir->fileCount; i++)
		if (strcmp(mystrlower(name), mystrlower(currDir->fileNames[i])) == 0)
			return true;
	return false;
}

bool cd (char* path)
{
	struct Directory *initialDir = currDir;
	char *token = strtok (path, "/");	
	
	if (strcmp(mystrlower(token), "root:") == 0)
		currDir = root;

	else if (strcmp(mystrlower(token), ".") == 0)
		currDir = currDir;

	else if (strcmp(mystrlower(token), "..") == 0)
	{
		if (currDir != root)
			currDir = currDir->parent;
	}

	else if (findDirectory(token))
		currDir = findDirectory(token);
	
	else
	{
		printf("The system cannot find the path specified.\n");
		return false;
	}
	
	token = strtok (NULL, "/");
	while (token)
	{
		if (!findDirectory(token))
		{
			printf("The system cannot find the path specified.\n");
			currDir = initialDir;
			return false;
		}
		
		currDir = findDirectory(token);
		token = strtok (NULL, "/");
	}
	return true;
}

void mkdir(char *input)
{
	struct Directory *initialDir = currDir;
	char name [20];
	char dirPath [strlen(input)];
	char dirPathCopy [strlen(input)];
	bool found;
	strcpy(dirPath, input);
	strcpy(dirPathCopy, dirPath);

	char *dirName = strtok (input, "/");
	if (!strtok (NULL, "/"))
		goto dirFound;

	strcpy(name, mystrrev(strtok(mystrrev(dirPathCopy), "/")));
	dirPath[strlen(dirPath) - strlen(name)] = '\0';

	found = cd(dirPath);
	if (!found)
		return;
	strcpy(dirName, name);
	
	dirFound:
	bool check = SpecialCharacter(dirName);
	
		if (folderExists(dirName))
		{
			printf("A subdirectory %s already exists.\n", dirName);
			return;
		}

		if(check == true)
		{
			printf("Invalid file name.\n");
			return;
		}

		currDir->folder[currDir->folderCount] = (struct Directory *) malloc (sizeof(struct Directory));
		currDir->folder[currDir->folderCount]->parent = currDir;
		strcpy(currDir->folderNames[currDir->folderCount], dirName);

		currDir->folder[currDir->folderCount]->folderCount = 0;
		currDir->folder[currDir->folderCount]->fileCount = 0;

		for (int i = 0; i < MAX; i++)
		{
			currDir->folder[currDir->folderCount]->folder[i] = NULL;
			currDir->folder[currDir->folderCount]->fileContent[i] = NULL;
			currDir->folder[currDir->folderCount]->fileContent[i] = NULL;
		}
		currDir->folderCount++;

		currDir = initialDir;
}

void rmdir (char *input)
{
	struct Directory *initialDir = currDir;
	char name [20];
	char dirPath [strlen(input)];
	char dirPathCopy [strlen(input)];
	bool found;
	strcpy(dirPath, input);
	strcpy(dirPathCopy, dirPath);

	char *dirName = strtok (input, "/");
	if (!strtok (NULL, "/"))
		goto dirFound;

	strcpy(name, mystrrev(strtok(mystrrev(dirPathCopy), "/")));
	dirPath[strlen(dirPath) - strlen(name)] = '\0';

	found = cd(dirPath);
	if (!found)
		return;
	strcpy(dirName, name);
	
	dirFound:
		if (folderExists(dirName) == false)
		{
			printf("The system cannot find the path specified.\n");
			return;
		}

		struct Directory *dirToDel = findDirectory(dirName);

		if (dirToDel == initialDir)
		{
			printf("Unable to remove directory. Access denied.\n");
			return;
		}

		if (dirToDel->folderCount > 0)
		{
			printf("The directory '%s' is not empty.\n", dirName);
			return;
		}

		struct Directory *parent = dirToDel->parent;
		int dirToDelIndex = findDirectoryIndex(dirToDel);

		parent->folder[dirToDelIndex] = parent->folder[parent->folderCount - 1];
		parent->folder[parent->folderCount - 1] = NULL;
		free(dirToDel);

		strcpy(parent->folderNames[dirToDelIndex], parent->folderNames[parent->folderCount - 1]);
		parent->folderCount--;
	
		currDir = initialDir;
}

void ls (bool validPath)
{
	if (!validPath)
		return;

	if (currDir->folderCount == 0 && currDir->fileCount == 0)
	{
		printf("No content available to display.\n");
		return;
	}

	printf("\nType	Name\n");
	for (int i = 0; i < currDir->folderCount; i++)
		printf("<DIR>   %s\n", currDir->folderNames[i]);
	for (int i = 0; i < currDir->fileCount; i++)
		printf("<FILE>   %s\n", currDir->fileNames[i]);
}

void deleteFile(char* name) {
	char dirName[20];
	scanf("%s", dirName);
	
	if(!cd(dirName))
		return;

	bool fileFound = false;
	for (int i = 0; i < MAX; i++) {
		if (currDir->fileNames[i][0] != '\0' && strcmp(currDir->fileNames[i], name) == 0) {
			free(currDir->fileContent[i]);
			currDir->fileContent[i] = NULL;
			strcpy(currDir->fileNames[i], "");
			printf("File '%s' removed.\n", name);
			fileFound = true;
			break;
		}
	}

	if (!fileFound) {
		printf("File '%s' not found.\n", name);
	}
}

void touch(char* input) {
	struct Directory *initialDir = currDir;
    char name[50];
    char content[100];
    
	sscanf(input, "%s", name);

	char dirPath [strlen(input)];
	char dirPathCopy [strlen(input)];
	bool found,check;
	strcpy(dirPath, input);
	strcpy(dirPathCopy, dirPath);

	char *dirName = strtok (input, "/");
	if (!strtok (NULL, "/"))
		goto dirFound;

	strcpy(name, mystrrev(strtok(mystrrev(dirPathCopy), "/")));
	dirPath[strlen(dirPath) - strlen(name)] = '\0';
	
	if (!cd(dirPath))
		return;

	dirFound:

	check = SpecialCharacter(name);

    if (fileExists(name)) {
        printf("A directory with name '%s' already exists. Cannot create file.\n", name);
        return;
    }

	if(check == true)
	{
		printf("Invalid file name.\n");
		return;
	}

	printf("file content: ");
	scanf(" %[^\n]", content);

    currDir->fileContent[currDir->fileCount] = (char *)malloc(sizeof(char)*100);
    strcpy(currDir->fileContent[currDir->fileCount], content);
    strcpy(currDir->fileNames[currDir->fileCount], name);
    currDir->fileCount++;

	currDir = initialDir;
}

void retouch(char *input) {
    char name[20];
    sscanf(input, "%s", name);

    struct Directory *initialDir = currDir;
    char dirPath[strlen(input)];
    char dirPathCopy[strlen(input)];
    bool found;
    strcpy(dirPath, input);
    strcpy(dirPathCopy, dirPath);

    char *fileName = strtok(input, "/");
    if (!strtok(NULL, "/")) {
        goto dirFound;
    }

    strcpy(name, mystrrev(strtok(mystrrev(dirPathCopy), "/")));
    dirPath[strlen(dirPath) - strlen(name)] = '\0';

    if (!cd(dirPath)) {
        return;
    }

dirFound:
    if (fileExists(name) == false) {
        printf("The '%s' file does not exist.\n", name);
        return;
    }

    int fileIndex = findFileIndex(name);
	printf("Enter file content: ");
    scanf(" %[^\n]", currDir->fileContent[fileIndex]);
    currDir = initialDir;
}

void cat(char* input) {

	struct Directory *initialDir = currDir;
	char name [20];
	char dirPath [strlen(input)];
	char dirPathCopy [strlen(input)];
	bool found;
	strcpy(dirPath, input);
	strcpy(dirPathCopy, dirPath);

	char *fileName = strtok (input, "/");
	if (!strtok (NULL, "/"))
		goto dirFound;

	strcpy(name, mystrrev(strtok(mystrrev(dirPathCopy), "/")));
	dirPath[strlen(dirPath) - strlen(name)] = '\0';

	found = cd(dirPath);
	if (!found)
		return;
	strcpy(fileName, name);
	
	dirFound:
		if (fileExists(fileName) == false)
		{
			printf("The system cannot find the path specified.\n");
			return;
		}

		int fileNameIndex = findFileIndex(fileName);
		printf("%s\n", currDir->fileContent[fileNameIndex]);
	
		currDir = initialDir;
}

void rm(char* input) {
    char name[20];
    sscanf(input, "%s", name);

	struct Directory *initialDir = currDir;
	char dirPath [strlen(input)];
	char dirPathCopy [strlen(input)];
	bool found,check;
	strcpy(dirPath, input);
	strcpy(dirPathCopy, dirPath);

	char *dirName = strtok (input, "/");
	if (!strtok (NULL, "/"))
		goto dirFound;

	strcpy(name, mystrrev(strtok(mystrrev(dirPathCopy), "/")));
	dirPath[strlen(dirPath) - strlen(name)] = '\0';
	
	if (!cd(dirPath))
		return;

	dirFound:
    if (!fileExists(name))
	{
		printf("The '%s' file does not exist.\n", name);
		return;
	}
	
	int fileToDelIndex = findFileIndex(name);
	strcpy(currDir->fileContent[fileToDelIndex], currDir->fileContent[currDir->fileCount - 1]);
	strcpy(currDir->fileNames[fileToDelIndex], currDir->fileNames[currDir->fileCount - 1]);
	currDir->fileCount--;

	currDir = initialDir;
}

int main ()
{
	root = (struct Directory *) malloc (sizeof(struct Directory));
	root->folderCount = 0;
	root->fileCount = 0;
	
	for (int i = 0; i < MAX; i++)
	{
		root->folder[i] = NULL;
		root->fileContent[i] = NULL;
	}

	root->parent = NULL;
	currDir = root;

	char command[100];
	while (true)
	{
		printPrompt();
		scanf("%s", command);
		
		if (strcmp(mystrlower(command), "exit") == 0)
		{
			break;
		}
		
		else if (strcmp(mystrlower(command), "mkdir") == 0)
		{
			char input[100];
			scanf(" %[^\n]", input);
			
			if (strlen(input) == 0)
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			char folderNames[MAX][20];	
			int i = 0;

			char *token = strtok(input, " ");
			while (token)
			{
				strcpy(folderNames[i++], token);
				token = strtok(NULL, " ");
			}

			if (i == 0)
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			else if (i == 1)
			{
				mkdir(folderNames[0]);
				continue;
			}

			else
			{
				bool invalidNames = false;
				for (int j = 0; j < i; j++)
				{
					token = strtok(folderNames[j], "/");
					if (strtok(NULL, "/"))
					{
						invalidNames = true;
						break;
					}
				}

				if (invalidNames)
				{
					printf("The syntax of the command is incorrect.\n");
					continue;
				}

				for (int j = 0; j < i; j++)
					mkdir(folderNames[j]);
			}
		}

		else if (strcmp(mystrlower(command), "rmdir") == 0)
		{
			char input[100];
			scanf(" %[^\n]", input);
			
			if (strlen(input) == 0)
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			char fileNames[MAX][20];
			int i = 0;

			char *token = strtok(input, " ");
			while (token)
			{
				strcpy(fileNames[i++], token);
				token = strtok(NULL, " ");
			}

			if (i == 0)
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			else if (i == 1)
			{
				rmdir(fileNames[0]);
				continue;
			}

			else
			{
				bool invalidNames = false;
				for (int j = 0; j < i; j++)
				{
					token = strtok(fileNames[j], "/");
					if (strtok(NULL, "/"))
					{
						invalidNames = true;
						break;
					}
				}

				if (invalidNames)
				{
					printf("The syntax of the command is incorrect.\n");
					continue;
				}

				for (int j = 0; j < i; j++)
					rmdir(fileNames[j]);
			}
		}
		
		else if (strcmp(mystrlower(command), "cd") == 0)
		{
			char input[100];
			scanf(" %[^\n]", input);
			if (input[0] == '\n')
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			char dirNames[MAX][20];
			int i = 0;

			char *token = strtok(input, " ");
			while (token)
			{
				strcpy(dirNames[i++], token);
				token = strtok(NULL, " ");
			}

			if (i == 0 || i > 1)
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			else
			{
				cd(dirNames[0]);
				continue;
			}
		}
		
		else if (strcmp(mystrlower(command), "ls") == 0)
		{
			struct Directory *initialDir = currDir;
			char input[100];
			scanf(" %[^\n]", input);
			if (input[0] == '\n')
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			char dirNames[MAX][20];
			int i = 0;

			char *token = strtok(input, " ");
			while (token)
			{
				strcpy(dirNames[i++], token);
				token = strtok(NULL, " ");
			}

			if (i == 0 || i > 1)
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			else
			{
				bool validPath = cd(dirNames[0]);
				ls(validPath);

				currDir = initialDir;
				continue;
			}
		}

		else if (strcmp(mystrlower(command), "touch") == 0)
		{
			char input[100];
			scanf(" %[^\n]", input);
			
			if (strlen(input) == 0)
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			char fileNames[MAX][20];	
			int i = 0;

			char *token = strtok(input, " ");
			while (token)
			{
				strcpy(fileNames[i++], token);
				token = strtok(NULL, " ");
			}

			if (i == 0)
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			else if (i == 1)
			{
				touch(fileNames[0]);
				continue;
			}

			else
			{
				bool invalidNames = false;
				for (int j = 0; j < i; j++)
				{
					token = strtok(fileNames[j], "/");
					if (strtok(NULL, "/"))
					{
						invalidNames = true;
						break;
					}
				}

				if (invalidNames)
				{
					printf("The syntax of the command is incorrect.\n");
					continue;
				}

				for (int j = 0; j < i; j++)
					touch(fileNames[j]);
			}
		}

		else if (strcmp(mystrlower(command), "rm") == 0)
		{
			char input[100];
			scanf(" %[^\n]", input);
			
			if (strlen(input) == 0)
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			char fileNames[MAX][20];
			int i = 0;

			char *token = strtok(input, " ");
			while (token)
			{
				strcpy(fileNames[i++], token);
				token = strtok(NULL, " ");
			}

			if (i == 0)
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			else if (i == 1)
			{
				rm(fileNames[0]);
				continue;
			}

			else
			{
				bool invalidNames = false;
				for (int j = 0; j < i; j++)
				{
					token = strtok(fileNames[j], "/");
					if (strtok(NULL, "/"))
					{
						invalidNames = true;
						break;
					}
				}

				if (invalidNames)
				{
					printf("The syntax of the command is incorrect.\n");
					continue;
				}

				for (int j = 0; j < i; j++)
					rm(fileNames[j]);
			}
		}

		else if (strcmp(mystrlower(command), "cat") == 0)
		{
			char input[100];
			scanf(" %[^\n]", input);
			
			if (strlen(input) == 0)
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			char dirNames[MAX][20];
			int i = 0;

			char *token = strtok(input, " ");
			while (token)
			{
				strcpy(dirNames[i++], token);
				token = strtok(NULL, " ");
			}

			if (i == 0 || i > 1)
			{
				printf("The syntax of the command is incorrect.\n");
				continue;
			}

			else
			{
				cat(dirNames[0]);
				continue;
			}
		}
		
		else if (strcmp(mystrlower(command), "retouch") == 0)
		{
			struct Directory *initialDir = currDir;
			char input[100];
			scanf(" %[^\n]", input);
			retouch(input);
			currDir = initialDir;
		}

		else if (strcmp(mystrlower(command), "help") == 0)
		{
			help();
		}
		
		else
		{
			printf("'%s' is not recognized as a valid command.\n", command);
		}
	}

	return 0;
}