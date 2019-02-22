/*
#####################################################################
# TIE-02207 Programming 2: Basics, F2018                            #
# Project4: Slots                                                   #
# Program description: Implements a virtual slots game.             #
# File: reelhandler.cpp                                             #
# Description: Makes QThread for a Reel                             #
# Notes: This is an exercise program.                               #
# Student: Marius Niemenmaa, marius.niemenmaa@tuni.fi               #
#####################################################################
*/

#include "reelhandler.hh"

reelHandler::reelHandler(Reel* reel) : reel_(reel)
{

}

void reelHandler::run() {
    reel_->spinReel();
}
