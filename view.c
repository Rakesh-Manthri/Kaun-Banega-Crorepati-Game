#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include "view.h"
#include "model.h"

// ANSI Color Codes
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BOLD_WHITE "\x1b[1;37m"

void displayMainMenu() {
    printf("\n=========================\n");
    printf("    " ANSI_COLOR_YELLOW "WELCOME TO KBC" ANSI_COLOR_RESET "\n");
    printf("=========================\n");
    printf("1. Admin Login\n");
    printf("2. Play as Contestant\n");
    printf("3. Exit\n");
    printf(ANSI_COLOR_CYAN "Enter your choice: " ANSI_COLOR_RESET);
}

int getMenuChoice() {
    int choice = 0;
    scanf("%d", &choice);
    while(getchar() != '\n');
    return choice;
}

void displayAdminMenu() {
    printf("\n--- " ANSI_COLOR_YELLOW "Admin Panel" ANSI_COLOR_RESET " ---\n");
    printf("1. View Questions\n");
    printf("2. Add Question\n");
    printf("3. Delete Question\n");
    printf("4. View Leaderboard\n");
    printf("5. Logout\n");
    printf(ANSI_COLOR_CYAN "Enter your choice: " ANSI_COLOR_RESET);
}

void getAdminCredentials(char* username, char* password) {
    printf(ANSI_COLOR_CYAN "Enter username: " ANSI_COLOR_RESET);
    scanf("%49s", username);
    while(getchar() != '\n');

    printf(ANSI_COLOR_CYAN "Enter password: " ANSI_COLOR_RESET);
    int i = 0;
    char ch;
    while (1) {
        ch = _getch();

        if (ch == '\r' || ch == '\n') {
            password[i] = '\0';
            break;
        } else if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (i < 49) {
            password[i++] = ch;
            printf(ANSI_COLOR_YELLOW "*" ANSI_COLOR_RESET);
        }
    }
    printf("\n");
}

Question getQuestionDetailsFromAdmin() {
    Question q;
    printf(ANSI_COLOR_CYAN "Enter Question ID: " ANSI_COLOR_RESET);
    scanf("%d", &q.id);
    getchar();

    printf(ANSI_COLOR_CYAN "Enter Question Text: " ANSI_COLOR_RESET);
    fgets(q.questionText, 512, stdin);

    for (int i = 0; i < 4; i++) {
        printf(ANSI_COLOR_CYAN "Enter Option %d: " ANSI_COLOR_RESET, i + 1);
        fgets(q.options[i], 100, stdin);
    }

    printf(ANSI_COLOR_CYAN "Enter Correct Option (1-4): " ANSI_COLOR_RESET);
    scanf("%d", &q.correctOption);
    getchar();

    printf(ANSI_COLOR_CYAN "Enter Category: " ANSI_COLOR_RESET);
    fgets(q.category, 50, stdin);

    printf(ANSI_COLOR_CYAN "Enter Difficulty (0-Easy, 1-Medium, 2-Hard): " ANSI_COLOR_RESET);
    scanf("%d", &q.difficulty);
    getchar();

    q.questionText[strcspn(q.questionText, "\n")] = 0;
    for(int i=0; i<4; i++) q.options[i][strcspn(q.options[i], "\n")] = 0;
    q.category[strcspn(q.category, "\n")] = 0;

    return q;
}

int getQuestionIdForDeletion() {
    int id;
    printf(ANSI_COLOR_CYAN "Enter the ID of the question to delete: " ANSI_COLOR_RESET);
    scanf("%d", &id);
    return id;
}

int getFilterChoiceForViewing() {
    int choice;
    printf("\nHow would you like to view questions?\n");
    printf("1. By Difficulty\n");
    printf("2. By Category\n");
    printf("3. View All\n");
    printf(ANSI_COLOR_CYAN "Enter your choice: " ANSI_COLOR_RESET);
    scanf("%d", &choice);
    while(getchar() != '\n');
    return choice;
}

int getDifficultyLevelFromAdmin() {
    int diff;
    printf(ANSI_COLOR_CYAN "Enter Difficulty (0-Easy, 1-Medium, 2-Hard): " ANSI_COLOR_RESET);
    scanf("%d", &diff);
    while(getchar() != '\n');
    return diff;
}

void getCategoryFromAdmin(char* category) {
    printf(ANSI_COLOR_CYAN "Enter category to filter by: " ANSI_COLOR_RESET);
    fgets(category, 50, stdin);
    category[strcspn(category, "\n")] = 0;
}

void displayFilteredQuestions(int difficulty, const char* category) {
    Question* lists[] = {easyQuestions, mediumQuestions, hardQuestions};
    int questionsFound = 0;
    int shouldExit = 0;
    for (int i = 0; i < 3; i++) {
        Question* current = lists[i];
        while (current != NULL) {
            int difficultyMatch = (difficulty == -1 || current->difficulty == difficulty);
            int categoryMatch = (strlen(category) == 0 || strcasecmp(current->category, category) == 0);

            if (difficultyMatch && categoryMatch) {
                questionsFound++;
                printf("\n-------------------------------------\n");
                printf(ANSI_COLOR_YELLOW "ID: %d | Difficulty: %d | Category: %s\n" ANSI_COLOR_RESET, current->id, current->difficulty, current->category);
                printf(ANSI_COLOR_BOLD_WHITE "Q: %s\n" ANSI_COLOR_RESET, current->questionText);
                for (int j = 0; j < 4; j++) {
                    printf("  %d. %s %s\n", j + 1, current->options[j], (j + 1 == current->correctOption) ? ANSI_COLOR_GREEN "<- Correct" ANSI_COLOR_RESET : "");
                }

                printf("\n\nPress 'e' to exit, or any other key to see the next question...");
                char choice = _getch();
                printf("\r                                                                  \r");

                if (choice == 'e' || choice == 'E') {
                    shouldExit = 1;
                    break;
                }
            }
            current = current->next;
        }
        if (shouldExit) {
            break;
        }
    }

    if (questionsFound == 0) {
        printf(ANSI_COLOR_RED "\nNo questions found matching the criteria.\n" ANSI_COLOR_RESET);
    }
}

void displayLeaderboard(Contestant* contestants, int count) {
    printf("\n\t\t\t" ANSI_COLOR_BLUE "********************************" ANSI_COLOR_RESET "\n");
    printf("\t\t\t" ANSI_COLOR_BLUE "*        KBC LEADERBOARD       *" ANSI_COLOR_RESET "\n");
    printf("\t\t\t" ANSI_COLOR_BLUE "********************************" ANSI_COLOR_RESET "\n\n");

    printf(ANSI_COLOR_BOLD_WHITE "----------------------------------------------------------------------------------------------------\n");
    printf("| %-4s | %-10s | %-30s | %-5s | %-10s | %-7s | %-12s |\n", "Rank", "ID", "Name", "Age", "Gender", "Qns Ans", "Prize Won");
    printf("----------------------------------------------------------------------------------------------------\n" ANSI_COLOR_RESET);
    for (int i = 0; i < count; i++) {
        printf("| %-4d | %-10d | %-30s | %-5d | %-10s | %-7d | Rs. %-8ld |\n",
               i + 1, contestants[i].id, contestants[i].name, contestants[i].age, contestants[i].gender, contestants[i].questionsAnswered, contestants[i].prizeWon);
    }
    printf(ANSI_COLOR_BOLD_WHITE "----------------------------------------------------------------------------------------------------\n" ANSI_COLOR_RESET);
}

Contestant getContestantDetails() {
    Contestant p;
    printf("\n--- " ANSI_COLOR_YELLOW "Contestant Registration" ANSI_COLOR_RESET " ---\n");
    printf(ANSI_COLOR_CYAN "Enter your name: " ANSI_COLOR_RESET);
    fgets(p.name, 100, stdin);
    p.name[strcspn(p.name, "\n")] = 0;

    printf(ANSI_COLOR_CYAN "Enter your age: " ANSI_COLOR_RESET);
    scanf("%d", &p.age);
    while(getchar() != '\n');

    printf(ANSI_COLOR_CYAN "Enter your gender: " ANSI_COLOR_RESET);
    fgets(p.gender, 10, stdin);
    p.gender[strcspn(p.gender, "\n")] = 0;

    p.id = time(NULL);
    p.questionsAnswered = 0;
    p.lifelinesUsed = 0;
    p.prizeWon = 0;

    return p;
}

void displayRules() {
    printf("\n--- " ANSI_COLOR_YELLOW "KBC Rules" ANSI_COLOR_RESET " ---\n");
    printf("1. There are 10 questions in total.\n");
    printf("2. Questions 1-3 (Easy) have a 30-second time limit.\n");
    printf("3. Questions 4-6 (Medium) have a 60-second time limit.\n");
    printf("4. Questions 7-10 (Hard) have no time limit.\n");
    printf("5. Answering incorrectly will end the game.\n");
    printf("6. You have three lifelines: 50-50, Flip the Question, Audience Poll.\n\n");
    printf(ANSI_COLOR_MAGENTA "Toh chaliye... shuru karte hain aaj ka yeh gyaan, himmat aur kismet ka safar.\n" ANSI_COLOR_RESET);
}

void displayQuizQuestion(Question q, int questionNumber, long prizeMoney) {
    printf(ANSI_COLOR_YELLOW "\n====================================\n");
    printf("Question %d for Rs. %ld\n", questionNumber, prizeMoney);
    printf("------------------------------------\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_BOLD_WHITE "Q: %s\n" ANSI_COLOR_RESET, q.questionText);
    printf(ANSI_COLOR_YELLOW "------------------------------------\n" ANSI_COLOR_RESET);
    for (int i = 0; i < 4; i++) {
        printf("  %d. %s\n", i + 1, q.options[i]);
    }
    printf(ANSI_COLOR_YELLOW "====================================\n" ANSI_COLOR_RESET);
}

int getAnswerFromUser() {
    int choice;
    printf(ANSI_COLOR_CYAN "Enter your choice (1-4), 5 for lifeline, or 0 to quit: " ANSI_COLOR_RESET);
    scanf("%d", &choice);
    while(getchar() != '\n');
    return choice;
}

void displayLifelineMenu(const int lifelines[]) {
    printf(ANSI_COLOR_MAGENTA "\nAgar man mein halka sa bhi sandeh hai... lifeline toh aapke saath hai hi." ANSI_COLOR_RESET);
    printf("\n--- " ANSI_COLOR_YELLOW "Available Lifelines" ANSI_COLOR_RESET " ---\n");
    if (lifelines[0]) printf("1. 50-50\n");
    if (lifelines[1]) printf("2. Flip the Question\n");
    if (lifelines[2]) printf("3. Audience Poll\n");
    printf("0. Cancel\n");
    printf(ANSI_COLOR_CYAN "Choose a lifeline: " ANSI_COLOR_RESET);
}

void displayAudiencePoll(const int percentages[]) {
    printf("\n--- " ANSI_COLOR_YELLOW "Audience Poll Results" ANSI_COLOR_RESET " ---\n");
    char options[] = {'A', 'B', 'C', 'D'};
    for (int i = 0; i < 4; i++) {
        printf("%c: %d%% ", options[i], percentages[i]);
        for (int j = 0; j < percentages[i] / 2; j++) {
            printf("#");
        }
        printf("\n");
    }
    printf("-----------------------------\n" ANSI_COLOR_RESET);
}

void displayAnswerResult(int isCorrect) {
    if (isCorrect) printf(ANSI_COLOR_GREEN "\n\t>>> Correct Answer! <<<\nBilkul sahi jawaab! Bahut-bahut badhaai ho aapko.\n" ANSI_COLOR_RESET);
    else printf(ANSI_COLOR_RED "\n\t>>> Oh no, that was the wrong answer! <<<\n" ANSI_COLOR_RESET);
}

void displayGameOver(Contestant player) {
    printf(ANSI_COLOR_MAGENTA "\nKoi baat nahi... har galti ek seekh hoti hai. Aapka safar abhi baaki hai." ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "\n\t\t--- GAME OVER ---\n" ANSI_COLOR_RESET);
    printf("\tCongratulations, %s!\n", player.name);
    printf("You answered %d questions correctly and won " ANSI_COLOR_GREEN "Rs. %ld!\n" ANSI_COLOR_RESET, player.questionsAnswered, player.prizeWon);
    printf("Press Enter to return to the main menu...");
    getchar();
}
