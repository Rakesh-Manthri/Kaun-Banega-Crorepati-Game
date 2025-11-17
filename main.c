#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "view.h"
#include "controller.h"
#include "model.h"

int main() {
    if (!loadQuestionsFromFile("questions.txt")) {
        fprintf(stderr, "Error: Could not load questions. Exiting.\n");
        return 1;
    }

    srand(time(NULL));

    int choice;
    while (1) {
        displayMainMenu();
        choice = getMenuChoice();

        switch (choice) {
            case 1:
                adminController();
                break;
            case 2:
                contestantController();
                break;
            case 3:
                printf("\nThank you for using the KBC system!\n");
                cleanupQuestions();
                return 0;
            default:
                printf("\nInvalid choice. Please enter a number between 1 and 3.\n");
                break;
        }
    }

    return 0;
}
