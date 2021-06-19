# jules

A non-learning chat bot which consists of a backend written in like
200 lines of C.

## Building

	$ make # build
	$ make install # install

## Usage

	$ # after building, run the example bot configuration
	$ ./jules scripts/texting.jul
	$
	$ # if you installed it, you can execute the example file itself:
	$ ./scripts/texting.jul

## Custom Bot Configurations

There are four sections in a configuration file. These are "NAME", "START", "PATTERN", and "DEFAULT".
These are terminated by the beginning of the next section or by the end of file.

### NAME

The `NAME` section gives the bot a name. Whenever the bot responds, the value of NAME will be shown in
the prefix of its response. For example, a bot named "Alice" would respond in this format:

	Alice: this is my message

The syntax for this section is simple.

	NAME:<name>

### START

The `START` section describes the initial message the bot will send when started. You could say it is their
greeting. The syntax is the same as NAME.

	START:<message>

### PATTERN

The `PATTERN` section dictates which response will be given based on a regular expression. These are POSIX extended
regular expressions, which can be referenced in the regex(7) manual page. Each line represents a pattern and the
comma separated response(s) that should be given (if there are multiple one will be chosen at random). For example:

	PATTERN:
	^(hi|hello).*:hey # single response
	^(how are you|how's it going).*:good,ok,bad # multiple possible responses

### DEFAULT

The `DEFAULT` section provides responses that will be given if a pattern is not found. The syntax is similar to `PATTERN`,
except that each whole line is a possible response which will be chosen at random. For example:

	DEFAULT:
	i don't understand
	what do you mean?
	i'm not sure what that means

## License

Copyright (C) 2020-2021 Ben O'Neill <ben@benoneill.xyz>. License: GNU
GPL Version 3 <https://gnu.org/licenses/gpl.html>. This is free
software: you are free to change and redistribute it. There is NO
WARRANTY, to the extent permitted by law.
