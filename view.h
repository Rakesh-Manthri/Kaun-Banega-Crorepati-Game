// view.h

#ifndef VIEW_H
#define VIEW_H

#include "model.h" // To know about the Question struct

// --- Main Menu ---
void displayMainMenu();
int getMenuChoice();

// --- Admin Views ---
void displayAdminMenu();
void getAdminCredentials(char* username, char* password);
Question getQuestionDetailsFromAdmin();
int getQuestionIdForDeletion();
int getFilterChoiceForViewing();
int getDifficultyLevelFromAdmin();
void getCategoryFromAdmin(char* category);
void displayFilteredQuestions(int difficulty, const char* category);
void displayAllParticipants(const char* filename);

// --- Contestant Views ---
Contestant getContestantDetails();
void displayRules();
void displayQuizQuestion(Question q, int questionNumber, long prizeMoney);
int getAnswerFromUser();
void displayLifelineMenu(const int lifelines[]);
void displayAudiencePoll(const int percentages[]);
void displayAnswerResult(int isCorrect);
void displayGameOver(Contestant player);


#endif // VIEW_H