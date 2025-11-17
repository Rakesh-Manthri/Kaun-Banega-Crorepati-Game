#ifndef VIEW_H
#define VIEW_H

#include "model.h"

void displayMainMenu();
int getMenuChoice();

void displayAdminMenu();
void getAdminCredentials(char* username, char* password);
Question getQuestionDetailsFromAdmin();
int getQuestionIdForDeletion();
int getFilterChoiceForViewing();
int getDifficultyLevelFromAdmin();
void getCategoryFromAdmin(char* category);
void displayFilteredQuestions(int difficulty, const char* category);
void displayLeaderboard(Contestant* contestants, int count);

Contestant getContestantDetails();
void displayRules();
void displayQuizQuestion(Question q, int questionNumber, long prizeMoney);
int getAnswerFromUser();
void displayLifelineMenu(const int lifelines[]);
void displayAudiencePoll(const int percentages[]);
void displayAnswerResult(int isCorrect);
void displayGameOver(Contestant player);


#endif