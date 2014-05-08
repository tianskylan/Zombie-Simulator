#pragma once
#include <exception>
class InvalidOp : public std::exception
{
public:
	const char* what() const throw() { return "Invalid OP code or parameters."; }
};

class AccessViolation : public std::exception
{
public:
	const char* what() const throw() { return "Machine tried to access outside available memory."; }
};

class FileIOError : public std::exception
{
public:
	const char* what() const throw() { return "Specified file cannot be opened"; }
};

class WrongLineNumberError : public std::exception
{
public:
	const char* what() const throw() { return "Cannot jump to the specified line number"; }
};

class MemoryAccessError : public std::exception
{
public:
	const char* what() const throw() { return "Memory Access Failed"; }
};