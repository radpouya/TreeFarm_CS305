// Pouya Rad
// James Schmidt
// CS 305 A
// Programming Assignment #5: BST
// 11/16/16
//
// This program creates a farm of different tree types using the Binary Search
// Tree data structure. This program should be run with 3 simple command line
// arguments: the run command for the program, the input file in the form of a
// .txt file, and the output file in the form of a .txt file. 
// Note: I know that it is not wise to include implementation details in the
// comments for methods but due to the name of the assignment, I felt that
// there wasn't a better way to do so without causing an immense amount of
// confusion. 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tree Farm node structure
struct treeFarm {
	int numTrees;
	int lotNum;
	int treeAge;
	char* sold;
	char* type;
	struct treeFarm* right;
	struct treeFarm* left;
	struct treeFarm* parent;
};

// Method calls
struct treeFarm* batchState(struct treeFarm*, char*);
struct treeFarm* batchHelp(char, struct treeFarm*, char*);
struct treeFarm* insert(struct treeFarm*, struct treeFarm*);
struct treeFarm* makeParents(struct treeFarm*);
struct treeFarm* delete(struct treeFarm*, int);
struct treeFarm* buy(struct treeFarm*, int);
struct treeFarm* search(struct treeFarm*, int);
struct treeFarm* findMin(struct treeFarm*);
struct treeFarm* findMax(struct treeFarm*);
int sum(struct treeFarm*, int);
void writeOutput(struct treeFarm*, FILE*, int);
void printMinAndMax(struct treeFarm*, FILE*);

// Main method to run the program. 
int main(int argc, char* args[]) {
	struct treeFarm* mainFarm = NULL;
	
	if(argc == 3) {
		mainFarm = batchState(mainFarm, args[1]);
	}
	
	FILE* treeOutput = fopen(args[2], "w");
	fprintf(treeOutput, "NumTree,Lot,Age,Type,Sold,TreeLevel\n");
	writeOutput(mainFarm, treeOutput, 0);
	printMinAndMax(mainFarm, treeOutput);
	fclose(treeOutput);
	
	return 0;
}

// This is the primary state that the program is in for most of the time.
// It accepts a treeFarm pointer that represents the top of the BST (the root)
// and string for the input path. 
// This method uses an input file to create a BST of Tree Farms and returns
// the completed tree. 
struct treeFarm* batchState(struct treeFarm* farm, char* inputPath) {
	FILE* input;
	input = fopen(inputPath, "r");
	char buff[1000];
	char* line = fgets(buff, 999, input);
	char currCommand;

	while(line != NULL) {
		if(line[0] == '=') {// assumes format of commands is maintained
			if(line[2] == 'i') {		
				currCommand = 'i';
				char buff2[1000]; // takes in the second line and knows that its just the order for values
				char* line2 = fgets(buff2, 999, input);
				char buff3[1000];
				char* line3 = fgets(buff3, 999, input);
				farm = batchHelp(currCommand, farm, line3);
			} else if(line[2] == 'd') {
				char buff2[1000]; // takes in the second line and knows that its just the order for values
				char* line2 = fgets(buff2, 999, input);
				char buff3[1000];
				char* line3 = fgets(buff3, 999, input);
				currCommand = 'd';
				farm = batchHelp(currCommand, farm, line3);
			} else if(line[2] == 'b') {
				char buff2[1000];
				char* line2 = fgets(buff2, 999, input);
				currCommand = 'b';
				farm = batchHelp(currCommand, farm, line2);
			} 
		} else {
			if(line[1] != '\0') {
				farm = batchHelp(currCommand, farm, line);
			}
		} 
		line = fgets(buff, 999, input);
	} 
	
	fclose(input);
	return farm;
}

// This is a helper method to the primary batch method.
// It accepts a char to let the program know which part of the input file it is
// on and what it should be doing. Additionally, the entire tree is accepted
// for the program to modify it as the input file dictates. Lastly, the current
// line of the input file that the program is on is accepted as a parameter to 
// be parsed into the relevant data. Additionally, when inserting, this is the 
// part of the program that creates each individual tree farm. 
// It returns the main tree farm when it is finished. 
struct treeFarm* batchHelp(char currCommand, struct treeFarm* farm, char* currLine) {
	if(currCommand == 'i') { // insert	
		struct treeFarm* tempFarm = malloc(sizeof(struct treeFarm));
		const char delim[2] = ",";
		char* token;
		char* ptr;
		char *newline = strchr(currLine, '\n');
		if(newline) {
  			*newline = 0;
  		}	
		token = strtok(currLine, delim);
		tempFarm->numTrees = atoi(token);

		token = strtok(NULL, delim);
		tempFarm->lotNum = atoi(token);

		token = strtok(NULL, delim);
		tempFarm->treeAge = atoi(token); 

		token = strtok(NULL, delim);
		tempFarm->type = malloc(strlen(token));

		strcpy(tempFarm->type, token);

		tempFarm->sold = "no";
		tempFarm->left = NULL;
		tempFarm->right = NULL; 
		tempFarm->parent = NULL;
		
		farm = insert(farm, tempFarm);
		farm = makeParents(farm);
	} else if(currCommand == 'd') { // delete
		int deleteNumTree = atoi(currLine);
		farm = delete(farm, deleteNumTree);
	} else if(currCommand == 'b') { // buy
		int buyNumTree = atoi(currLine);
		farm = buy(farm, buyNumTree);
	} else {
		return farm;
	}
	return farm;
}

// This method inserts a new Tree Farm into the existing BST of Tree Farms.
// It accepts a treeFarm* as the root for the overall BST and then a temp
// which represents the newly created Tree Farm to be inserted. 
// The method returns the overall farm. 
struct treeFarm* insert(struct treeFarm* farm, struct treeFarm* temp) {
	if(farm == NULL) {
		return temp;
	} else {
		if(temp->numTrees < farm->numTrees) {
			farm->left = insert(farm->left, temp);
		} else {
			farm->right = insert(farm->right, temp);
		}
	} 
	return farm;
}

// This method is a helper method to the insert function. After all trees have
// been inserted, this method goes through the entire BST of Tree Farms and 
// assigns each Tree Farm's parent pointer to its rightful parent node. 
// It accepts a treeFarm* for the overall root of the BST and returns the farm
// when it is finished updating the parent pointers. 
struct treeFarm* makeParents(struct treeFarm* farm) {
	if(farm->left != NULL) {
		farm->left->parent = farm;
		makeParents(farm->left);
	}
	if(farm->right != NULL) {
		farm->right->parent = farm;
		makeParents(farm->right);
	}
	return farm;
}

// This method is used to delete a Tree Farm from the BST. 
// It accepts the whole BST and an integer as parameters. The former is 
// to be able to go through the BST and find the Tree Farm to remove and the
// latter is the Tree Farm that should be deleted. 
// It returns the updated BST of Tree Farms when finished. 
struct treeFarm* delete(struct treeFarm* farm, int d) {
	if(farm == NULL) {
		return farm;
	}
	if(d < farm->numTrees) {
		farm->left = delete(farm->left, d);
	} else if(d > farm->numTrees) {
		farm->right = delete(farm->right, d);
	} else {
		if(farm->left == NULL && farm->right == NULL) {
			free(farm);
			farm = NULL;
		} else if(farm->left == NULL) {
			struct treeFarm* temp = farm;
			farm = farm->right;
			farm->parent = temp->parent;
			free(temp);
		} else if(farm->right == NULL) {
			struct treeFarm* temp = farm;
			farm = farm->left;
			farm->parent = temp->parent;
			free(temp);
		} else {
			struct treeFarm* temp = findMin(farm->right);
			temp->left = farm->left;
			temp = farm;
			farm = farm->right;
			farm->parent = temp->parent;
			free(temp);
		}
	}
	return farm;
}

// This method is called when a Tree Farm is bought. 
// It accepts 2 parameters: the BST of the Tree Farms and the Tree Far that is
// going to be bought. 
// It returns the BST when the Tree Farm that is to be bought has been marked
// as bought. 
struct treeFarm* buy(struct treeFarm* farm, int numTrees) {
	if(farm == NULL) {
		return NULL;
	}
	if(numTrees < farm->numTrees) {
		buy(farm->left, numTrees);
	} else if(numTrees > farm->numTrees) {
		buy(farm->right, numTrees);
	} else {
		farm->sold = "yes";
	}
	return farm;
}

// This method searches the entire BST for a particular Tree Farm.
// It accepts two parameters: the entire BST and the Tree Farm that is being 
// searched for and returns that target farm. 
struct treeFarm* search(struct treeFarm* farm, int target) {
	if(farm == NULL) {
		return NULL;
	}
	if(target < farm->numTrees) {
		search(farm->left, target);
	} else if(target > farm->numTrees) {
		search(farm->right, target);
	} else {
		return farm;
	}
	return farm;
}

// This method finds the smallest Tree Farm. 
// It accepts the BST and returns the smallest Tree Farm when it has been
// found.
struct treeFarm* findMin(struct treeFarm* farm) {
	struct treeFarm* curr = farm;
	while(curr->left != NULL) {
		curr = curr->left;
	}
	return curr;
}

// This method finds the largest Tree Farm. 
// It accepts the BST and returns the largest Tree Farm when it has been
// found.
struct treeFarm* findMax(struct treeFarm* farm) {
	struct treeFarm* curr = farm;
	while(curr->right != NULL) {
		curr = curr->right;
	}
	return curr;
}

// This method goes through all of the Tree Farms and calculates the sum of 
// all of the trees present. It accepts two parameters: the BST that it goes
// through and an integer representing the rolling sum of trees, initially 
// starting at 0. It then returns an integer value for how many trees exist.
int sum(struct treeFarm* farm, int s) {
	if(farm == NULL) {
		return 0;
	} else {
		return s + sum(farm->left, farm->numTrees) + sum(farm->right, farm->numTrees);
	}
}

// This is the main method that prints the contents of the Tree Farms to an
// output file once the input file has been completely run through. It does
// the printing in order from least to greatest. 
// It accepts 3 parameters: the BST that it will traverse, the output file that
// is being written to, and the current level that that Tree Farm is found at. 
void writeOutput(struct treeFarm* farm, FILE* output, int level) {
	if(farm != NULL) {
		level++;
		writeOutput(farm->left, output, level);
		fprintf(output, "%d,%d,%d,%s,%s,%d\n", farm->numTrees, farm->lotNum, farm->treeAge, farm->type, farm->sold, level);
		writeOutput(farm->right, output, level);
		level--;
	}
}

// This method utilizies two other methods to find and print the smallest and 
// largest Tree Farms by printing the type of tree that has the min/max and
// which lot it is from. The two other methods used are findMin() and findMax().
void printMinAndMax(struct treeFarm* farm, FILE* output) {
	fprintf(output, "\n");
	struct treeFarm* min = findMin(farm);
	fprintf(output, "min number of tree is %s lot %d\n", min->type, min->lotNum);
	struct treeFarm* max = findMax(farm);
	fprintf(output, "max number of tree is %s lot %d\n", max->type, max->lotNum);
}