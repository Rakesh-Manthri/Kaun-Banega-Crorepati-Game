// model.h

#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>

// --- Data Structures ---

// Represents a single question node in a linked list
typedef struct Question {
    int id;
    char questionText[512];
    char options[4][100];
    int correctOption; // Stored as 1-4
    char category[50];
    int difficulty; // 0: Easy, 1: Medium, 2: Hard
    struct Question *next;
} Question;

// Represents a single contestant's data
typedef struct Contestant {
    int id;
    char name[100];
    int age;
    char gender[10];
    int questionsAnswered;
    int lifelinesUsed;
    long prizeWon;
} Contestant;

// Represents a queue of Questions
typedef struct {
    int f, r, s; // front, rear, size
    Question *arr;
} Queue;

// Represents a stack of Questions
typedef struct {
    int top, size;
    Question *a;
} Stack;

// Bundles the Queue and Stack for a quiz session
typedef struct {
    Queue* quizQueue;
    Stack* lifelineStack;
} QuizData;

// Global heads for the three question linked lists
extern Question *easyQuestions;
extern Question *mediumQuestions;
extern Question *hardQuestions;


// --- Function Prototypes ---
int loadQuestionsFromFile(const char* filename);
int saveQuestionsToFile(const char* filename);
void addQuestionToList(Question* newQuestion);
int deleteQuestionById(int id);
void cleanupQuestions();

// --- Queue Functions ---
Queue* createQueue(int size);
int isQueueEmpty(Queue *q);
void enqueue(Queue *q, Question item);
Question dequeue(Queue *q);

// --- Stack Functions ---
Stack* createStack(int size);
int isStackEmpty(Stack *s);
void push(Stack *s, Question item);
Question pop(Stack *s);
Question peek(Stack *s);

QuizData* getQuizData();
int saveContestantToFile(const char* filename, Contestant contestant);

#endif // MODEL_H