# Real language syntax
Learn the syntax of the Real language; and build your first application.


## Contents
 * [Basic syntax](#Basic-syntax)
 * [Logic](#Logic)
 * [Return registers](#Return-registers)
 * [Functions](#Functions)
   * [Parameters](#Parameters)
   * [Starting function](#Starting-function)
 * [Comments](#Comments)
 * [Default constants](#Default-constants)
 * [Before you start](#Before-you-start)

## Basic syntax
The basic commands to use in order to create a program.
| Command | Parameters | About | Example |
|-|-|-|-|
| add  | 2 | Adds the two values together | <nobr>`add 2 3`</nobr> |
| sub  | 2 | Subtracts the two values together | <nobr>`sub 2 3`</nobr> |
| mul  | 2 | Multiplies the two values together | <nobr>`mul 2 3`</nobr> |
| div  | 2 | Divides the two values together | <nobr>`div 2 3`</nobr> |
| inc | 1 | Increments the value by 1 | <nobr>`inc 2`</nobr> |
| dec | 1 | Deducts the value by 1 | <nobr>`dec 2`</nobr> |
| sqrt | 1 | Calculates the square root | <nobr>`sqrt 2`</nobr> |
| sin | 1 | Calculates the sine | <nobr>`sin 2`</nobr> |
| cos | 1 | Calculates the cosine | <nobr>`cos 2`</nobr> |
| tan | 1 | Calculates the tangent | <nobr>`tan 2`</nobr> |
| floor | 1 | Rounds the value downwards | <nobr>`floor 2`</nobr> |
| ceil | 1 | Rounds the value up | <nobr>`ceil 2`</nobr> |
| rnd | 1 | Rounds the value | <nobr>`rnd 2`</nobr> |
| glbl | 1 or 2 | Gets the value of the variable by the name provided, or if another parameter is provided will set the value to the name of the variable provided - in this example the variable name is `number` | <nobr>`glbl number 2`</nobr> or <nobr>`glbl number 2`</nobr> |
| parm | 1 | Gets the value of the parameter to the host function based on the name of the parameter provided - in this example the variable name is `example` | <nobr>`parm example`</nobr> |
| rt | 1 | Return from the host function and add the value to the [return registers](#Return-registers) | <nobr>`rt 2`</nobr> |
| set | 1 | Adds the value to the [return registers](#Return-registers) | <nobr>`set 2`</nobr> |
| rand | 0 | Generates a random number | `rand` |

In the examples above all of the numbers provided were whole numbers; but they're actually all treated as floating-point numbers; so the answer for <nobr>`add 2 3`</nobr> would be the exact same as <nobr>`add 2.0 3.0`</nobr> and vice-versa.

## Logic
Do you want your program to make decisions based on certain conditions; well then, in that case logic will play an important part in that.
| Statement | Parameters | About | Example |
| - | - | - | - |
| ifeq | 2 | Verifies if the 2 provided values are of equivalence | <nobr>`ifeq 1 2`</nobr> |
| ifneq | 2 | Verifies if the 2 provided values are not of equivalence | <nobr>`ifneq 1 2`</nobr> |
| ifls | 2 | Validates if the first value is less than the second value | <nobr>`ifls 1 2`</nobr> |
| ifmr | 2 | Validates if the first value is more than the second value | <nobr>`ifmr 1 2`</nobr> |
| iflsq | 2 | Validates if the first value is the equivalent, or less than the second value | <nobr>`iflsq 1 2`</nobr> |
| ifmrq | 2 | Validates if the first value is the equivalent, or more than the second value | <nobr>`ifmrq 1 2`</nobr> |
| ifelse | 0 | If all other logical conditions fail, then this will pass | `ifelse` |
| ifend | 0 | Ends the block of if statements - is required at the end of the block of if statements | `ifend` |
| loop | 0 | Starts a loop, which can only be exited by a positive `break` condition; a loop can be nested within another loop | `loop` |
| loopend | 0 | Defines the end of the loop - when is reached, the program will then go back to the most recent `loop`, and execute the code within get again, until broken by a positive `break` condition | `loopend` |
| break | 2 | Similar to `ifeq` the break command can only validate the equivalence of 2 difference values, but if the 2 values are the equivalent of each other, the break command will break out, and exit the loop (which it is required to be nested within) | <nobr>`break 1 2`</nobr> |

Each if based statement will execute one after the other, in order to find a match. Unlike loops, if based statements cannot be nested within another if statement directly; but other ways of getting nested logic from if statements would be to use [functions](#Functions).

## Return registers
What are return registers, you may ask? Well return registers are an important part of the Real language. Return registers are where the values that are returned from: a function, or any command; are stored for later use. There are 4 return registers, and from each time something is returned into the return registers, the values are then shifted along to the next register. Well then, what happens when the last register is shifted? The value fourth/last register is discarded, as it is being replaced by the value from the third. To access the values from the return registers, you can use the constant reference to the register's addresses, which values will return the value within the defined register, the following are examples of return register addresses: `rtv`, `rtv1`, `rtv2`, `rtv3`, `rtv4`; there are 4 return register addresses, but there are 5 ways to access them; `rtv` and `rtv1` will always return the value from the first register, `rtv` can be considered to be the default return register value as it will exclusively always be the most latest thing that was returned.

## Functions
A function is a part of any program, that will perform a task; typically a repeated task where it wouldn't make any sense to keep writing out the same large portion of code. A function can be called from another function, and even be nested (up to 5000 functions can be nested within each other). A function can also be provided with a set of parameters, which can be used to perform further calculations within the function. Here is an example of a function that sets the return registers to an incrementing value.

	example:
		set 1
		inc rtv1
		inc rtv1
		inc rtv1

### Parameters
But what if you want to set the starting value as a parameter. You will need to define a parameter, in this case the parameter will be named `start`; and will use the value of `start` to increment 3 times.

	example: start
		parm start
		inc rtv1
		inc rtv1
		inc rtv1
In some cases you may wish to use multiple parameters, such as to perform more calculations with the values.

	example: years months
		parm months
		div rtv1 12
		parm years
		add rtv1 rtv2

The above function utilises multiple parameters to calculate the total amount of time in years, by adding the `months` to the `years` provided. A function can have as many parameters as you wish; and when called if not all the parameters values have been provided, they will default to a `0` value.

### Starting function 
Everything in the Real language, although all being a floating-point; must be callable from within a function; but then the question arises of: which function will be the starting function? Well then, the fitting place to start a program in the Real language, is from function called `__start` which will always be the first function to be called from any Real program; without a defined function called `__start` the program will not package itself.

## Comments
Comments can be used to explain the code that you have written. Context, and content; of your program can be more easily identified through the use of comments throughout your program. It is an important practice to use in any language, and my using comments your code can more easily be understood by someone else when reading (or reviewing) your code. In the Real language, comments are instantiated by the `;` character, and any content after the `;` will not be used in the packaging of your program; an example of the use of comments in a program - is as follows:

	example:
		; This function is used to add 2 random values together
		rand ; A random value is added to the return register
		rand ; Another random value is added to the return register
		add rtv1 rtv2 ; Adds together the 2 most recent values in the return register

## Default constants
There are constant values (or references) that you will need to use at some point. What follows is a reference list of the default constants - for use, when developing your program.
| Constant | About | Value |
| - | - | - |
| rtv | The value from the first return register, updates as values are returned |  |
| rtv1 | The value from the first return register, updates as values are returned |  |
| rtv2 | The value from the second return register, updates as values are returned |  |
| rtv3 | The value from the third return register, updates as values are returned |  |
| rtv4 | The value from the fourth return register, updates as values are returned |  |
| PI | The number π (Pi) is a mathematical constant, it is defined as the ratio of a circle's circumference to its diameter | <nobr>`3.1415927410`</nobr> |
| RADIAN | The length of an arc of a unit circle is numerically equal to the measurement in radians of the angle that it subtends | <nobr>`57.295779513`</nobr> |
| TRUE | The value for true boolean logic | `1.0` |
| FALSE | The value for false boolean logic | `0.0` |
| NULL | Used to denote that no value is provided | `0.0` |

<br>

###### Last updated: 15/11/2020 - Bradley Marshall