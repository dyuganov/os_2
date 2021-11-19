#define STR_BUF_SIZE (80)

struct node;
typedef struct node Node;
struct list;
typedef struct list List;

Node* createNode();
void freeNode(Node* currentNode);
List* createList();
char* createString(unsigned int length);
int addString(List* list, const char* newLine);
void deleteList(List* list);
void printList(List* list);
void bubbleSort(List* list);
