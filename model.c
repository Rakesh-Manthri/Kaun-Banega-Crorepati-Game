#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "model.h"

Question *easyQuestions = NULL;
Question *mediumQuestions = NULL;
Question *hardQuestions = NULL;

void insertNode(Question** head, Question* newNode) {
    newNode->next = *head;
    *head = newNode;
}

void parseLine(char* line, char* value) {
    char* colon = strchr(line, ':');
    if (colon != NULL) {
        strcpy(value, colon + 1);
        value[strcspn(value, "\r\n")] = 0;
    }
}

int loadQuestionsFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
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

        fgets(line, sizeof(line), file);

        addQuestionToList(newQ);
        free(newQ); // addQuestionToList makes its own copy, so free the temporary one
    }

    fclose(file);
    return 1;
}

void addQuestionToList(Question* newQuestionData) {
    Question* newNode = (Question*)malloc(sizeof(Question));
    if (newNode == NULL) return;
    
    *newNode = *newQuestionData; // Copy all data
    newNode->next = NULL;

    switch (newNode->difficulty) {
        case 0:
            insertNode(&easyQuestions, newNode);
            break;
        case 1:
            insertNode(&mediumQuestions, newNode);
            break;
        case 2:
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
                if (prev == NULL) {
                    *(heads[i]) = current->next;
                } else {
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

int countQuestions(Question* head) {
    int count = 0;
    Question* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

Question* getQuestionAtIndex(Question* head, int index) {
    Question* current = head;
    for (int i = 0; i < index; i++) {
        if (current == NULL) return NULL;
        current = current->next;
    }
    return current;
}

Queue* createQueue(int size) {
    Queue *q = malloc(sizeof(Queue));
    q->f = q->r = -1;
    q->s = size;
    q->arr = malloc(q->s * sizeof(Question));
    return q;
}

int isQueueEmpty(Queue *q) {
    return (q->f == -1 || q->f > q->r);
}

void enqueue(Queue *q, Question item) {
    if (q->r == q->s - 1) {
        printf("Queue is full.\n");
        return;
    }
    if (q->f == -1) {
        q->f = 0;
    }
    q->arr[++q->r] = item;
}

Question dequeue(Queue *q) {
    return q->arr[q->f++];
}

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

        for (int j = 0; j < requiredForQueue[i]; j++) {
            int rand_index;
            do {
                rand_index = rand() % counts[i];
            } while (selected_indices[rand_index] == 1); // Ensure we don't pick the same question twice
            
            selected_indices[rand_index] = 1;
            Question* q = getQuestionAtIndex(lists[i], rand_index);
            if (q != NULL) enqueue(data->quizQueue, *q);
        }

        for (int j = 0; j < requiredForStack[i]; j++) {
            int rand_index;
            do {
                rand_index = rand() % counts[i];
            } while (selected_indices[rand_index] == 1);

            selected_indices[rand_index] = 1;
            Question* q = getQuestionAtIndex(lists[i], rand_index);
            if (q != NULL) {
                if (i == 0) push(data->lifelineStack, *q);
                else if (i == 1) push(data->lifelineStack, *q);
                else if (i == 2) push(data->lifelineStack, *q);
            }
        }
        free(selected_indices);
    }
    return data;
}

int getLastContestantId() {
    FILE* file = fopen("participants.txt", "r");
    if (file == NULL) {
        return 0; // If file doesn't exist, start IDs from 1 (0+1)
    }

    char lastLine[256] = "";
    char currentLine[256];
    
    // Read file line by line to get the last one
    while (fgets(currentLine, sizeof(currentLine), file) != NULL) {
        strcpy(lastLine, currentLine);
    }
    fclose(file);

    if (strlen(lastLine) > 0) {
        int lastId = 0;
        sscanf(lastLine, "ID:%d,", &lastId);
        return lastId;
    }

    return 0; // If file is empty, start IDs from 1 (0+1)
}

int saveContestantToFile(const char* filename, Contestant contestant) {
    FILE* file = fopen(filename, "a");
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
