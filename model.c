// model.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "model.h"

// Definition of the global linked list heads
Question *easyQuestions = NULL;
Question *mediumQuestions = NULL;
Question *hardQuestions = NULL;

// Helper function to insert a node into a list
void insertNode(Question** head, Question* newNode) {
    newNode->next = *head;
    *head = newNode;
}

// Helper to parse a line like "KEY:Value"
void parseLine(char* line, char* value) {
    char* colon = strchr(line, ':');
    if (colon != NULL) {
        strcpy(value, colon + 1);
        // Trim leading/trailing whitespace, especially the newline
        value[strcspn(value, "\r\n")] = 0;
    }
}

int loadQuestionsFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        // If the file doesn't exist, it's not an error.
        // It just means we start with no questions.
        return 1; 
    }

    char line[512];
    while (fgets(line, sizeof(line), file) != NULL) {
        Question* newQ = (Question*)malloc(sizeof(Question));
        if (newQ == NULL) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            fclose(file);
            return 0;
        }
        newQ->next = NULL;

        // Use a structured way to read key-value pairs
        char value[512];
        
        parseLine(line, value); // ID:101
        newQ->id = atoi(value);

        fgets(line, sizeof(line), file); // QUESTION:Text
        parseLine(line, newQ->questionText);

        for (int i = 0; i < 4; i++) {
            fgets(line, sizeof(line), file); // OPTx:Text
            parseLine(line, newQ->options[i]);
        }

        fgets(line, sizeof(line), file); // CORRECT:3
        parseLine(line, value);
        newQ->correctOption = atoi(value);

        fgets(line, sizeof(line), file); // CATEGORY:Text
        parseLine(line, newQ->category);

        fgets(line, sizeof(line), file); // DIFFICULTY:0
        parseLine(line, value);
        newQ->difficulty = atoi(value);

        // Read the separator "---"
        fgets(line, sizeof(line), file);

        // Add the new question to the correct list
        addQuestionToList(newQ);
        free(newQ); // addQuestionToList makes its own copy, so free the temporary one
    }

    fclose(file);
    return 1;
}

void addQuestionToList(Question* newQuestionData) {
    // Create a new node and copy the data
    Question* newNode = (Question*)malloc(sizeof(Question));
    if (newNode == NULL) return;
    
    *newNode = *newQuestionData; // Copy all data
    newNode->next = NULL;

    // Insert into the correct list based on difficulty
    switch (newNode->difficulty) {
        case 0: // Easy
            insertNode(&easyQuestions, newNode);
            break;
        case 1: // Medium
            insertNode(&mediumQuestions, newNode);
            break;
        case 2: // Hard
            insertNode(&hardQuestions, newNode);
            break;
        default: // Invalid difficulty, free the allocated memory
            fprintf(stderr, "Warning: Invalid difficulty %d for question ID %d.\n", newNode->difficulty, newNode->id);
            free(newNode);
            break;
    }
}

int saveQuestionsToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s for writing.\n", filename);
        return 0;
    }

    Question* lists[] = {easyQuestions, mediumQuestions, hardQuestions};
    for (int i = 0; i < 3; i++) {
        Question* current = lists[i];
        while (current != NULL) {
            fprintf(file, "ID:%d\n", current->id);
            fprintf(file, "QUESTION:%s\n", current->questionText);
            for (int j = 0; j < 4; j++) {
                fprintf(file, "OPT%d:%s\n", j + 1, current->options[j]);
            }
            fprintf(file, "CORRECT:%d\n", current->correctOption);
            fprintf(file, "CATEGORY:%s\n", current->category);
            fprintf(file, "DIFFICULTY:%d\n", current->difficulty);
            fprintf(file, "---\n");
            current = current->next;
        }
    }

    fclose(file);
    return 1;
}

int deleteQuestionById(int id) {
    Question* lists[] = {easyQuestions, mediumQuestions, hardQuestions};
    Question** heads[] = {&easyQuestions, &mediumQuestions, &hardQuestions};

    for (int i = 0; i < 3; i++) {
        Question* current = lists[i];
        Question* prev = NULL;
        
        while (current != NULL) {
            if (current->id == id) {
                if (prev == NULL) { // Deleting the head node
                    *(heads[i]) = current->next;
                } else { // Deleting a middle or tail node
                    prev->next = current->next;
                }
                free(current);
                return 1; // Found and deleted
            }
            prev = current;
            current = current->next;
        }
    }
    return 0; // Not found
}

// Helper function to free a single list
void freeList(Question* head) {
    Question* current = head;
    Question* nextNode;
    while (current != NULL) {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
}

void cleanupQuestions() {
    freeList(easyQuestions);
    freeList(mediumQuestions);
    freeList(hardQuestions);
    easyQuestions = mediumQuestions = hardQuestions = NULL;
}

// Helper function to get the count of questions in a list
int countQuestions(Question* head) {
    int count = 0;
    Question* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// Helper function to get a question at a specific index in a list
Question* getQuestionAtIndex(Question* head, int index) {
    Question* current = head;
    for (int i = 0; i < index; i++) {
        if (current == NULL) return NULL;
        current = current->next;
    }
    return current;
}

// --- Queue Function Implementations ---

Queue* createQueue(int size) {
    Queue *q = malloc(sizeof(Queue));
    q->f = q->r = -1;
    q->s = size;
    q->arr = malloc(q->s * sizeof(Question));
    return q;
}

int isQueueEmpty(Queue *q) {
    // The queue is empty if front is -1 or if front has passed rear
    return (q->f == -1 || q->f > q->r);
}

void enqueue(Queue *q, Question item) {
    if (q->r == q->s - 1) { // Check if the queue is full
        printf("Queue is full.\n");
        return;
    }
    if (q->f == -1) { // If it's the first element
        q->f = 0;
    }
    q->arr[++q->r] = item;
}

Question dequeue(Queue *q) {
    // It's good practice to check if empty before calling dequeue,
    // but we return the element at the front and increment the front index.
    return q->arr[q->f++];
}


// --- Stack Function Implementations ---

Stack* createStack(int size) {
    Stack *s = malloc(sizeof(Stack));
    s->size = size;
    s->top = -1;
    s->a = (Question *)malloc(s->size * sizeof(Question));
    return s;
}

int isStackEmpty(Stack *s) {
    return s->top == -1;
}

void push(Stack *s, Question item) {
    if (s->top == s->size - 1) {
        printf("Stack is full.\n");
        return;
    }
    s->a[++s->top] = item;
}

Question pop(Stack *s) {
    return s->a[s->top--];
}

Question peek(Stack *s) {
    return s->a[s->top];
}

QuizData* getQuizData() {
    QuizData* data = malloc(sizeof(QuizData));
    data->quizQueue = createQueue(10);
    data->lifelineStack = createStack(3);

    Question* lists[] = {easyQuestions, mediumQuestions, hardQuestions};
    int counts[] = {countQuestions(easyQuestions), countQuestions(mediumQuestions), countQuestions(hardQuestions)};
    // We need +1 for each difficulty for the lifeline stack
    int requiredForQueue[] = {3, 3, 4};
    int requiredForStack[] = {1, 1, 1};

    for (int i = 0; i < 3; i++) { // For each difficulty level
        if (counts[i] < (requiredForQueue[i] + requiredForStack[i])) {
            fprintf(stderr, "Error: Not enough questions of difficulty %d. Need %d, have %d.\n", i, requiredForQueue[i] + requiredForStack[i], counts[i]);
            free(data->quizQueue->arr);
            free(data->quizQueue);
            free(data->lifelineStack->a);
            free(data->lifelineStack);
            free(data);
            return NULL; // Not enough questions to build a quiz
        }

        int* selected_indices = calloc(counts[i], sizeof(int));
        if (selected_indices == NULL) return NULL;

        // Select questions for the main quiz queue
        for (int j = 0; j < requiredForQueue[i]; j++) {
            int rand_index;
            do {
                rand_index = rand() % counts[i];
            } while (selected_indices[rand_index] == 1); // Ensure we don't pick the same question twice
            
            selected_indices[rand_index] = 1;
            Question* q = getQuestionAtIndex(lists[i], rand_index);
            if (q != NULL) enqueue(data->quizQueue, *q);
        }

        // Select one more unique question for the lifeline stack
        for (int j = 0; j < requiredForStack[i]; j++) {
            int rand_index;
            do {
                rand_index = rand() % counts[i];
            } while (selected_indices[rand_index] == 1);

            selected_indices[rand_index] = 1;
            Question* q = getQuestionAtIndex(lists[i], rand_index);
            // The stack is populated later to ensure correct order
            if (q != NULL) {
                if (i == 0) push(data->lifelineStack, *q); // Easy
                else if (i == 1) push(data->lifelineStack, *q); // Medium
                else if (i == 2) push(data->lifelineStack, *q); // Hard
            }
        }
        free(selected_indices);
    }
    // The logic above pushes easy, then medium, then hard, so hard is at the top.
    return data;
}

int saveContestantToFile(const char* filename, Contestant contestant) {
    FILE* file = fopen(filename, "a"); // Open in append mode
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s for appending.\n", filename);
        return 0;
    }

    fprintf(file, "ID:%d, Name:%s, Age:%d, Gender:%s, QuestionsAnswered:%d, Prize:Rs.%ld\n",
            contestant.id, contestant.name, contestant.age, contestant.gender,
            contestant.questionsAnswered, contestant.prizeWon);

    fclose(file);
    return 1;
}
