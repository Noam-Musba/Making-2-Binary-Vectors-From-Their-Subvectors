# Making-2-Binary-Vectors-From-Their-Subvectors

In the PDF file you can read in depth what is the problem this project tries to identify and solve, and what are the outcomes of our solution's simulations.

In the .cpp file named "solution simulation" there is the code made to check our first solution.
This code does the whole process from scratch- it makes two random binary words, then code them, read from them randomly in the wanted window size, then classify and decode them correctly.
At the beginning of the code there are some definitions of the problem's variables- the length of the words, the length of the window, and "saftey_num" which is the variable 'k' from the pdf, meaning how many times will we read randomly the 2(n-w+1) windows. Here the user can decide what is the data he wants to check.
In the main function you can decide how many times will you want to run the simulation and it will print the average of all the simulations.

in the .cpp file named "improved solution simulation" there is the code made to check the second allegedly better solution.
This code, like the prior, does the whole process from scratch, and it has all the definitions of the problem at the beginning of the code. Here the user can decide what is the data he wants to check.
Moreover, there are two tables, which relies on the pdf file, that match the word size with the jumps between the redundancy bits and the size of the encoded window which match them.
Here too in the main function you can decide how many times the simulation will run and it will print the average of all the simulations.

For a more detailed information about the simulations, please read the PDF file, as it has all the information about the solutions and how we implemented them.

If met with a problem, please contact us via email:
Noam Musba- noamm7466@gmail.com
Etamar Croitoru- eta.croi@gmail.com
