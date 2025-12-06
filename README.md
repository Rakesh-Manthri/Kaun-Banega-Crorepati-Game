# Kaun Banega Crorepati (KBC) Game in C

A comprehensive console-based implementation of the famous quiz show **"Kaun Banega Crorepati"**. This project utilizes the **Model-View-Controller (MVC)** architectural pattern to deliver a robust and interactive gaming experience, complete with lifelines, timers, and administrative controls.

## 📋 Table of Contents
- [About the Project](#about-the-project)
- [Key Features](#key-features)
- [Project Architecture (MVC)](#project-architecture-mvc)
- [File Structure](#file-structure)
- [Prerequisites](#prerequisites)
- [How to Compile and Run](#how-to-compile-and-run)
- [How to Play](#how-to-play)
- [Future Improvements](#future-improvements)

## 📖 About the Project
This application simulates the high-stakes environment of the KBC quiz show directly in the terminal. It features a persistent database of questions and high scores, managed via a file system. The game supports two distinct roles—**Administrator** (for managing content) and **Participant** (for playing the game)—ensuring a dynamic experience.

## ✨ Key Features
-   **MVC Design Pattern:** The code is structured into Model, View, and Controller to separate data handling, user interface, and game logic.
-   **⏱️ Countdown Timer:** Adds pressure to the gameplay! Participants must answer within a set time limit to proceed.
-   **🆘 50-50 Lifeline:** A strategic tool that removes two incorrect options, leaving the correct answer and one wrong answer.
-   **Lifelines:** The game includes multiple lifelines to help the participant: **50-50**, **Audience Poll**, and **Flip the Question**.
-   **Role-Based Access:**
    -   **Admin Mode:** Securely add new questions and view participant records.
    -   **Player Mode:** Experience the quiz with progressive difficulty and prize tracking.
-   **File Persistence:**
    -   Questions are loaded dynamically from `questions.txt`.
    -   Player winnings are saved to `participants.txt`.

## 🏗 Project Architecture (MVC)
The source code is organized to ensure modularity and ease of maintenance:

*   **Model (`model.c` / `model.h`):** The data layer. It handles file I/O operations (reading questions, saving scores) and manages the core data structures for the game state.
*   **View (`view.c` / `view.h`):** The presentation layer. It is responsible for all console output, including drawing the question interface, the timer, and the menu systems.
*   **Controller (`controller.c` / `controller.h`):** The logic layer. It processes user inputs, handles the countdown timer logic, implements the 50-50 lifeline, and coordinates between the Model and View.

## 📂 File Structure
```text
📦 Kaun-Banega-Crorepati-Game
 ┣ 📜 main.c              # Entry point; initializes the game loop
 ┣ 📜 controller.c        # Handles game rules, timer, and lifelines
 ┣ 📜 controller.h        # Header for controller functions
 ┣ 📜 model.c             # Manages file reading/writing (Questions/Scores)
 ┣ 📜 model.h             # Header for model functions
 ┣ 📜 view.c              # UI rendering and display logic
 ┣ 📜 view.h              # Header for view functions
 ┣ 📜 questions.txt       # Database of questions and answers
 ┣ 📜 participants.txt    # Log of past players and winnings
 ┗ 📜 README.md           # Project documentation
```

## ⚙️ Prerequisites
To run this project, you need a C compiler installed on your system.
-   **Compiler:** GCC (GNU Compiler Collection) is recommended.
-   **OS:** Compatible with Windows, Linux, and macOS.

## 🚀 How to Compile and Run

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/Rakesh-Manthri/Kaun-Banega-Crorepati-Game.git
    cd Kaun-Banega-Crorepati-Game
    ```

2.  **Compile the source files:**
    Run the following command in your terminal:
    ```bash
    gcc main.c controller.c model.c view.c -o kbc_game
    ```

3.  **Run the executable:**
    -   **On Windows:**
        ```bash
        kbc_game.exe
        ```
    -   **On Linux/Mac:**
        ```bash
        ./kbc_game
        ```

## 🎮 How to Play
1.  **Start the Game:** Run the executable to enter the main menu.
2.  **Select Role:**
    -   Choose **Participant** to start the quiz.
    -   Choose **Admin** to manage the question bank.
3.  **The Quiz:**
    -   A question is displayed along with 4 options (A, B, C, D).
    -   **Keep an eye on the Timer!** You must select an answer before time runs out.
4.  **Using the Lifeline:**
    -   If you are stuck, you can trigger the **50-50** or **Audience Poll** or **Flip the question LifeLines**.
    -   Each lifeline provides a unique advantage to help you answer correctly.
5.  **Winning:** Answer correctly to move up the money ladder. A wrong answer ends the game immediately.

## 🔮 Future Improvements
-   Add a "Phone a Friend" lifeline.
-   Implement background music and sound effects for correct/incorrect answers.
-   Create a graphical user interface (GUI) using libraries like SDL or GTK.
-   Encrypt the password protection for the Admin mode.

---
