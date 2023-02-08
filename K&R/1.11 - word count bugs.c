/* Exercise 1-11.
	How would you test the word count program? What kinds of
	input are most likely to uncover bugs if there are any?
*/

/*
	The most obvious weakness of the program (to me) is the
	state variable. It was designed to have only 2 possible
	values but that isn't enforced anywhere. Another way of
	doing that would be using an enum. Has the same effect.
*/