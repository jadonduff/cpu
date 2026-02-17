# Contributing Guidelines

If you wish to contribute to this project, please follow the guidelines below.

Reminder: by contributing, you agree that your code will be licensed under the project’s license.

## Table of Contents

- [Contributing to the Project](#contributing-to-the-project)
	- [Testing](#testing)
	- [Pull Request and Commit Message Guidelines](#pull-request-and-commit-message-guidelines)
	- [Branches](#branches)
- [Code Style Guide](#code-style-guide)
	- [Documentation](#documentation)
	- [Naming](#naming)
	- [Braces](#braces)
	- [If Statements](#if-statements)
	- [Enums](#enums)
	- [Structs and Classes](#structs-and-classes)

## Contributing to the Project

All contributions to the project's main branch should be submitted through a pull request.
Once in a pull request, the contribution will be reviewed and tested. 
Once both steps are completed, the contribution can be added to the project.

### Code Review Expectations

- Reviewers should focus on correctness first.
- Contributors should be open to feedback.
- Many small pull requests are better than one big pull request.

### Pull Request and Commit Message Guidelines

All pull requests and commits should contain a title describing what is changed.
The description should list and provide reasoning for each change.

All pull requests should strictly follow this guideline, but commits can be more lax depending on branch.

### Testing

Testing should be completed by an online automated workflow with github. 
Additional testing may be completed during review.

All features added should have a test added to the "test" sub-project. 
Tests are written in Google Test framework.


### Branches

Branches should be created for development, archiving older stable releases, and experimental features.

- Development Branches should be named "dev/username"
- Stable release branches should be named "rel/major.minor"
- Experimental branches should be named "exp/major.minor"

Stable release branches should coincide with releases on github. All Stable release branches should be assumed as archived, but can receive critical fixes. All Stable release branches should never be deleted.

All other branches are mostly free to be modified and deleted. Consult owner of the branch if there is an issue. Good practice is to report a branch being modified or deleted as an issue.

## Code Style Guide

This section is dedicated to ensuring a consistent style throughout the project. 
All code should adhere to the style guidelines, but in justified situations the guidelines may be broken.

### Documentation

All features should be documented with doxygen-style comments.
Although comments are not needs, all comments within functions should be maintained similarly to the code itself.

For every release, a new doxygen will be generated.

### Naming

All classes, variables, methods, constants, namespaces, and header files should use snake_casing.

*Example*

	my_type_t my_function(int my_variable);

snake_case matches the naming convention used throughout ISO C++ standard library. 
Using a unified naming style avoids any mismatch between standard types and user-defined types.

The only exception to this rule is template parameters. 
All template parameters should be in CamelCase abbreviations (ValT, DefT).

*Example*

	template<typename ValT, typename... ParT>

Any nested templates should have distinct names from any other template in the same scope.
Within a templated class, try to add all template parameters in typedefs or constexpr statements.

### Braces

For all code blocks (function body, if body, etc), the opening curly brace should be on the same line as the keyword. 
The closing curly brace should always be on its own line or merged with other closing curly braces.

*Example*

	if( a == b ) {
		// code block
	}
	else if( a > b ) {
		// second code block
	}
	else {
		// another code block
	}

For C-style object declarations, you may keep everything on the same line or adhere to the code block standard.

*Example*

	my_type_t val { .data = 1 };

### If statements

Single-line if statements are permitted only as early terminators in functions, similar to asserts.

*Example*

	bool my_func(int x) {
		if(x < 5) return false;
		// do something
		return true;
	}



### Enums

All enums should be enclosed in a namespace. The enum block should be anonymous. 
A typedef that shares the name should be present similar to the example.

*Example*

	using my_enum_t = unsigned;
	namespace my_enum {
		enum : my_enum_t {
			value1,
			value2,
			value3,
		};
	}

Bitmasks should follow the same format as above, but assign each enum to a bit.

*Example*

	using my_bitmask_t = unsigned;
	namespace my_bitmask {
		enum : my_bitmask_t {
			bit1 = 1 << 0,
			bit2 = 1 << 1,
			bit3 = 1 << 2,
			combined = bit1 | bit2,
		};
	}

These requrements for enums are to ensure maximum flexibility. 
Such as, stating a "using namespace" clause for the enums; prevent the need to type-cast enums to integers; and to allow for bitmasks.

### Structs and Classes

A struct should be used when declaring a type, if it is comparable in use to trivial types (int, float, etc). 
An example of a trivial type is vectors when developing math libraries.

A class should be used when declaring a type in all other cases not described by struct.

It is up to programmer to decide the cutoff between struct and class, but generally class should be taken as the default.
