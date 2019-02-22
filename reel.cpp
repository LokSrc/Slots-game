/*
#####################################################################
# TIE-02207 Programming 2: Basics, F2018                            #
# Project4: Slots                                                   #
# Program description: Implements a virtual slots game.             #
# File: reel.cpp                                                    #
# Description: Draws random fruits for labels to display.           #
# Notes: * This is an exercise program.                             #
#        * Students need to modify this file.                       #
# Student: Marius Niemenmaa, marius.niemenmaa@tuni.fi               #
#####################################################################
*/

#include "reel.hh"
#include <QDebug>
#include <unistd.h>
#include <thread>

Reel::Reel(const std::vector<QLabel*> labels,
           const QPushButton* lockButton,
           const Fruits* fruits,
           std::shared_ptr<std::default_random_engine> rng,
           QLabel* lockLabel):
           labels_(labels),
           lockButton_(lockButton),
           fruits_(fruits),
           rng_(rng),
           lockLabel_(lockLabel),
           locked_(false),
           dist_(1, rngValue()),
           spinDist_(6, 20),
           reelFruits_(3) {

    // Connect lockbutton to reel
    connect(lockButton_, &QPushButton::clicked, this, &Reel::lockButtonClicked);

    // Initialize fruits on reel by randomizing them.
    int loopIndex = 0;
    for (QLabel* label: labels_) {
        std::string myFruit = randomFruit();
        label->setPixmap(fruits_->at(myFruit).first.scaled(80, 80, Qt::IgnoreAspectRatio, Qt::FastTransformation));
        reelFruits_.at(loopIndex) = myFruit;
        loopIndex++;
    }
}

void Reel::lockButtonClicked()
{
    if (!locked_) {
        locked_ = true;
        lockLabel_->setText("Locked!");
    } else {
        locked_ = false;
        lockLabel_->setText("");
    }
}

int Reel::rngValue() {
    int value = 0;
    for (std::pair<std::string, std::pair<QPixmap, int>>  fruit: *fruits_) {
        value += fruit.second.second;
    }
    return value;
}

std::string Reel::randomFruit() {

    // Get random value (int) from correct distribution
    int fruitValue = dist_(*rng_);
    int currentValue = 0;
    std::string currentFruit = "";

    // Select correct fruit according to randomized value
    for (std::pair<std::string, std::pair<QPixmap, int>>  fruit: *fruits_) {
        if (currentValue >= fruitValue) {
            return currentFruit;
        }
        currentValue += fruit.second.second;
        currentFruit = fruit.first;
    }
    return currentFruit;
}

void Reel::spinReel()
{
    if (locked_) {
        emit stopped(reelFruits_[1]);
        return;
    }

    // Randomize animation length and do animation
    for (int i = spinDist_(*rng_); i > 0; i--) {
        reelFruits_[2] = reelFruits_[1];
        reelFruits_[1] = reelFruits_[0];
        reelFruits_[0] = randomFruit();
        int index = 0;
        for (QLabel* label: labels_) {
            label->setPixmap(fruits_->at(reelFruits_[index]).first.scaled(80, 80, Qt::IgnoreAspectRatio, Qt::FastTransformation));
            index++;
        }
        // We need to sleep thread and update labels to achieve animation
        emit updateUI();
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }

    emit stopped(reelFruits_[1]);
}

bool Reel::isLocked()
{
    return locked_;
}
