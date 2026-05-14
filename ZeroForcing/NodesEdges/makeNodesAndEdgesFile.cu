// To compile the simulation type the line below in the terminal.
// nvcc makeNodesAndEdgesFile.cu -o temp
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#define PI 3.14

double Volume, Radius;
double Width, Height, Depth;
double WidthLayers, HeightLayers, DepthLayers;
int NumberOfNodes;
int NumberOfMuscles;
float3* Node;
int2* Muscle;
int PulsePointNode;
int UpNode;
int FrontNode;

// Prototypes 
void setup();
double croppedRandomNumber(double, double, double);
void setNodesAndMusclesUniformSheet();
void setNodesAndMusclesCrossSheet();
void saveNodesAndMuscle();

void setup()
{
	// A normal left atrium holds around 23,640 cubic millimeters of blood.
	Volume = 23640.0;
	Radius = pow((3.0*Volume)/(4.0*PI), 1.0/3.0);
	
	Width = Radius;
	Height = Radius;
	Depth = Radius/10.0;
	
	// How many cuts in each direction;
	WidthLayers = 21;
	HeightLayers = 21;
	DepthLayers = 1;
}

/*
 This function: 
 1: Uses the Box-Muller method to create a standard normal random number from two uniform random numbers.
 2: Sets the standard deviation to what was input.
 3: Checks to see if the random number is between the desired numbers. If not throw it away and choose again.
*/
double croppedRandomNumber(double stddev, double left, double right)
{
	double temp1, temp2;
	double randomNumber;
	bool test = false;
			
	while(test == false)
	{
		// Getting two uniform random numbers in [0,1]
		temp1 = ((double) rand() / (RAND_MAX));
		temp2 = ((double) rand() / (RAND_MAX));
		
		// Using Box-Muller to get a standard normally distributed random number (mean = 0, stddev = 1)
		randomNumber = sqrt(-2.0 * log(temp1))*cos(2.0*PI*temp2);
		
		// Setting its Standard Deviation to the the desired value. 
		randomNumber *= stddev;
		
		// Chopping the random number between left and right.  
		if(randomNumber < left || right < randomNumber) test = false;
		else test = true;
	}
	return(randomNumber);	
}

void setNodesAndMusclesUniformSheet() 
{
	float dx, dy, dz;
	
	// This will be one more than the cuts because you will need a termination of the last cut.
	int nodesX = WidthLayers + 1;
	int nodesY = HeightLayers + 1;
	int nodesZ = DepthLayers + 1;
	
	NumberOfNodes = nodesX*nodesY*nodesZ;
	NumberOfMuscles = ((nodesX - 1)*nodesY + nodesX*(nodesY - 1))*nodesZ + nodesX*nodesY*(nodesZ - 1);
	
	// This is putting the pulse node in the middle of one side of the sheet. 
	PulsePointNode = nodesX/2 + ((nodesZ -1)/2)*nodesX*nodesY;
	UpNode = 0;
	FrontNode = 0;
	
	dx =  Width/(float)(nodesX - 1);
	if(1 < nodesY) dy = Height/(float)(nodesY - 1);
	else dy = 0.0;
	if(1 < nodesZ) dz = Depth/(float)(nodesZ - 1);
	else dz = 0.0;
	
	double stddev = 0.5;
	
	Node = (float3*)malloc(NumberOfNodes*sizeof(float3));
	Muscle = (int2*)malloc(NumberOfMuscles*sizeof(int2));
	
	// Setting the positions on a line.
	float startX = -Width/2.0;
	float startY = -Height/2.0;
	float startZ = -Depth/2.0;
	int k = 0;
	for(int l = 0; l < nodesZ; l++)
	{
		for(int j = 0; j < nodesY; j++)
		{
			for(int i = 0; i < nodesX; i++)
			{
				Node[k].x = startX + i*dx + croppedRandomNumber(stddev, -dx/2.2, dx/2.2);
				if(1 < nodesY) Node[k].y = startY + j*dy + croppedRandomNumber(stddev, -dy/2.2, dy/2.2);
				else Node[k].y = 0.0;
				if(1 < nodesZ) Node[k].z = startZ + l*dz + croppedRandomNumber(stddev, -dz/2.2, dz/2.2);
				else Node[k].z = 0.0;
				k++;
			}
		}
	}
	
	// Setting the Muscles links to -1 so you can see if they are not used.
	for(int i = 0; i < NumberOfMuscles; i++)
	{
		Muscle[i].x =  -1;
		Muscle[i].y =  -1;	
	}
	
	// Setting the Muscles.
	int index;
	k = 0;
	for(int l = 0; l < nodesZ; l++)
	{
		for(int j = 0; j < nodesY; j++)
		{
			for(int i = 0; i < nodesX; i++)
			{
				index = i + j*nodesX + l*nodesX*nodesY;
				
				if(i < (nodesX - 1))
				{
					Muscle[k].x =  index;
					Muscle[k].y =  index + 1;
					k++;
				}
				if(j < (nodesY - 1))
				{
					Muscle[k].x =  index;
					Muscle[k].y =  index + nodesX;
					k++;
				}
				if(l < (nodesZ - 1))
				{
					Muscle[k].x =  index;
					Muscle[k].y =  index + nodesX*nodesY;
					k++;
				}
			}
		}
	}
}

void setNodesAndMusclesCrossSheet() 
{
	float dx, dy, dz;
	
	// This will be one more than the cuts because you will need a termination of the last cut.
	int nodesX = WidthLayers + 1;
	int nodesY = HeightLayers + 1;
	int nodesZ = 1;
	
	NumberOfNodes = nodesX*nodesY*nodesZ;
	NumberOfMuscles = ((nodesX - 1)*nodesY + nodesX*(nodesY - 1) + (nodesX - 1)*(nodesY - 1)*2);
	
	// This is putting the pulse node in the middle of one side of the sheet. 
	PulsePointNode = nodesX/2 + ((nodesZ -1)/2)*nodesX*nodesY;
	UpNode = 0;
	FrontNode = 0;
	
	dx =  Width/(float)(nodesX - 1);
	if(1 < nodesY) dy = Height/(float)(nodesY - 1);
	else dy = 0.0;
	if(1 < nodesZ) dz = Depth/(float)(nodesZ - 1);
	else dz = 0.0;
	
	double stddev = 0.0;
	
	Node = (float3*)malloc(NumberOfNodes*sizeof(float3));
	Muscle = (int2*)malloc(NumberOfMuscles*sizeof(int2));
	
	// Setting the positions on a line.
	float startX = -Width/2.0;
	float startY = -Height/2.0;
	float startZ = -Depth/2.0;
	int k = 0;
	for(int l = 0; l < nodesZ; l++)
	{
		for(int j = 0; j < nodesY; j++)
		{
			for(int i = 0; i < nodesX; i++)
			{
				Node[k].x = startX + i*dx + croppedRandomNumber(stddev, -dx/2.2, dx/2.2);
				if(1 < nodesY) Node[k].y = startY + j*dy + croppedRandomNumber(stddev, -dy/2.2, dy/2.2);
				else Node[k].y = 0.0;
				if(1 < nodesZ) Node[k].z = startZ + l*dz + croppedRandomNumber(stddev, -dz/2.2, dz/2.2);
				else Node[k].z = 0.0;
				k++;
			}
		}
	}
	
	// Setting the Muscles links to -1 so you can see if they are not used.
	for(int i = 0; i < NumberOfMuscles; i++)
	{
		Muscle[i].x =  -1;
		Muscle[i].y =  -1;	
	}
	
	// Setting the Muscles.
	int index;
	k = 0;
	for(int j = 0; j < nodesY; j++)
	{
		for(int i = 0; i < nodesX; i++)
		{
			index = i + j*nodesX;
			
			if(i < (nodesX - 1))
			{
				Muscle[k].x =  index;
				Muscle[k].y =  index + 1;
				k++;
				if(j < (nodesY - 1))
				{
					Muscle[k].x =  index;
					Muscle[k].y =  index + nodesX;
					k++;
					Muscle[k].x =  index;
					Muscle[k].y =  index + nodesX + 1;
					k++;
				}
				if(0 < j)
				{
					Muscle[k].x =  index;
					Muscle[k].y =  index - nodesX + 1;
					k++;
				}
				
				
			}
			if(i == (nodesX - 1) && j < (nodesY - 1))
			{
				Muscle[k].x =  index;
				Muscle[k].y =  index + nodesX;
				k++;
			}
		}
	}
}

void saveNodesAndMuscle()
{
	const char *folderName = "Name It";
	
	// Creating the diretory to hold the run settings.
	if(mkdir(folderName, 0777) != 0)
	{
		printf("\n Error creating Nodes and Muscles folder '%s'.\n", folderName);
	}
	
	// Moving into the folder
	chdir(folderName);
	
	// Copying all the Nodes into this folder in the file named Nodes.
	FILE *NodesFile;
  	NodesFile = fopen("Nodes", "wb");
  	if (NodesFile == NULL) 
  	{
		printf("Error opening NodesFile!\n");
		exit(0);
    	}
    	fprintf(NodesFile, "%d\n", NumberOfNodes);
  	fprintf(NodesFile, "%d\n", PulsePointNode);
  	fprintf(NodesFile, "%d\n", UpNode);
  	fprintf(NodesFile, "%d\n", FrontNode);
  	
  	for(int i = 0; i < NumberOfNodes; i++)
	{
		fprintf(NodesFile,"%d %f %f %f\n", i, Node[i].x, Node[i].y, Node[i].z);
	}
  	fclose(NodesFile);
  	
  	// Copying all the muscules into this folder in the file named Muscles.
	FILE *MusclesFile;
  	MusclesFile = fopen("Edges", "wb");
  	if (MusclesFile == NULL) 
  	{
		printf("Error opening MusclesFile!\n");
		exit(0);
    	}
	fprintf(MusclesFile, "%d\n", NumberOfMuscles);
  	for(int i = 0; i < NumberOfMuscles; i++)
	{
		fprintf(MusclesFile,"%d %d %d\n", i, Muscle[i].x, Muscle[i].y);
	}
  	fclose(MusclesFile);
  	
  	// Creating BachmannsBundle folder
  	/*
	FILE *BachmannsBundleFile;
  	BachmannsBundleFile = fopen("BachmannsBundle", "wb");
  	if (BachmannsBundleFile == NULL) 
  	{
		printf("Error opening BachmannsBundleFile!\n");
		exit(0);
    	}
	fprintf(BachmannsBundleFile, "%d\n", 0);
  	fclose(BachmannsBundleFile);
  	*/
  	
	// Moving back to original directory.
	//chdir("../");

	printf("\n Nodes and Muscles have been created and saved.\n");
}

int main(int argc, char** argv)
{
	int type = 2;
	setup();
	if(type == 1) setNodesAndMusclesUniformSheet();
	if(type == 2) setNodesAndMusclesCrossSheet();
	saveNodesAndMuscle();
	return 0;
}
