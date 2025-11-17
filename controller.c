// controller.c

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "controller.h"
#include "view.h"
#include "model.h"

// --- Forward Declarations for Static Functions ---
static int performAdminLogin();
static void handleViewQuestions();
static void handleAddQuestion();
static void handleDeleteQuestion();
static void handleViewAllParticipants();
static void shuffleQuestionOptions(Question *q);
static void useFiftyFifty(Question *q);
static void useAudiencePoll(Question *q, int percentages[]);
static int handleLifeline(Question *currentQ, int lifelines[], Contestant *player, Stack* lifelineStack);


// --- Admin Login ---
// This function handles the admin authentication.
// For simplicity, credentials are hardcoded.
static int performAdminLogin() {
    char username[50];
    char password[50];
    
    getAdminCredentials(username, password); // View function to get input

    // Check against hardcoded credentials
    if (strcmp(username, "admin") == 0 && strcmp(password, "pass123") == 0) {
        printf("\nLogin Successful!\n");
        return 1; // Success
    } else {
        printf("\nInvalid credentials!\n");
        return 0; // Failure
    }
}

// --- Main Admin Controller ---
void adminController() {
    if (!performAdminLogin()) {
        return; // If login fails, return to the main menu
    }

    int choice;
    do {
        displayAdminMenu(); // View function
        choice = getMenuChoice(); // View function

        switch (choice) {
            case 1:
                // View Questions
                handleViewQuestions();
                break;
            case 2:
                // Add Question
                handleAddQuestion();
                break;
            case 3:
                // Delete Question
                handleDeleteQuestion();
                break;
            case 4:
                // View All Participants
                handleViewAllParticipants();
                break;
            case 5:
                // View Top Participants
                // (Implementation would go here)
                printf("\nFeature 'View Top Participants' is under construction.\n");
                break;
            case 6:
                // Analysis of Participants
                // (Implementation would go here)
                printf("\nFeature 'Analysis of Participants' is under construction.\n");
                break;
            case 7:
                printf("\nLogging out...\n");
                break; // This will exit the loop
            default:
                printf("\nInvalid choice. Please try again.\n");
                break;
        }
    } while (choice != 7); // 7 is Logout
}

static void handleAddQuestion() {
    Question newQ = getQuestionDetailsFromAdmin(); // View function to get all question data
    addQuestionToList(&newQ); // Model function to add to the correct linked list
    if (saveQuestionsToFile("questions.txt")) { // Model function to persist changes
        printf("\nQuestion added successfully!\n");
    } else {
        printf("\nError: Could not save the question to the file.\n");
    }
}

static void handleDeleteQuestion() {
    int id = getQuestionIdForDeletion(); // View function to ask for an ID
    if (deleteQuestionById(id)) { // Model function to remove from linked list
        if (saveQuestionsToFile("questions.txt")) { // Rewrite the file without the deleted question
            printf("\nQuestion with ID %d deleted successfully.\n", id);
        } else {
            printf("\nError: Could not update the questions file.\n");
        }
    } else {
        printf("\nQuestion with ID %d not found.\n", id);
    }
}

static void handleViewQuestions() {
    int filterChoice = getFilterChoiceForViewing(); // View function
    int difficulty = -1;
    char category[50] = "";

    if (filterChoice == 1) { // Filter by difficulty
        difficulty = getDifficultyLevelFromAdmin();
    }
    else if (filterChoice == 2) { // Filter by category
        getCategoryFromAdmin(category);
    }

    printf("\n--- Displaying Questions ---\n");
    displayFilteredQuestions(difficulty, category); // View function that iterates through Model data
}

static void handleViewAllParticipants() {
    printf("\n--- All Participants ---\n");
    displayAllParticipants("participants.txt"); // View function to read and print from file
}

static void shuffleQuestionOptions(Question *q) {
    // Store the original correct answer text to find it later
    char correctOptionText[100];
    strcpy(correctOptionText, q->options[q->correctOption - 1]);

    // Fisher-Yates shuffle algorithm
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);

        // Swap options[i] and options[j]
        char temp[100];
        strcpy(temp, q->options[i]);
        strcpy(q->options[i], q->options[j]);
        strcpy(q->options[j], temp);
    }

    // Find the new index of the correct answer and update it
    for (int i = 0; i < 4; i++) {
        if (strcmp(q->options[i], correctOptionText) == 0) {
            q->correctOption = i + 1;
            break;
        }
    }
}

/**
 * @brief Applies the 50-50 lifeline logic to a question.
 * It finds two incorrect options and replaces their text with empty strings.
 * @param q A pointer to the Question struct to be modified.
 */
static void useFiftyFifty(Question *q) {
    int removed_count = 0;
    while (removed_count < 2) {
        int rand_option = rand() % 4; // Get a random option index (0-3)

        // Check if the option is not the correct one and not already removed
        if ((rand_option + 1 != q->correctOption) && (strlen(q->options[rand_option]) > 0)) {
            strcpy(q->options[rand_option], ""); // "Remove" the option
            removed_count++;
        }
    }
}

/**
 * @brief Generates simulated audience poll results based on question difficulty.
 * @param q The question for which to generate the poll.
 * @param percentages An array to be filled with the poll percentages.
 */
static void useAudiencePoll(Question *q, int percentages[]) {
    for (int i = 0; i < 4; i++) percentages[i] = 0;

    int correct_idx = q->correctOption - 1;
    int remaining_percentage = 100;

    // 1. Assign a higher percentage to the correct answer based on difficulty
    if (q->difficulty == 0) { // Easy
        percentages[correct_idx] = 60 + (rand() % 21); // 60% to 80%
    } else if (q->difficulty == 1) { // Medium
        percentages[correct_idx] = 40 + (rand() % 21); // 40% to 60%
    } else { // Hard
        percentages[correct_idx] = 25 + (rand() % 26); // 25% to 50%
    }
    remaining_percentage -= percentages[correct_idx];

    // 2. Distribute the remaining percentage among the three incorrect options
    int incorrect_indices[3];
    int k = 0;
    for (int i = 0; i < 4; i++) {
        if (i != correct_idx) {
            incorrect_indices[k++] = i;
        }
    }

    if (remaining_percentage > 0) {
        // Distribute randomly among the three incorrect options
        int p1 = rand() % (remaining_percentage + 1);
        int p2 = rand() % (remaining_percentage - p1 + 1);
        int p3 = remaining_percentage - p1 - p2;

        percentages[incorrect_indices[0]] = p1;
        percentages[incorrect_indices[1]] = p2;
        percentages[incorrect_indices[2]] = p3;
    }
}

/**
 * @brief Handles the lifeline selection process for the contestant.
 */
static int handleLifeline(Question *currentQ, int lifelines[], Contestant *player, Stack* lifelineStack) {
    displayLifelineMenu(lifelines);
    int choice = getMenuChoice();
    int question_was_flipped = 0;
    
    switch (choice) {
        case 1: // 50-50
            if (lifelines[0]) {
                printf("\nUsing 50-50 lifeline...\n");
                useFiftyFifty(currentQ);
                lifelines[0] = 0; // Mark as used
                player->lifelinesUsed++;
            } else {
                printf("\nYou have already used the 50-50 lifeline!\n");
            }
            break;
        case 2: // Flip the Question
            if (lifelines[1]) {
                lifelines[1] = 0; // Mark as used immediately upon choosing
                printf("\nFlipping the question...\n");
                int found_replacement = 0;
                while (!isStackEmpty(lifelineStack)) {
                    Question replacement = peek(lifelineStack);
                    if (replacement.difficulty == currentQ->difficulty) {
                        *currentQ = pop(lifelineStack); // Pop and assign
                        player->lifelinesUsed++;
                        question_was_flipped = 1;
                        found_replacement = 1;
                        break; // Exit the while loop
                    } else {
                        pop(lifelineStack); // Pop and discard, check next one
                    }
                }
                if (!found_replacement) {
                     printf("\nSorry, no replacement question of the same difficulty was available. You have lost the lifeline.\n");
                }

            } else {
                printf("\nYou have already used the Flip the Question lifeline!\n");
            }
            break;
        case 3: // Audience Poll
            if (lifelines[2]) {
                printf("\nAsking the audience...\n");
                int poll_results[4];
                useAudiencePoll(currentQ, poll_results);
                displayAudiencePoll(poll_results);
                lifelines[2] = 0; // Mark as used
                player->lifelinesUsed++;
            } else {
                printf("\nYou have already used the Audience Poll lifeline!\n");
            }
            break;
        default:
            printf("\nReturning to the question.\n");
            break;
    }

    return question_was_flipped;
}

void contestantController() {
    // 1. Registration
    Contestant player = getContestantDetails();

    // 2. Display Rules
    displayRules();
    printf("\nPress Enter to start the quiz...");
    getchar();

    // Seed the random number generator once for the entire quiz session
    srand(time(NULL));

    // 3. Prepare Quiz Questions and load them into the queue
    QuizData* quizData = getQuizData();
    if (quizData == NULL) {
        printf("\nCould not start the quiz due to an insufficient number of questions. Please contact the admin.\n");
        return;
    }
    Queue* quizQueue = quizData->quizQueue;
    Stack* lifelineStack = quizData->lifelineStack;

    long prizeLevels[] = {1000, 5000, 10000, 40000, 160000, 640000, 1250000, 2500000, 10000000, 70000000};
    int lifelines[3] = {1, 1, 1}; // 0: 50-50, 1: Flip, 2: Poll
    int isPlaying = 1;
    long safePrize = 0;
    int questionNumber = 0;

    // 4. Start Quiz Loop by popping questions from the queue
    while (!isQueueEmpty(quizQueue)) {
        // Dequeue the next question
        Question currentQ = dequeue(quizQueue); // This is a copy
        long currentPrize = prizeLevels[questionNumber];
        questionNumber++;

        shuffleQuestionOptions(&currentQ); // Initial shuffle

        int answer_given = 0;
        while (!answer_given) {
            displayQuizQuestion(currentQ, questionNumber, currentPrize);

            // Timer Logic
            time_t startTime = time(NULL);
            int timeLimit = 0;
            if (currentQ.difficulty == 0) timeLimit = 30;
            if (currentQ.difficulty == 1) timeLimit = 60;

            int answer = getAnswerFromUser();
            time_t endTime = time(NULL);

            if (answer == 5) { // Lifeline choice
                if (handleLifeline(&currentQ, lifelines, &player, lifelineStack)) {
                    // Question was flipped, re-shuffle its options
                    shuffleQuestionOptions(&currentQ);
                }
                // Loop continues to re-display the (potentially new) question
            } else if (answer == 0) { // Quit choice
                printf("\nYou have chosen to quit the game.\n");
                isPlaying = 0;
                answer_given = 1;
            } else if (timeLimit > 0 && (endTime - startTime) > timeLimit) {
                printf("\nTime's up!\n");
                isPlaying = 0;
                answer_given = 1;
            } else if (answer >= 1 && answer <= 4) {
                if (answer == currentQ.correctOption) {
                    displayAnswerResult(1);
                    player.prizeWon = currentPrize;
                    player.questionsAnswered++;
                    // Update safe prize at transition levels
                    if (questionNumber == 3) { // End of Easy
                        safePrize = currentPrize;
                    } else if (questionNumber == 6) { // End of Medium
                        safePrize = currentPrize;
                    }
                } else {
                    displayAnswerResult(0);
                    player.prizeWon = safePrize; // Fall back to the last safe prize
                    isPlaying = 0;
                }
                answer_given = 1;
            }
            else {
                printf("\nInvalid input. Please choose an option from 1-4, or 5 for a lifeline.\n");
            }
        }

        if (!isPlaying) break;

        printf("Press Enter to proceed to the next question...");
        getchar();
    }

    // 5. Game Over and Save Results
    displayGameOver(player);
    saveContestantToFile("participants.txt", player);

    // Cleanup the queue memory
    free(quizQueue->arr);
    free(quizQueue);
    free(lifelineStack->a);
    free(lifelineStack);
    free(quizData);
}
