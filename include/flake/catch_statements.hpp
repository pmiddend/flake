#ifndef FLAKE_CATCH_STATEMENTS_HPP_INCLUDED
#define FLAKE_CATCH_STATEMENTS_HPP_INCLUDED

#include <awl/main/exit_failure.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/text.hpp>
#include <fcppt/config/external_begin.hpp>
#include <flake/message_box.hpp>
#include <exception>
#include <fcppt/config/external_end.hpp>


// Purely a hack because libclang doesn't get this try {} catch notation.
#ifndef FLAKE_NO_CATCH_STATEMENTS
#define FLAKE_CATCH_STATEMENTS\
	catch(\
		fcppt::exception const &e)\
	{\
		flake::message_box(\
			FCPPT_TEXT("fcppt exception caught: ")+\
			e.string());\
		return\
			awl::main::exit_failure();\
	}\
	catch(\
		std::exception const &e)\
	{\
		flake::message_box(\
			FCPPT_TEXT("fcppt exception caught: ")+\
			fcppt::from_std_string(\
				e.what()));\
		return\
			awl::main::exit_failure();\
	}\
	catch(\
		...)\
	{\
		flake::message_box(\
			FCPPT_TEXT("unknown exception caught"));\
		return\
			awl::main::exit_failure();\
	}
#else
#define FLAKE_CATCH_STATEMENTS
#endif

#endif

