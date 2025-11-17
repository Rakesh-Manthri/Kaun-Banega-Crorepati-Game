#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>

typedef struct Question {
    int id;
    char questionText[512];
    char options[4][100];
    int correctOption;
    char category[50];
    int difficulty;
    struct Question *next;
} Question;

typedef struct Contestant {
    int id;
    char name[100];
    int age;
    char gender[10];
    int questionsAnswered;
    int lifelinesUsed;
    long prizeWon;
} Contestant;

typedef struct {
    int f, r, s;
    Question *arr;
} Queue;

typedef struct {
    int top, size;
    Question *a;
} Stack;

typedef struct {
    Queue* quizQueue;
    Stack* lifelineStack;
} QuizData;

extern Question *easyQuestions;
extern Question *mediumQuestions;
extern Question *hardQuestions;
int loadQuestionsFromFile(const char* filename);
int saveQuestionsToFile(const char* filename);
void addQuestionToList(Question* newQuestion);
int deleteQuestionById(int id);
void cleanupQuestions();

Queue* createQueue(int size);
int isQueueEmpty(Queue *q);
void enqueue(Queue *q, Question item);
Question dequeue(Queue *q);

Stack* createStack(int size);
int isStackEmpty(Stack *s);
void push(Stack *s, Question item);
Question pop(Stack *s);
Question peek(Stack *s);

QuizData* getQuizData();
int saveContestantToFile(const char* filename, Contestant contestant);

#endif