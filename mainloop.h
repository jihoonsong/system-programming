/**
 * @file  mainloop.h
 * @brief A mainloop of this program. Acts like a client of
 *        command pattern. Receives command and invokes handler.
 */

#ifndef __MAINLOOP_H__
#define __MAINLOOP_H__

/**
 * @brief Initialize all interal states.
 */
void mainloop_initialize(void);

/**
 * @brief   The main loop of this program.
 * @details Receives command along with its arguments, check if the
 *          given command is valid, pass it to the designated handler
 *          if valid, and ignore it if invalid.
 */
void mainloop_launch(void);

/**
 * @brief Release all allocated memory.
 */
void mainloop_terminate(void);

#endif
