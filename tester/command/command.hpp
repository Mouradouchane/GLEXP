#pragma once 

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <map>
#include <string>

using command_function = void (*)(std::string const& args);

struct command_object {
	std::string      command;
	command_function function;
};

class commands {

private:
	std::map<std::string , command_function> command_list;

public:
	 commands() = default;
	~commands() = default;

	void add_command(std::string const& command_name , command_function func) noexcept;
	bool execute(std::string const& command_name) noexcept;
	command_object get_command(std::string const& command_name) noexcept;

	bool find(std::string const& command_name) noexcept;
};

#endif