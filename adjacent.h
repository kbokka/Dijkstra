#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

//Graph Data type
//Contains the vertex node's information
typedef struct Vertex {
	int x;
	int y;
	int index;
	int visited;
	int new_distance;
	struct Edge * first;
}Vertex;

//Contains the Edges
typedef struct Edge {
	int distance;
	int search_distance;
	Vertex * PrevNode;
	//int prev_index;
	Vertex * node;
	struct Edge * next;
}Edge;

//Points to the head of the vertex
typedef struct VertexHead {
	Vertex * head;
}VertexHead;

typedef struct Graph {
	int v; //number of verticies
	VertexHead * array;
}Graph;



//Heap Data Type
typedef struct Heap {
	int CurrentSize;
	int Capacity;
	int * minheap;
}Heap;


//Graph Function Declerations
Graph * StoreGraph(FILE * fptr, int vertex, int edge);
Graph * CreateGraph(int count);
void printGraph(Graph *graph);
int ComputeDistance(int x1, int y1, int x2, int y2);
int *getQuery(int query, FILE * sptr);
void FreeGraph(Graph * graph);
//Heap Function Declerations 
Heap * CreateHeap(int cap);
void UpHeapify(Heap * heap, int index, Graph * graph);
void PushHeap(Heap * heap, int index, Graph * graph);
void DownHeapify(Heap * heap, int index,  Graph * graph);
int PopHeap(Heap * heap, Graph * graph);

//Other Function Declerations
int *getQuery(int query, FILE * sptr);
void dijkstra(Heap * heap, Graph * graph, int src, int des);
void heapTest(Heap * heap, Graph * graph);
int relax(Edge * temp, int index, Graph * graph);

//Heap Functions
Heap * CreateHeap(int cap) {
	if(cap < 0) {
		printf("\nInvalid vertex count.");
		return NULL;
	}	
	Heap * heap = (Heap *)malloc(sizeof(Heap));
	if(heap == NULL) {
		printf("\nCan't allocated enough heap space.");
		return NULL;
	}
	heap->Capacity = cap+1;
	heap->CurrentSize = 0; 
	heap->minheap = (int *)malloc((cap) * sizeof(int));
	return heap;
}
void PushHeap(Heap * heap, int index, Graph * graph) {
	if(heap == NULL) {
		printf("\nCan't insert into heap.");
		return;
	}
	if(heap->CurrentSize == 0) {
		heap->minheap[0] = index;
		heap->CurrentSize = 1;
		//printf("\n%d ", heap->minheap[0]);
	}
	else {
		if(heap->CurrentSize <= heap->Capacity) {
			heap->CurrentSize += 1;
			heap->minheap[heap->CurrentSize - 1] = index;
			UpHeapify(heap, heap->CurrentSize - 1, graph);
		}
		else {
			//printf("\nHeap is full.");
			return;
		}
	}
}
void UpHeapify(Heap * heap, int index, Graph * graph) {
	if(heap == NULL || graph == NULL) {
		printf("\nHeap or graph not allocated.");
		return;
	}
	int temp = heap->minheap[index];
	int child = heap->CurrentSize - 1;
	int parent = (int)((child - 1) / 2);
	//while(graph->array[heap->minheap[parent]].head->first->search_distance > graph->array[temp].head->first->search_distance && child > 0) {
	while(graph->array[heap->minheap[parent]].head->new_distance > graph->array[temp].head->new_distance && child > 0) {
		heap->minheap[child] = heap->minheap[parent];
		child = parent;
		parent = (int)((child - 1) / 2);
	}
	heap->minheap[child] = temp;
}
/*
void UpHeapify(Heap *heap, int index, Graph * graph) {
	if(heap == NULL || graph == NULL) {
		printf("\nHeap or graph not allocated.");
		return;
	}
	int pos = index; //minheap index value of the new insertion
	int child = heap->minheap[heap->CurrentSize - 1]; //graph index value of the new inseration
	int parent = (int)((pos - 1) / 2);//minheap parent index value of the new insertion
	//printf("\n%d", index);
	int swap = 0; //temp variable for swap

	//shift the new element into correct position
	while(graph->array[heap->minheap[parent]].head->first->search_distance > graph->array[child].head->first->search_distance && pos > 0) {
		//swap parent and child values in minheap
		swap = heap->minheap[parent];
		heap->minheap[parent] = heap->minheap[pos];
		heap->minheap[pos] = swap;
		pos = parent;
		child = heap->minheap[pos];
		parent = (int)((pos - 1) / 2);
	}
	heap->minheap[pos] = child;
}
*/
//Get remove the root node in the heap
int PopHeap(Heap * heap, Graph * graph) {
	if(heap == NULL || graph == NULL) {
		return - 1;
	}
	if(heap->CurrentSize == 0) {
		//printf("\nNothing in heap.");
		return -1;
	}
	int min = 0; 
	min = heap->minheap[0];
	if(heap->CurrentSize == 1) {
		heap->CurrentSize = heap->CurrentSize - 1;
	}
	else {
		heap->minheap[0] = heap->minheap[heap->CurrentSize - 1];
		heap->CurrentSize = heap->CurrentSize - 1;
		DownHeapify(heap, 1,  graph);
	}
	return min;
}

void DownHeapify(Heap * heap, int index,  Graph * graph) {
	if(heap == NULL || graph == NULL) {
		printf("\nGraph and Heap not allocated.");
		return;
	}
	/*
	if(heap->CurrentSize == 1 || heap->CurrentSize == 0) {
		printf("\nHeap has either one element or no elements.");
		return;
	}
	*/
	int temp = heap->minheap[index - 1];
	int i = index;
	int j = 0; 
	
	while(i <= (heap->CurrentSize / 2)) {
		j = 2 * i;
		//if(j < heap->CurrentSize && graph->array[heap->minheap[j-1]].head->first->search_distance >= graph->array[heap->minheap[j]].head->first->search_distance) {
		if(j < heap->CurrentSize && graph->array[heap->minheap[j-1]].head->new_distance >= graph->array[heap->minheap[j]].head->new_distance) {
			j = j + 1;
		}
		//if(graph->array[temp].head->first->search_distance <= graph->array[heap->minheap[j-1]].head->first->search_distance) {
		if(graph->array[temp].head->new_distance <= graph->array[heap->minheap[j-1]].head->new_distance) {
			break;
		}
		else {
			heap->minheap[i-1] = heap->minheap[j-1];
			i = j;
		}
		heap->minheap[i-1] = temp;
	}
	//heap->minheap[i-1] = temp;

}
/*
void DownHeapify(Heap *heap, Graph * graph) {
	if(heap == NULL || graph == NULL) {
		printf("\nGraph and Heap not allocated.");
		return;
	}
	if(heap->CurrentSize == 1 || heap->CurrentSize == 0) {
		printf("\nHeap has either one element or no elements.");
		return;
	}
	int parent = 0;//parent index of minheap
	int left = (2 * parent) + 1;//left child index of minheap
	int right = (2 * parent) + 2;//right child index of minheap
	int swap = 0; 

	while((parent < heap->CurrentSize && left < heap->CurrentSize && right < heap->CurrentSize) || graph->array[heap->minheap[parent]].head->first->search_distance > graph->array[heap->minheap[left]].head->first->search_distance || graph->array[heap->minheap[parent]].head->first->search_distance > graph->array[heap->minheap[right]].head->first->search_distance) {
		if(graph->array[heap->minheap[parent]].head->first->search_distance > graph->array[heap->minheap[left]].head->first->search_distance) {
			swap = heap->minheap[left];
			heap->minheap[left] = heap->minheap[parent];
			heap->minheap[parent] = swap;
			parent = left;
			left = (2 * parent) + 1;
			right = (2 * parent) + 2;
		}
		else if(graph->array[heap->minheap[parent]].head->first->search_distance > graph->array[heap->minheap[right]].head->first->search_distance) {
			swap = heap->minheap[right];
			heap->minheap[right] = heap->minheap[parent];
			heap->minheap[parent] = swap;
			parent = right;
			left = (2 * parent) + 1;
			right = (2 * parent) + 2;
		}
		else {
			break;
		}
	}

}
*/

