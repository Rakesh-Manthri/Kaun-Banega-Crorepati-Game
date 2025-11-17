// main.c

#include <stdio.h>
#include "view.h"       // For displaying menus
#include "controller.h" // For admin and contestant logic
#include "model.h"      // For loading data and cleanup

int main() {
    // Load all questions from the file into our linked lists at startup.
    if (!loadQuestionsFromFile("questions.txt")) {
        fprintf(stderr, "Error: Could not load questions. Exiting.\n");
        return 1;
    }

    int choice;
    while (1) {
        displayMainMenu(); // A function from view.c
        choice = getMenuChoice(); // A function from view.c

        switch (choice) {
            case 1:
                adminController(); // Switch to the admin module
                break;
            case 2:
                contestantController(); // Switch to the contestant module
                break;
            case 3:
                printf("\nThank you for using the KBC system!\n");
                // Free all dynamically allocated memory for the question linked lists
                cleanupQuestions(); // A function from model.c
                return 0; // Exit the program
            default:
                printf("\nInvalid choice. Please enter a number between 1 and 3.\n");
                break;
        }
    }

    return 0;
}
