/*
#####################################################################
# TIE-02207 Programming 2: Basics, F2018                            #
# Project4: Slots                                                   #
# Program description: Implements a virtual slots game.             #
# File: main_window.h                                               #
# Description: Implements a UI for the game to be playable.         #
# Notes: * This is an exercise program.                             #
#        * Students need to modify this file.                       #
# Student: Marius Niemenmaa, marius.niemenmaa@tuni.fi               #
#####################################################################
*/

#ifndef SLOTS_MAINWINDOW_H
#define SLOTS_MAINWINDOW_H

#include "reel.hh"
#include "ui_main_window.h"
#include "reelhandler.hh"
#include <QMainWindow>
#include <string>

Q_DECLARE_METATYPE(std::string)

/// \class MainWindow
/// \brief Implements the main window which is used to interact with the game.
///
/// MainWindow owns all the widgets used to display the game to the player
/// and to play the game. MainWindow contains the whole game, no other windows
/// are used.
///
class MainWindow: public QMainWindow {
    Q_OBJECT


public:

    /// \brief Construct a new MainWindow.
    ///
    /// \param[in] parent Points to this MainWindow's parent widget.
    ///
    explicit MainWindow(QWidget* parent = nullptr);

    /// \brief Deconstruct MainWindow and delete pointers
    ///
    ~MainWindow();


private slots:

    /// \brief Check if the round is over and prepare for the next round.
    ///
    /// Should get called when a Reel has decided on which fruits to display.
    ///
    /// \param[in] middle_sym Name of the fruit symbol on the middle lane.
    ///
    void reelStopped(const std::string& middle_sym);

    /// \brief Spin all reels
    ///
    void on_spinButton_clicked();

    /// \brief Adds one coin to saldo (coins_).
    ///
    void on_addMoney_clicked();

    /// \brief Updates UI
    ///
    /// Should get called when Reels fruits have been changed
    ///
    void updateUI();

private:

    /// \brief Initialize the Reels, buttons, etc. so the game can be played.
    ///
    void initUi();

    /// \brief Change state of all UI buttons.
    ///
    /// \param[in] state State we want buttons to be.
    void buttonState(bool state);

    /// \brief Process win of the user
    ///
    /// \param[in] amount Amount of coins won.
    void win(int amount);

    std::vector<Reel*> reels_;
    std::vector<std::string> reelFruits_;  ///< Holds name of fruit in middle slot of each reel

    // Constant variables to make code look simpler
    const bool DISABLE;
    const bool ENABLE;

    int coins_;
    int maxWin_;
    int coinsUsed_;
    int roundsPlayed_;

    std::vector<QThread*> threads_;   ///< Holds pointers to threads used for simultaneous animation
    const Fruits fruits_;       ///< The game symbols and their rarities.
    Ui::MainWindowClass ui_;    ///< (Black) Qt magic handling the UI.

};  // class MainWindow


#endif  // SLOTS_MAINWINDOW_H
