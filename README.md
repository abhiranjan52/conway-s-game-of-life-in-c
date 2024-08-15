# Conway's game of life in C

simply compile the file named game_of_life.c and run.

The program also requires that some alive cells must be initialized before the next generation is created. This can be done either directly on the terminal by pressing "t" when the prompts the user to "enter a seed" or it can be done by editting the text file named game_of_life_seed.txt and pressing "f" when the prompt appears. Simply replace the space between the brackets with a "#" to represent a live cell in the game_of_life_seed.txt text file.

# to compile with gcc:
write in terminal:

gcc -o game_of_life game_of_life.c

# to run(on windows):
write in terminal:

.\game_of_life