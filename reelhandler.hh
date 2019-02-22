/*
#####################################################################
# TIE-02207 Programming 2: Basics, F2018                            #
# Project4: Slots                                                   #
# Program description: Implements a virtual slots game.             #
# File: reelhandler.hh                                              #
# Description: Makes QThread for a Reel                             #
# Notes: This is an exercise program.                               #
# Student: Marius Niemenmaa, marius.niemenmaa@tuni.fi               #
#####################################################################
*/

#ifndef REELHANDLER_HH
#define REELHANDLER_HH

#include <main_window.hh>
#include <QThread>

class reelHandler : public QThread
{
    Q_OBJECT
public:
    /// \biref Construct a new reelHandler
    ///
    /// param[in] reel Points to reel this reelHandler is handling
    ///
    explicit reelHandler(Reel* reel);

    /// \brief Spin Reel of this thread
    ///
    void run();

private:
    Reel* reel_; ///< Store pointer to reel

signals:


};

#endif // REELHANDLER_HH
