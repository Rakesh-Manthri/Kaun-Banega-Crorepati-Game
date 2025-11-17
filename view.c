#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include "view.h"
#include "model.h"

void displayMainMenu() {
    printf("\n=========================\n");
    printf("    WELCOME TO KBC\n");
    printf("=========================\n");
    printf("1. Admin Login\n");
    printf("2. Play as Contestant\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
}

int getMenuChoice() {
    int choice = 0;
    scanf("%d", &choice);
    while(getchar() != '\n');
    return choice;
}

void displayAdminMenu() {
    printf("\n--- Admin Panel ---\n");
    printf("1. View Questions\n");
    printf("2. Add Question\n");
    printf("3. Delete Question\n");
    printf("4. View All Participants List\n");
    printf("5. View Top Participants\n");
    printf("6. Analysis of Participants\n");
    printf("7. Logout\n");
    printf("Enter your choice: ");
}

void getAdminCredentials(char* username, char* password) {
    printf("Enter username: ");
    scanf("%49s", username);
    while(getchar() != '\n');

    printf("Enter password: ");
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
            printf("*");
        }
    }
    printf("\n");
}

Question getQuestionDetailsFromAdmin() {
    Question q;
    printf("Enter Question ID: ");
    scanf("%d", &q.id);
    getchar();

    printf("Enter Question Text: ");
    fgets(q.questionText, 512, stdin);

    for (int i = 0; i < 4; i++) {
        printf("Enter Option %d: ", i + 1);
        fgets(q.options[i], 100, stdin);
    }

    printf("Enter Correct Option (1-4): ");
    scanf("%d", &q.correctOption);
    getchar();

    printf("Enter Category: ");
    fgets(q.category, 50, stdin);

    printf("Enter Difficulty (0-Easy, 1-Medium, 2-Hard): ");
    scanf("%d", &q.difficulty);
    getchar();

    q.questionText[strcspn(q.questionText, "\n")] = 0;
    for(int i=0; i<4; i++) q.options[i][strcspn(q.options[i], "\n")] = 0;
    q.category[strcspn(q.category, "\n")] = 0;

    return q;
}

int getQuestionIdForDeletion() {
    int id;
    printf("Enter the ID of the question to delete: ");
    scanf("%d", &id);
    return id;
}

int getFilterChoiceForViewing() {
    int choice;
    printf("\nHow would you like to view questions?\n");
    printf("1. By Difficulty\n");
    printf("2. By Category\n");
    printf("3. View All\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    while(getchar() != '\n');
    return choice;
}

int getDifficultyLevelFromAdmin() {
    int diff;
    printf("Enter Difficulty (0-Easy, 1-Medium, 2-Hard): ");
    scanf("%d", &diff);
    while(getchar() != '\n');
    return diff;
}

void getCategoryFromAdmin(char* category) {
    printf("Enter category to filter by: ");
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
                printf("ID: %d | Difficulty: %d | Category: %s\n", current->id, current->difficulty, current->category);
                printf("Q: %s\n", current->questionText);
                for (int j = 0; j < 4; j++) {
                    printf("  %d. %s %s\n", j + 1, current->options[j], (j + 1 == current->correctOption) ? "<- Correct" : "");
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
        printf("\nNo questions found matching the criteria.\n");
    }
}

void displayAllParticipants(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("\nNo participant data found.\n");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

Contestant getContestantDetails() {
    Contestant p;
    printf("\n--- Contestant Registration ---\n");
    printf("Enter your name: ");
    fgets(p.name, 100, stdin);
    p.name[strcspn(p.name, "\n")] = 0;

    printf("Enter your age: ");
    scanf("%d", &p.age);
    while(getchar() != '\n');

    printf("Enter your gender: ");
    fgets(p.gender, 10, stdin);
    p.gender[strcspn(p.gender, "\n")] = 0;

    p.id = time(NULL);
    p.questionsAnswered = 0;
    p.lifelinesUsed = 0;
    p.prizeWon = 0;

    return p;
}

void displayRules() {
    printf("\n--- KBC Rules ---\n");
    printf("1. There are 10 questions in total.\n");
    printf("2. Questions 1-3 (Easy) have a 30-second time limit.\n");
    printf("3. Questions 4-6 (Medium) have a 60-second time limit.\n");
    printf("4. Questions 7-10 (Hard) have no time limit.\n");
    printf("5. Answering incorrectly will end the game.\n");
    printf("6. You have three lifelines: 50-50, Flip the Question, Audience Poll (Not yet implemented).\n");
    printf("Good luck!\n");
}

void displayQuizQuestion(Question q, int questionNumber, long prizeMoney) {
    printf("\n====================================\n");
    printf("Question %d for Rs. %ld\n", questionNumber, prizeMoney);
    printf("------------------------------------\n");
    printf("Q: %s\n", q.questionText);
    printf("------------------------------------\n");
    for (int i = 0; i < 4; i++) {
        printf("  %d. %s\n", i + 1, q.options[i]);
    }
    printf("====================================\n");
}

int getAnswerFromUser() {
    int choice;
    printf("Enter your choice (1-4), 5 for lifeline, or 0 to quit: ");
    scanf("%d", &choice);
    while(getchar() != '\n');
    return choice;
}

void displayLifelineMenu(const int lifelines[]) {
    printf("\n--- Available Lifelines ---\n");
    if (lifelines[0]) printf("1. 50-50\n");
    if (lifelines[1]) printf("2. Flip the Question\n");
    if (lifelines[2]) printf("3. Audience Poll\n");
    printf("0. Cancel\n");
    printf("Choose a lifeline: ");
}

void displayAudiencePoll(const int percentages[]) {
    printf("\n--- Audience Poll Results ---\n");
    char options[] = {'A', 'B', 'C', 'D'};
    for (int i = 0; i < 4; i++) {
        printf("%c: %d%% ", options[i], percentages[i]);
        for (int j = 0; j < percentages[i] / 2; j++) {
            printf("#");
        }
        printf("\n");
    }
    printf("-----------------------------\n");
}

void displayAnswerResult(int isCorrect) {
    if (isCorrect) printf("\n>>> Correct Answer! <<<\n");
    else printf("\n>>> Oh no, that was the wrong answer! <<<\n");
}

void displayGameOver(Contestant player) {
    printf("\n--- GAME OVER ---\n");
    printf("Congratulations, %s!\n", player.name);
    printf("You answered %d questions correctly and won Rs. %ld!\n", player.questionsAnswered, player.prizeWon);
    printf("Press Enter to return to the main menu...");
    getchar();
}
