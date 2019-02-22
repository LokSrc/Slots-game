/*
#####################################################################
# TIE-02207 Programming 2: Basics, F2018                            #
# Project4: Slots                                                   #
# Program description: Implements a virtual slots game.             #
# File: reel.h                                                      #
# Description: Draws random fruits for labels to display.           #
# Notes: * This is an exercise program.                             #
#        * Students need to modify this file.                       #
# Student: Marius Niemenmaa, marius.niemenmaa@tuni.fi               #
#####################################################################
*/

#ifndef SLOTS_REEL_H
#define SLOTS_REEL_H

#include <QLabel>
#include <QObject>
#include <QPixmap>
#include <QPushButton>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

// Fruits is an alias for a map which contains a fruit's name and
// corresponding image and likelyhood of appearance.
using Fruits = std::map<std::string, std::pair<QPixmap, int>>;


/// \class Reel
/// \brief Implements reels which are used to determine if the player has won.
///
/// Reel spins for a random duration and draws random fruits to display on its
/// labels. Reel signals MainWindow with the name of the middlemost fruit, but
/// an ordered (top-to-bottom) list of all fruits can also be queried.
///
class Reel: public QObject {
    Q_OBJECT

public:

    /// \brief Construct a new Reel.
    ///
    /// \param[in] labels Labels which should be used to display the symbols.
    /// \param[in] lock_button Button whose state determines if Reel is locked.
    /// \param[in] fruits Images and frequencies of each fruit.
    /// \param[in] rng Random number generator used to determine the symbols.
    ///
    Reel(const std::vector<QLabel*> labels,
         const QPushButton* lockButton,
         const Fruits* fruits,
         std::shared_ptr<std::default_random_engine> rng,
         QLabel* lockLabel);

    /// \brief Make current reel spin.
    ///
    void spinReel();

    /// \brief Return state of reel
    ///
    /// \return bool is reel locked
    bool isLocked();

private slots:
    /// \brief Change state of reel (lock)
    ///
    void lockButtonClicked();

signals:

    /// \brief Is emitted when the Reel stops.
    ///
    /// \param[in] Name of the middle symbol on the Reel.
    ///
    void stopped(const std::string& middle_sym);

    /// \brief Is emitted when labels change (during animation)
    ///
    void updateUI();

private:
    /// \brief Return sum of fruit frequencies
    ///
    /// \return int sum
    int rngValue();

    /// \brief Select random fruit according to probabilities
    ///
    /// \return std::string Name of selected fruit
    std::string randomFruit();


    const std::vector<QLabel*> labels_;
    const QPushButton* lockButton_;
    const Fruits* fruits_;

    // Random engine
    std::shared_ptr<std::default_random_engine> rng_;

    QLabel* lockLabel_;
    bool locked_;

    // Distributions for randomizing
    std::uniform_int_distribution<int> dist_;
    std::uniform_int_distribution<int> spinDist_;

    // Holds names of Fruits on reel
    std::vector<std::string> reelFruits_;
};  // class Reel


#endif  // SLOTS_REEL_H
