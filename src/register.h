/*
 * register.h
 *
 * This module provides register enum definition and a convinience function
 * to get a string represenation of a register enum.
 */

#ifndef REGISTER_H
#define REGISTER_H
typedef enum _Register {
	A = 0, X = 1, L = 2, B = 3, 
	S = 4, T = 5, PC = 8, SW = 9,
} Register;

const char *register_to_string(Register reg);
/*
 * Return the string representation of register enum.
 * Note that this returns a pointer to a statically allocated string literal.
 */
#endif
