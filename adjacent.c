#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "adjacent.h"
#include <limits.h>

int main(int argc, char **argv) {
	FILE * fptr = NULL;
	FILE * sptr = NULL;
	int vertex = 0; //number of verticies
	int edge = 0; //number of edges
	int queries = 0; //number of queries
	int * search = NULL; //stores the search queries
	Graph * graph = NULL;
	Heap * heap = NULL;
	int i = 0; 	
	int j = 0; 

	//Error Check input argurments 
	if(argv[1] == NULL) {
		printf("\nNot enough input arguments.");
		return -1;
	}
	fptr = fopen(argv[1], "r");
	if(argv[2] == NULL) {
		printf("\nNot enough input arguments.");
		return -1;	
	}
	sptr = fopen(argv[2], "r");
	if(fptr == NULL || sptr == NULL) {
		printf("\nNot enough space for file pointer.");
		return -1;
	}
	
	//read in number of verticies 
	fscanf(fptr, "%d", &vertex);
	fscanf(fptr, "%d", &edge);
	//read in the number of quries
	fscanf(sptr, "%d", &queries);
	//printf("Searches: %d\n", queries);
	//printf("vertex = %d edge = %d", vertex, edge);
	graph = StoreGraph(fptr, vertex, edge);
	if(graph == NULL) {
		printf("\nNo graph allocated");
		return -1;
	}
	//printGraph(graph);
	search = getQuery(queries, sptr);
	
	//Build MinHeap
	heap = CreateHeap(graph->v);
	if(heap == NULL) {
		printf("\nNo heap allocated.");
		return -1;
	}
	for(i = 0; i < (2*queries); i++) {
		//printf("\nSearch=%d", search[i]);
	}
	//Iterate through all of the queries
	for(i = 0, j = 1; i <= (2 * queries); i = i+2, j = j+2) {
		//printf("\nSrc = %d Des = %d", search[i], search[j]);
		if(search[i] > vertex || search[j] > vertex) {
			printf("\nINF");
			printf("\n%d %d", search[i], search[j]);	
		}
		else {
			//printf("\nSrc = %d Des = %d", search[i], search[j]);
			dijkstra(heap, graph, search[i], search[j]);
		}
		//printf("\nHeap Size = %d", heap->CurrentSize);
	}
	//printf("\nHeap Size = %d", heap->CurrentSize);
	//heapTest(heap, graph);
	//Free Memory
	//still need to free graph
	fclose(fptr);
	fclose(sptr);
	free(search);
	free(heap->minheap);
	free(heap);

}

//prints out the adjacency list
void printGraph(Graph * graph) {
	if(graph == NULL) {
		printf("\nCan't print graph.");
		return;
	}
	//printf("\n%d %d %d",graph->array[1].head->index, graph->array[1].head->x, graph->array[1].head->y);
	int i = 0;
	Edge * temp = NULL;
	//print out the adjacency list
	for(i = 0; i < graph->v; i++) {
		printf("\n%d: ", graph->array[i].head->index);
		temp = graph->array[i].head->first;
		//print out the linked list
		while(temp != NULL) {
			printf("%d ", temp->node->index);
			temp = temp->next;
		}
	}
}


//Creates the graph from text file
Graph * StoreGraph(FILE *fptr, int vertexCount, int edgeCount) {
	if(fptr == NULL) {
		printf("\nNo file pointer.");
		return NULL;
	}
	int i = 0;
	int src = 0; //source node
	int des = 0; //destination node
	Vertex * vertex = NULL;
	Graph * graph = NULL;
	graph = CreateGraph(vertexCount);
	Edge * prev = NULL;
	Edge * temp = NULL;
	//store all of the verticies
	for(i = 0 ; i < vertexCount; i++) {
		//create the node
		vertex = (Vertex *)malloc(sizeof(vertex));
		if(vertex == NULL) {
			printf("\nCan't allocat Vertex.");
			return NULL;
		}
		assert(vertex != NULL);
		fscanf(fptr, "%d", &(vertex->index));
		fscanf(fptr, "%d", &(vertex->x));
		fscanf(fptr, "%d", &(vertex->y));
		vertex->first = NULL;
		graph->array[i].head = vertex;			
	//	printf("\nIndex = %d X = %d, Y = %d",graph->array[i].head->index, graph->array[i].head->x, graph->array[i].head->y);
	
	}

	//store all of the connections
	for(i = 0; i < edgeCount; i++) {
		fscanf(fptr, "%d", &src);
		fscanf(fptr, "%d", &des);
		//if the edge is not allocated
		if(graph->array[src].head->first == NULL) {
			graph->array[src].head->first = malloc(sizeof(Edge));
			graph->array[src].head->first->node = graph->array[des].head;
			graph->array[src].head->first->distance = ComputeDistance(graph->array[src].head->x, graph->array[src].head->y, graph->array[des].head->x, graph->array[des].head->y);
			//printf("\n%d %d",graph->array[src].head->index, graph->array[src].head->first->node->index);
			//printf("\n%d", graph->array[src].head->first->distance);
			graph->array[src].head->first->next = NULL;
			
			//get rid of this if else if out of time 
			//make connection with destination node and connecting node
			if(graph->array[des].head->first == NULL) {
				graph->array[des].head->first = malloc(sizeof(Edge));
				graph->array[des].head->first->node = graph->array[src].head;
				graph->array[des].head->first->distance = ComputeDistance(graph->array[src].head->x, graph->array[src].head->y, graph->array[des].head->x, graph->array[des].head->y);
				graph->array[des].head->first->next = NULL;				
			}
			else{
				temp = graph->array[des].head->first;
				while(temp != NULL) {
					prev = temp;
					temp = temp->next;
				}
				temp = malloc(sizeof(Edge));
				temp->node = graph->array[src].head;
				prev->next = temp;
				temp->next = NULL;
				temp->distance = ComputeDistance(graph->array[src].head->x, graph->array[src].head->y, graph->array[des].head->x, graph->array[des].head->y);
			}
		}
		//if edge is already allocated 
		else {
			//This is for the src node
			temp = graph->array[src].head->first;
			//go to end of linked list and add new edge
			while(temp != NULL) {
				prev = temp;
				temp = temp->next;
			}
			temp = malloc(sizeof(Edge));
			temp->node = graph->array[des].head;
		//	printf("\n%d %d",graph->array[src].head->index, temp->node->index);
			prev->next = temp;
			temp->next = NULL;
			temp->distance = ComputeDistance(graph->array[src].head->x, graph->array[src].head->y, graph->array[des].head->x, graph->array[des].head->y); 
			//printf("\n%d", temp->distance);
			
			//This is for des node
			if(graph->array[des].head->first == NULL) {
				graph->array[des].head->first = malloc(sizeof(Edge));
				graph->array[des].head->first->node = graph->array[src].head;
				graph->array[des].head->first->distance = ComputeDistance(graph->array[src].head->x, graph->array[src].head->y, graph->array[des].head->x, graph->array[des].head->y);
				graph->array[des].head->first->next = NULL;
			}
			else {
				temp = graph->array[des].head->first;
				while(temp != NULL) {
					prev = temp;
					temp = temp->next;
				}
				temp = malloc(sizeof(Edge));
				temp->node = graph->array[src].head;
				prev->next = temp;
				temp->next = NULL;
				temp->distance = ComputeDistance(graph->array[src].head->x, graph->array[src].head->y, graph->array[des].head->x, graph->array[des].head->y);
			}
		}
	}
	return graph;
}


Graph * CreateGraph(int count) {
	Graph * graph = (Graph *)malloc(sizeof(Graph));
	assert(graph != NULL);
	graph->v = count; //total number of verticies
	int i = 0;
	graph->array = (VertexHead *)malloc(sizeof(Graph)*count);
	for(i = 0; i < count; i++) {
		graph->array[i].head = NULL;	
	}
	return graph;
}

//computes the distance between two nodes
int ComputeDistance(int x1, int y1, int x2, int y2) {
	int x = 0;
	int y = 0;
	int distance = 0;

	x = pow(x2-x1, 2);
	y = pow(y2-y1, 2);
	distance = sqrt((x+y));
	return distance;
}

int * getQuery(int query, FILE * sptr) {
	if(sptr == NULL || query < 1) {
		printf("\nNo queries.");
		return NULL;
	}
	int * search = (int*)malloc(sizeof(int) * (2 * query));
	int i = 0;
	int j = 0;
	for(i = 0, j = 1; i <= (2*query); i = i+2, j = j+2) {
		fscanf(sptr, "%d", &search[i]);
		fscanf(sptr, "%d", &search[j]);
		//printf("\nsource = %d", search[i]);
		//printf("\ndestination = %d", search [j]);
	}
	return search;
}

void FreeGraph(Graph * graph) {
	if(graph == NULL) {
		printf("\nGraph already free.");
		return;
	}
	int i = 0;
	Edge * temp = NULL;
	Edge * prev = NULL;
	//delete all verticies
	for(i = 0; i < graph->v; i++) {
		temp = graph->array[i].head->first;
		while(temp != NULL) {
			prev = temp;
			temp = temp->next;
			free(prev);
		}
		free(graph->array[i].head);
	}
	free(graph->array);
	free(graph);
}

//Function to test if heap works properly 
void heapTest(Heap * heap, Graph * graph) {
	if(heap == NULL || graph == NULL) {
		printf("\nHeap or Graph not allocated.");
		return;
	}

	//int j = 0; 
	int i = 0;
	int index = 0; 
	printf("\n 2 %d %d", graph->array[2].head->first->distance, graph->v);
	//update search distance
	for(i = 0; i < graph->v; i++) {
		if(graph->array[i].head->first != NULL) {
			graph->array[i].head->first->search_distance = graph->array[i].head->first->distance;
		}
		else {
			graph->array[i].head->first = malloc(sizeof(Edge));
			graph->array[i].head->first->search_distance = 200000000;
		}
	}
	//Push onto heap
	for(i = 0; i < graph->v; i++) {
		PushHeap(heap, i, graph);
		printf("\nCurrent Size = %d ", heap->CurrentSize);
	}
	printf("\nCurrent Size = %d", heap->CurrentSize);
	//Build heap more efficiently
	for(i = (heap->Capacity / 2); i != 0; i--) {
		DownHeapify(heap, i, graph);
	}
	for(i = 0; i < heap->CurrentSize; i++) {
		printf("\nIndex = %d ", heap->minheap[i]);
		if(graph->array[heap->minheap[i]].head->first != NULL) {
			printf("Distance = %d", graph->array[heap->minheap[i]].head->first->search_distance);
		}
	}
	printf("\n");
	//Emptry Heap 
	for(i = 0; i < graph->v; i++) {
		index = PopHeap(heap, graph);
		printf("\nIndex = %d Distance = %d Size = %d", index, graph->array[index].head->first->search_distance, heap->CurrentSize);  
		/*
		for(j = 0; j < heap->CurrentSize; j++) {
			printf("\nIndex = %d Distance = %d", heap->minheap[i], graph->array[heap->minheap[i]].head->first->search_distance);
		}
		*/
	}

}

void dijkstra(Heap * heap, Graph * graph, int src, int des) {
	if(heap == NULL || graph == NULL) {
		printf("\nHeap not allocated.");
		return;
	}
	if(src == des) {
		printf("0\n");
		printf("%d\n", src);
		return;
	}
	int i = 0; 
	Edge * temp = NULL;
	int index = 0; 
	int flag = 0; 
	int distance = 0; 
	//Initalize serach distance
	for(i = 0; i < graph->v; i++) {
		if(i != src) {
			//update initial conditions for other nodes
			if(graph->array[i].head->first != NULL) {
				graph->array[i].head->first->search_distance = INT_MAX;
				graph->array[i].head->visited = 0;
				graph->array[i].head->new_distance = INT_MAX;
			}
			else {
				graph->array[i].head->first = malloc(sizeof(Edge));
				graph->array[i].head->first->node = NULL;
				graph->array[i].head->first->search_distance = INT_MAX;
				graph->array[i].head->visited = 0;
				graph->array[i].head->new_distance = INT_MAX;
			}
		}
		else {
			//update initial conditions for source node
			if(graph->array[i].head->first != NULL) {
				graph->array[i].head->first->search_distance = 0;
				graph->array[i].head->visited = 0;
				graph->array[i].head->new_distance = 0;
				PushHeap(heap, src, graph); //push source node onto heap
			}
			//source node has no connections
			else {
				printf("\nINF");
				printf("\n%d %d", src, des);
				return;
			}
		}
	}

	//Find Shortest Path
	while(heap->CurrentSize != 0 && flag == 0) {
		//Dequeue node 
		index = PopHeap(heap, graph);
		//printf("\nIndex = %d Distance = %d Des = %d", index, graph->array[index].head->new_distance, des);
		//printf("%d ", index);
		graph->array[index].head->visited = 1; 
		temp = graph->array[index].head->first;
		if(index == des) {
			flag = 1; 
		}
		//parse through temp and add in new nodes
		while(index != des && temp != NULL) {
		  //printf("\nLoop = %d", temp->node->index);
		        //Edge Relaxation
			if(temp->node != NULL && temp->search_distance == INT_MAX) {
				temp->search_distance = temp->distance + graph->array[index].head->new_distance;
			    temp->node->new_distance = temp->search_distance;
				temp->PrevNode = graph->array[index].head; //links back to the previous node
				//temp->prev_index = index;
				//printf("\nPrev = %d", temp->prev_index);
			}
			else {
			  if(temp->node == NULL) {
			    temp->search_distance = temp->distance + graph->array[index].head->new_distance;
				temp->PrevNode = graph->array[index].head;
				//temp->prev_index = index;
				//printf("\nPrev = %d", temp->prev_index);
			  }
			  else {
			    distance = relax(temp, index, graph);
				//update previous pointer if shorter distance
				if(distance < temp->node->new_distance) {
					temp->PrevNode = graph->array[index].head;
					//temp->prev_index = index;
				}
			    temp->node->new_distance = distance;
			    temp->search_distance = distance;
				//printf("\nPrev = %d", temp->prev_index);
			  }
			}
			//Push onto heap
			if(temp->node != NULL && temp->node->visited == 0) {
			  PushHeap(heap, temp->node->index, graph);			
			}
			temp = temp->next;
		}
	}
	if(graph->array[des].head->new_distance == INT_MAX) {
		printf("\nINF");
		printf("\n%d %d", src, des);
	}
	else {
		printf("%d\n", graph->array[des].head->new_distance);
		//printf("\nDes = %d", des);
	}
	index = des;
	
	//clear heap if still full
	while(heap->CurrentSize != 0) {
		index = PopHeap(heap, graph);
	}

	/*
	while(index != src) {
		printf("%d ", index);
		index = graph->array[index].head->first->prev_index;
	}
	*/		
}

int relax(Edge * temp, int index, Graph * graph) {
  if(temp == NULL || graph == NULL || index == -1) {
    printf("\nMemory not allocated or index out of bounds.");
    return (-1);
  }
  int distance = 0; 
  distance = temp->distance + graph->array[index].head->new_distance;
  if(distance < temp->node->new_distance && temp->node->visited != 1) {
    return distance;
  }
  else {
    return temp->node->new_distance;
  }
}

