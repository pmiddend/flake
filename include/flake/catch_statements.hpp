#ifndef FLAKE_CATCH_STATEMENTS_HPP_INCLUDED
#define FLAKE_CATCH_STATEMENTS_HPP_INCLUDED

#include <awl/main/exit_failure.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/text.hpp>
#include <iostream>
#include <ostream>
#include <exception>
#include <fcppt/io/cerr.hpp>

#define FLAKE_CATCH_STATEMENTS\
	catch(\
		fcppt::exception const &e)\
	{\
		fcppt::io::cerr() << FCPPT_TEXT("fcppt exception caught: ") << e.string() << FCPPT_TEXT("\n");\
		return\
			awl::main::exit_failure();\
	}\
	catch(\
		std::exception const &e)\
	{\
		std::cerr << "standard exception caught: " << e.what() << "\n";\
		return\
			awl::main::exit_failure();\
	}\
	catch(\
		...)\
	{\
		std::cerr << "unknown exception caught\n";\
		return\
			awl::main::exit_failure();\
	}\

#endif

