// void readSimulationParameters();
// void ReadInAndSetNodes();
// void ReadInAndSetEdges();
// void linkNodes();

void readSimulationParameters()
{
	ifstream data;
	string name;
	
	data.open("./simulationSetup");
	
	if(data.is_open() == 1)
	{
		getline(data,name,'=');
		data >> NodesEdgesFileName;
	}
	else
	{
		printf("\nTSU Error could not open simulationSetup file\n");
		exit(0);
	}
	data.close();
	
	printf("\n Simulation Parameters have been read in.");
}

void ReadInAndSetNodes()
{	
	FILE *inFile;

	float x, y, z;
	float d, dx, dy, dz;
	int id;
	char fileName[256];
	char directory[] = "./NodesEdges/";
	strcpy(fileName, "");
	strcat(fileName, directory);
	strcat(fileName, NodesEdgesFileName);
	strcat(fileName, "/Nodes");
	//printf("\n fileName = %s\n", fileName);

	inFile = fopen(fileName,"rb");
	if(inFile == NULL)
	{
		printf("\n Can't open Nodes file.\n");
		exit(0);
	}
	
	fscanf(inFile, "%d", &NumberOfNodes);
	printf("\n NumberOfNodes = %d", NumberOfNodes);
	
	// Allocating memory for the nodes and connections.
	Node = (nodeAtributesStructure*)malloc(NumberOfNodes*sizeof(nodeAtributesStructure));
	
	for(int i = 0; i < NumberOfNodes; i++)
	{
		fscanf(inFile, "%d %f %f %f", &id, &x, &y, &z);
		
		Node[id].position.x = x;
		Node[id].position.y = y;
		Node[id].position.z = z;
		
		Node[i].color.x = 1.0;
		Node[i].color.y = 1.0;
		Node[i].color.z = 1.0;
	}

	fclose(inFile);
    
	// Setting the nodes to -1 so you can tell the nodes that where not used.
	for(int i = 0; i < NumberOfNodes; i++)
	{
		for(int j = 0; j < LinksPerNode; j++)
		{
			Node[i].connection[j] = -1;
		}	
	}
	
	float3 centerOfObject;
	centerOfObject.x = 0.0;
	centerOfObject.y = 0.0;
	centerOfObject.z = 0.0;
	for(int i = 0; i < NumberOfNodes; i++)
	{
		 centerOfObject.x += Node[i].position.x;
		 centerOfObject.y += Node[i].position.y;
		 centerOfObject.z += Node[i].position.z;
	}
	centerOfObject.x /= NumberOfNodes;
	centerOfObject.y /= NumberOfNodes;
	centerOfObject.z /= NumberOfNodes;
	
	printf("\n center (%f, %f, %f).", centerOfObject.x, centerOfObject.y, centerOfObject.z);
	
	for(int i = 0; i < NumberOfNodes; i++)
	{
		Node[i].position.x -= centerOfObject.x;
		Node[i].position.y -= centerOfObject.y;
		Node[i].position.z -= centerOfObject.z;
	}
	
	float radiusOfObject = 0.0;
	for(int i = 0; i < NumberOfNodes; i++)
	{
	        dx = Node[i].position.x*Node[i].position.x;
	        dy = Node[i].position.y*Node[i].position.y;
	        dz = Node[i].position.z*Node[i].position.z;
	        d = sqrt(dx + dy + dz);
		if(radiusOfObject < d) radiusOfObject = d;
	}
	
	for(int i = 0; i < NumberOfNodes; i++)
	{
	        Node[i].position.x /= radiusOfObject;
		Node[i].position.y /= radiusOfObject;
		Node[i].position.z /= radiusOfObject;
	}
	//RadiusOfObject = 1.0;
	
	CenterOfSimulation.x = centerOfObject.x;
	CenterOfSimulation.y = centerOfObject.y;
	CenterOfSimulation.z = centerOfObject.z;
	
	printf("\n Nodes have been read in and set.");
}

void ReadInAndSetEdges()
{	
	FILE *inFile;

	//float x, y, z;
	int id, idNode1, idNode2;
	char fileName[256];
	char directory[] = "./NodesEdges/";
	strcpy(fileName, "");
	strcat(fileName, directory);
	strcat(fileName, NodesEdgesFileName);
	strcat(fileName, "/Nodes");
	//printf("\n fileName = %s\n", fileName);
	
	strcpy(fileName, "");
	strcat(fileName, directory);
	strcat(fileName, NodesEdgesFileName);
	strcat(fileName, "/Edges");
	//printf("\n fileName = %s\n",  fileName);
	
	inFile = fopen(fileName,"rb");
	if (inFile == NULL)
	{
		printf("\n Can't open Edges file.\n");
		exit(0);
	}
	
	fscanf(inFile, "%d", &NumberOfEdges);
	printf("\n NumberOfEdges = %d", NumberOfEdges);
	
	// Allocating memory for the nodes and connections.
	Edge = (edgeAtributesStructure*)malloc(NumberOfEdges*sizeof(edgeAtributesStructure));
	
	for(int i = 0; i < NumberOfEdges; i++)
	{
		fscanf(inFile, "%d", &id);
		fscanf(inFile, "%d", &idNode1);
		fscanf(inFile, "%d", &idNode2);
		//printf("\n%d %d %d\n", id, idNode1, idNode2);
		Edge[id].nodeA = idNode1;
		Edge[id].nodeB = idNode2;
	}
	
	fclose(inFile);
	
	printf("\n Edges have been read in and set.");
}

// This code tells a node all the nodes it is connected to.
void linkNodes()
{	
        int id;
        int check;
	for(int i = 0; i < NumberOfEdges; i++)
	{
	        id = Edge[i].nodeA;
	        check = 0;
		for(int j = 0; j < LinksPerNode; j++)
		{
		      if(Node[id].connection[j] == -1) 
		      {
		          Node[id].connection[j] = Edge[i].nodeB;
		          check = 1;
		          break;
		      }
		}
		if(check == 0)
		{
		      printf("\n Nodes %d have too many links. Max links per node = %d", id, LinksPerNode);
		      printf("\n Check this out! Good Bye");
		      exit(0);
		}
		
		id = Edge[i].nodeB;
		check = 0;
		for(int j = 0; j < LinksPerNode; j++)
		{
		      if(Node[id].connection[j] == -1) 
		      {
		          Node[id].connection[j] = Edge[i].nodeA;
		          check = 1;
		          break;
		      }
		}
		if(check == 0)
		{
		      printf("\n Nodes %d have too many links. Max links per node = %d", id, LinksPerNode);
		      printf("\n Check this out! Good Bye");
		      exit(0);
		}
	}
      
	printf("\n Nodes have been linked");
}
