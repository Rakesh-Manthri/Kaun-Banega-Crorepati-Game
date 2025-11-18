#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "controller.h"
#include "view.h"
#include "model.h"

static int performAdminLogin();
static void handleViewQuestions();
static void handleAddQuestion();
static void handleDeleteQuestion();
static void shuffleQuestionOptions(Question *q);
static void useFiftyFifty(Question *q);
static void useAudiencePoll(Question *q, int percentages[]);
static int handleLifeline(Question *currentQ, int lifelines[], Contestant *player, Stack* lifelineStack);
static void handleLeaderboard();

// ANSI Color Codes - Duplicated here for controller-specific messages
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static int performAdminLogin() {
    char username[50];
    char password[50];
    
    getAdminCredentials(username, password);

    if (strcmp(username, "admin") == 0 && strcmp(password, "pass123") == 0) {
        printf(ANSI_COLOR_GREEN "\nLogin Successful!\n" ANSI_COLOR_RESET);
        return 1;
    } else {
        printf(ANSI_COLOR_RED "\nInvalid credentials!\n" ANSI_COLOR_RESET);
        return 0;
    }
}

void adminController() {
    if (!performAdminLogin()) {
        return;
    }

    int choice;
    do {
        displayAdminMenu();
        choice = getMenuChoice();

        switch (choice) {
            case 1:
                handleViewQuestions();
                break;
            case 2:
                handleAddQuestion();
                break;
            case 3:
                handleDeleteQuestion();
                break;
            case 4:
                handleLeaderboard();
                break;
            case 5:
                printf(ANSI_COLOR_YELLOW "\nLogging out...\n" ANSI_COLOR_RESET);
                break;
            default:
                printf(ANSI_COLOR_RED "\nInvalid choice. Please try again.\n" ANSI_COLOR_RESET);
                break;
        }
    } while (choice != 5);
}

static void handleAddQuestion() {
    Question newQ = getQuestionDetailsFromAdmin();
    addQuestionToList(&newQ);
    if (saveQuestionsToFile("questions.txt")) {
        printf(ANSI_COLOR_GREEN "\nQuestion added successfully!\n" ANSI_COLOR_RESET);
    } else {
        printf(ANSI_COLOR_RED "\nError: Could not save the question to the file.\n" ANSI_COLOR_RESET);
    }
}

static void handleDeleteQuestion() {
    int id = getQuestionIdForDeletion();
    if (deleteQuestionById(id)) {
        if (saveQuestionsToFile("questions.txt")) {
            printf(ANSI_COLOR_GREEN "\nQuestion with ID %d deleted successfully.\n" ANSI_COLOR_RESET, id);
        } else {
            printf(ANSI_COLOR_RED "\nError: Could not update the questions file.\n" ANSI_COLOR_RESET);
        }
    } else {
        printf(ANSI_COLOR_RED "\nQuestion with ID %d not found.\n" ANSI_COLOR_RESET, id);
    }
}

static void handleViewQuestions() {
    int filterChoice = getFilterChoiceForViewing();
    int difficulty = -1;
    char category[50] = "";

    if (filterChoice == 1) {
        difficulty = getDifficultyLevelFromAdmin();
    }
    else if (filterChoice == 2) {
        getCategoryFromAdmin(category);
    }

    printf("\n--- " ANSI_COLOR_YELLOW "Displaying Questions" ANSI_COLOR_RESET " ---\n");
    displayFilteredQuestions(difficulty, category);
}

int compareContestants(const void* a, const void* b) {
    Contestant* c1 = (Contestant*)a;
    Contestant* c2 = (Contestant*)b;
    if (c1->prizeWon < c2->prizeWon) return 1;
    if (c1->prizeWon > c2->prizeWon) return -1;
    return 0;
}

static void handleLeaderboard() {
    FILE* file = fopen("participants.txt", "r");
    if (file == NULL) {
        printf(ANSI_COLOR_RED "\nNo participant data found to generate a leaderboard.\n" ANSI_COLOR_RESET);
        return;
    }

    Contestant contestants[100]; // Assuming a max of 100 participants for simplicity
    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL && count < 100) {
        sscanf(line, "ID:%d, Name:%[^,], Age:%d, Gender:%[^,], QuestionsAnswered:%d, Prize:Rs.%ld",
               &contestants[count].id,
               contestants[count].name,
               &contestants[count].age,
               contestants[count].gender,
               &contestants[count].questionsAnswered,
               &contestants[count].prizeWon);
        count++;
    }
    fclose(file);

    if (count == 0) {
        printf(ANSI_COLOR_RED "\nNo participant data found.\n" ANSI_COLOR_RESET);
        return;
    }

    qsort(contestants, count, sizeof(Contestant), compareContestants);

    displayLeaderboard(contestants, count);
}

static void shuffleQuestionOptions(Question *q) {
    char correctOptionText[100];
    strcpy(correctOptionText, q->options[q->correctOption - 1]);

    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);

        char temp[100];
        strcpy(temp, q->options[i]);
        strcpy(q->options[i], q->options[j]);
        strcpy(q->options[j], temp);
    }

    for (int i = 0; i < 4; i++) {
        if (strcmp(q->options[i], correctOptionText) == 0) {
            q->correctOption = i + 1;
            break;
        }
    }
}

static void useFiftyFifty(Question *q) {
    int removed_count = 0;
    while (removed_count < 2) {
        int rand_option = rand() % 4;

        if ((rand_option + 1 != q->correctOption) && (strlen(q->options[rand_option]) > 0)) {
            strcpy(q->options[rand_option], "");
            removed_count++;
        }
    }
}

static void useAudiencePoll(Question *q, int percentages[]) {
    for (int i = 0; i < 4; i++) percentages[i] = 0;

    int correct_idx = q->correctOption - 1;
    int remaining_percentage = 100;

    if (q->difficulty == 0) {
        percentages[correct_idx] = 60 + (rand() % 21);
    } else if (q->difficulty == 1) {
        percentages[correct_idx] = 40 + (rand() % 21);
    } else {
        percentages[correct_idx] = 25 + (rand() % 26);
    }
    remaining_percentage -= percentages[correct_idx];

    int incorrect_indices[3];
    int k = 0;
    for (int i = 0; i < 4; i++) {
        if (i != correct_idx) {
            incorrect_indices[k++] = i;
        }
    }

    if (remaining_percentage > 0) {
        int p1 = rand() % (remaining_percentage + 1);
        int p2 = rand() % (remaining_percentage - p1 + 1);
        int p3 = remaining_percentage - p1 - p2;

        percentages[incorrect_indices[0]] = p1;
        percentages[incorrect_indices[1]] = p2;
        percentages[incorrect_indices[2]] = p3;
    }
}

static int handleLifeline(Question *currentQ, int lifelines[], Contestant *player, Stack* lifelineStack) {
    displayLifelineMenu(lifelines);
    int choice = getMenuChoice();
    int question_was_flipped = 0;
    
    switch (choice) {
        case 1:
            if (lifelines[0]) {
                printf("\nUsing 50-50 lifeline...\n");
                useFiftyFifty(currentQ);
                lifelines[0] = 0;
                player->lifelinesUsed++;
            } else {
                printf("\nYou have already used the 50-50 lifeline!\n");
            }
            break;
        case 2:
            if (lifelines[1]) {
                lifelines[1] = 0;
                printf("\nFlipping the question...\n");
                int found_replacement = 0;
                while (!isStackEmpty(lifelineStack)) {
                    Question replacement = peek(lifelineStack);
                    if (replacement.difficulty == currentQ->difficulty) {
                        *currentQ = pop(lifelineStack);
                        player->lifelinesUsed++;
                        question_was_flipped = 1;
                        found_replacement = 1;
                        break;
                    } else {
                        pop(lifelineStack);
                    }
                }
                if (!found_replacement) {
                     printf(ANSI_COLOR_RED "\nSorry, no replacement question of the same difficulty was available. You have lost the lifeline.\n" ANSI_COLOR_RESET);
                }

            } else {
                printf(ANSI_COLOR_RED "\nYou have already used the Flip the Question lifeline!\n" ANSI_COLOR_RESET);
            }
            break;
        case 3:
            if (lifelines[2]) {
                printf("\nAsking the audience...\n");
                int poll_results[4];
                useAudiencePoll(currentQ, poll_results);
                displayAudiencePoll(poll_results);
                lifelines[2] = 0;
                player->lifelinesUsed++;
            } else {
                printf(ANSI_COLOR_RED "\nYou have already used the Audience Poll lifeline!\n" ANSI_COLOR_RESET);
            }
            break;
        default:
            printf(ANSI_COLOR_YELLOW "\nReturning to the question.\n" ANSI_COLOR_RESET);
            break;
    }

    return question_was_flipped;
}

void contestantController() {
    Contestant player = getContestantDetails();
    player.id = getLastContestantId() + 1;

    displayRules();
    printf("\nPress Enter to start the quiz...");
    getchar();

    QuizData* quizData = getQuizData();
    if (quizData == NULL) {
        printf(ANSI_COLOR_RED "\nCould not start the quiz due to an insufficient number of questions. Please contact the admin.\n" ANSI_COLOR_RESET);
        return;
    }
    Queue* quizQueue = quizData->quizQueue;
    Stack* lifelineStack = quizData->lifelineStack;

    long prizeLevels[] = {1000, 5000, 10000, 40000, 160000, 640000, 1250000, 2500000, 10000000, 70000000};
    int lifelines[3] = {1, 1, 1};
    int isPlaying = 1;
    long safePrize = 0;
    int questionNumber = 0;

    while (!isQueueEmpty(quizQueue)) {
        Question currentQ = dequeue(quizQueue);
        long currentPrize = prizeLevels[questionNumber];
        questionNumber++;

        shuffleQuestionOptions(&currentQ);

        int answer_given = 0;
        int timer_needs_reset = 1;
        time_t startTime;

        while (!answer_given) {
            displayQuizQuestion(currentQ, questionNumber, currentPrize);

            if (timer_needs_reset) {
                startTime = time(NULL);
                timer_needs_reset = 0;
            }

            int timeLimit = 0;
            if (currentQ.difficulty == 0) timeLimit = 30;
            if (currentQ.difficulty == 1) timeLimit = 60;

            int answer = getAnswerFromUser();
            time_t endTime = time(NULL);

            if (answer == 5) {
                if (handleLifeline(&currentQ, lifelines, &player, lifelineStack)) {
                    shuffleQuestionOptions(&currentQ);
                    timer_needs_reset = 1;
                }
            } else if (answer == 0) {
                printf("\nYou have chosen to quit the game.\n");
                isPlaying = 0;
                answer_given = 1;
            } else if (timeLimit > 0 && (endTime - startTime) > timeLimit) {
                printf(ANSI_COLOR_RED "\nTime's up!\n" ANSI_COLOR_RESET);
                player.prizeWon = safePrize;
                isPlaying = 0;
                answer_given = 1;
            } else if (answer >= 1 && answer <= 4) {
                if (answer == currentQ.correctOption) {
                    displayAnswerResult(1);
                    player.prizeWon = currentPrize;
                    player.questionsAnswered++;
                    if (questionNumber == 3) {
                        safePrize = currentPrize;
                    } else if (questionNumber == 6) {
                        safePrize = currentPrize;
                    }
                } else {
                    displayAnswerResult(0);
                    player.prizeWon = safePrize;
                    isPlaying = 0;
                }
                answer_given = 1;
            }
            else {
                printf(ANSI_COLOR_RED "\nInvalid input. Please choose an option from 1-4, or 5 for a lifeline.\n" ANSI_COLOR_RESET);
            }
        }

        if (!isPlaying) break;

        printf("Press Enter to proceed to the next question...");
        getchar();
    }

    displayGameOver(player);
    saveContestantToFile("participants.txt", player);

    free(quizQueue->arr);
    free(quizQueue);
    free(lifelineStack->a);
    free(lifelineStack);
    free(quizData);
}
