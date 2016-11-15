#ifndef SIM_TIME_HPP
#define SIM_TIME_HPP

#include <chrono>

//----------------------------------------------------------------------------------------------------------------------
/// \file sim_time.hpp
/// \brief This file contains the sim_time class, useful for timing events.
/// \author Ben Hawkyard
/// \version 1.0
/// \date 11/04/16
/// Revision History :
/// This is an initial version used for the game.
/// \class sim_time
/// \brief Used to control time-stepping in-game. Also stores some values it probably needn't, since
/// they aren't strictly necessary for the class to function, but I found it useful to have them all in one
/// place.
//----------------------------------------------------------------------------------------------------------------------

class sim_time
{
public:
    //----------------------------------------------------------------------------------------------------------------------
    /// \brief ctor for the sim_time class.
    /// \param _fps Stores the fps at which we will update the game.
    //----------------------------------------------------------------------------------------------------------------------
    sim_time(float _fps);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Setter for m_start, also recalculates m_diff and m_time_since_creation.
    //----------------------------------------------------------------------------------------------------------------------
    void setStart();

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Setter for m_cur, also recalculates m_diff and m_time_since_creation.
    //----------------------------------------------------------------------------------------------------------------------
    void setCur();

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Getter for m_diff.
    //----------------------------------------------------------------------------------------------------------------------
    double getDiff() const {return m_diff;}

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Getter for m_time_since_creation.
    //----------------------------------------------------------------------------------------------------------------------
    double getTime() const {return m_time_since_creation;}

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Getter for m_sim_dt.
    //----------------------------------------------------------------------------------------------------------------------
    double getFrame() const {return m_sim_dt;}

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Getter for m_sim_accumulator.
    //----------------------------------------------------------------------------------------------------------------------
    double getAcc() const {return m_sim_accumulator;}

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Increments m_sim_accumulator by a given value.
    /// \param _incr Increment value
    //----------------------------------------------------------------------------------------------------------------------
    void incrAcc(float _incr) {m_sim_accumulator += _incr;}

private:
    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Makes the main type this class deals with less laborious to write.
    //----------------------------------------------------------------------------------------------------------------------
    typedef std::chrono::high_resolution_clock hr_clock;

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Stores the tickrate as a const double.
    //----------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
    const double m_tickRate = static_cast<const double>(CLOCKS_PER_SEC * 10000);
#endif
#ifndef _WIN32
    const double m_tickRate = static_cast<const double>(1000000000.0f);
#endif

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Tracks time since instantiation.
    //----------------------------------------------------------------------------------------------------------------------
    double m_time_since_creation;

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief The last value the timer was set to.
    //----------------------------------------------------------------------------------------------------------------------
    long int m_start;

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief The current timer value.
    //----------------------------------------------------------------------------------------------------------------------
    long int m_cur;

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Number of seconds between m_start and m_cur.
    //----------------------------------------------------------------------------------------------------------------------
    double m_diff;

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Used in the game loop to control update fps.
    //----------------------------------------------------------------------------------------------------------------------
    float m_sim_fps;

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Used in the game loop to limit the timestep size.
    //----------------------------------------------------------------------------------------------------------------------
    float m_sim_dt;

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief The time we have accumulated, to step through in chunks of m_sim_dt.
    //----------------------------------------------------------------------------------------------------------------------
    float m_sim_accumulator;
};

#endif
