#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "view.h"
#include "controller.h"
#include "model.h"

// ANSI Color Codes
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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
                printf(ANSI_COLOR_MAGENTA "\nZindagi ek khel hai, aur seekhna uska sabse bada inaam.\nAap sabka pyaar aur saath bana rahe.\nPhirse milte hain. Namaskar\n" ANSI_COLOR_RESET);
                cleanupQuestions();
                return 0;
            default:
                printf("\nInvalid choice. Please enter a number between 1 and 3.\n");
                break;
        }
    }
    return 0;
}
