/*
#####################################################################
# TIE-02207 Programming 2: Basics, F2018                            #
# Project4: Slots                                                   #
# Program description: Implements a virtual slots game.             #
# File: main_window.cpp                                             #
# Description: Implements a UI for the game to be playable.         #
# Notes: * This is an exercise program.                             #
#        * Students need to modify this file.                       #
# Student: Marius Niemenmaa, marius.niemenmaa@tuni.fi               #
#####################################################################
*/

#include "main_window.hh"
#include <QPixmap>
#include <QString>
#include <map>
#include <memory>
#include <random>
#include <utility>
#include <QDebug>
#include <string>


// To avoid polluting the global namespace with all kinds of
// names, free functions and global variables / constants are
// usually placed inside a namespace. As we don't have a named
// namespace here, we'll use the anonymous namespace instead.
namespace {

/// \brief Get the fruit symbols and their frequencies.
///
/// \return A map of image names and corresponding images and frequencies.
///
Fruits loadFruits() {
    // Set frequencies for each symbol.
    // * You can freely play around with the values below.
    // * Values are relative to each other, higher == more common.
    // * Names of these fruits (and berries too) have to actually match
    //   images found on the disk. The images must be in the PNG format.
    const std::map<std::string, int> frequencies{
        {"cherries", 56},
        {"strawberry", 50},
        {"orange", 46},
        {"pear", 39},
        {"apple", 35},
        {"bananas", 30},
        {"tomato", 24},
        {"grapes", 15},
        {"eggplant", 5}
    };

    // Define where the images can be found and what kind of images they are.
    // * Here we assume that the build directory is next to Slots directory.
    const std::string directory("../slots/Resources/");
    const std::string extension(".png");

    // Load the images into QPixmaps and bundle them with their frequencies.
    Fruits symbols;
    for (const std::pair<std::string, int>& frequency : frequencies) {
        const std::string filename = directory + frequency.first + extension;
        const QPixmap image(QString::fromStdString(filename));
        symbols[frequency.first] = std::make_pair(image, frequency.second);
    }

    return symbols;
}

}  // namespace


MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent),
    reels_({}),
    reelFruits_({}),
    DISABLE(false),
    ENABLE(true),
    coins_(10),
    maxWin_(0),
    coinsUsed_(10),
    roundsPlayed_(0),
    threads_({}),
    fruits_(loadFruits()) {

    qRegisterMetaType<std::string>();
    ui_.setupUi(this);
    initUi();
}

MainWindow::~MainWindow()
{
    for (QThread* thread: threads_) {
        delete thread;
    }

    for(Reel* reel: reels_) {
        delete reel;
    }
}

void MainWindow::reelStopped(const std::string& middle_sym) {
    reelFruits_.push_back(middle_sym);

    // To make sure UI is up-to-date
    this->update();
    if (reelFruits_.size() >= 3) {

        // Make buttons enabled again
        buttonState(ENABLE);
        // Check if user has won and amount won. (More about amounts in "instructions.txt")
        if (reelFruits_[1].compare(reelFruits_[2]) == 0 && reelFruits_[1].compare(reelFruits_[0]) == 0) {
            if (reelFruits_[0].compare("cherries") == 0) {
                win(1);
            } else if (reelFruits_[0].compare("strawberry") == 0) {
                win(2);
            } else if (reelFruits_[0].compare("orange") == 0) {
                win(3);
            } else if (reelFruits_[0].compare("pear") == 0) {
                win(4);
            } else if (reelFruits_[0].compare("apple") == 0) {
                win(4);
            } else if (reelFruits_[0].compare("bananas") == 0) {
                win(5);
            } else if (reelFruits_[0].compare("tomato") == 0) {
                win(6);
            } else if (reelFruits_[0].compare("grapes") == 0) {
                win(10);
            } else if (reelFruits_[0].compare("eggplant") == 0) {
                win(50);
            }
            ui_.moneyLabel->setText("Coins: " + QString::number(coins_));
        } else {
            ui_.winLabel->setText("Better luck next time!");
        }

        // Check game status (is game over or not) and tell amount won/lost if game is over.
        if(coins_ <= 0 || roundsPlayed_ >= 25) {
            ui_.winLabel->setText("Game over!");
            if (coins_ == coinsUsed_) {
                ui_.moneyLabel->setText("Did not lose or win any money!");
            } else if (coins_ > coinsUsed_) {
                ui_.moneyLabel->setText("You won: " + QString::number(coins_ - coinsUsed_) + " coins!");
            } else {
                ui_.moneyLabel->setText("You lost: " + QString::number(coinsUsed_ - coins_) + " coins!");
            }
            buttonState(DISABLE);
        }
    }

}

void MainWindow::initUi() {
    // Initialize and display current funds etc.

    // Set lock buttons to desired starting state.

    // Create one random number generator for all Reels to use.
    // * The seed value is obtained via the chrono library and
    //   represents the number of seconds since 1.1.1970.
    // * If you need to debug some pesky error, you can temporarily
    //   replace the seed with some literal. See Minesweeper or
    //   Waterdropgame versions 1 and 2.
    const auto time = std::chrono::high_resolution_clock::now();
    const auto seed = time.time_since_epoch().count();
    auto rng = std::make_shared<std::default_random_engine>(seed);

    // Simple database to help us initialize reels. Holds lockbuttons and locklabels
    std::map<int, std::pair<QPushButton*, QLabel*>> lockTools = {{0, {ui_.lockButton0, ui_.lockLabel0}},
                                               {1, {ui_.lockButton1, ui_.lockLabel1}},
                                               {2, {ui_.lockButton2, ui_.lockLabel2}}};

    // Simple database to help us initialize reels. Holds reellabels.
    std::map<int, std::vector<QLabel*>> labels = {{0, {ui_.reel0top, ui_.reel0mid, ui_.reel0bot}},
                                                 {1, {ui_.reel1top, ui_.reel1mid, ui_.reel1bot}},
                                                 {2, {ui_.reel2top, ui_.reel2mid, ui_.reel2bot}}};

    ui_.moneyLabel->setText("Coins: " + QString::number(coins_));

    /* Lets create 3 reels with correct labels and buttons.
     * And also threads for them to achieve simultaneous animations. */
    for (int i = 0; i < 3; i++) {
        Reel* reel = new Reel(labels.at(i), lockTools.at(i).first, &fruits_, rng, lockTools.at(i).second);
        reelHandler* thread = new reelHandler(reel);
        connect(reel, &Reel::stopped, this, &MainWindow::reelStopped);
        connect(reel, &Reel::updateUI, this, &MainWindow::updateUI);
        reels_.push_back(reel);
        threads_.push_back(thread);
    }

}

void MainWindow::buttonState(bool state)
{
    ui_.lockButton0->setEnabled(state);
    ui_.lockButton1->setEnabled(state);
    ui_.lockButton2->setEnabled(state);
    ui_.spinButton->setEnabled(state);
    ui_.addMoney->setEnabled(state);
}

void MainWindow::win(int amount)
{
    coins_ += amount;
    if (amount > maxWin_) {
        maxWin_ = amount;
        ui_.maxWinLabel->setText("Biggest win: " + QString::number(amount));
    }
    if (amount > 1) {
        ui_.winLabel->setText("Won " + QString::number(amount) + " coins!");
    } else {
        ui_.winLabel->setText("Won 1 coin!");
    }

}

void MainWindow::on_spinButton_clicked()
{
    ui_.winLabel->setText("");
    reelFruits_ = {};

    // If all reels are locked we dont want to spin reels.
    if (reels_[0]->isLocked() && reels_[1]->isLocked() && reels_[2]->isLocked()) {
        ui_.winLabel->setText("Unlock a reel to spin.");
        return;
    }

    // Set buttons disabled during animations
    buttonState(DISABLE);
    coins_--;
    roundsPlayed_++;
    ui_.moneyLabel->setText("Coins: " + QString::number(coins_));
    // Spin all reels using threads.
    for (QThread* thread: threads_) {
        thread->start();
    }
}

void MainWindow::on_addMoney_clicked()
{
    coins_ += 1;
    coinsUsed_ += 1;
    ui_.moneyLabel->setText("Coins: " + QString::number(coins_));
}

void MainWindow::updateUI()
{
    this->update();
}
