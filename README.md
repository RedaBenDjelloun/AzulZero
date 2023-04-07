# AzulZero
Authors : Reda BEN DJELLOUN, Hector MARTINET (c) 2023, an engineering student project.

Artificial Intelligence for Azul game (2 players only).

This project use the Imagine++ library for the graphic interface which can be found here : http://imagine.enpc.fr/~monasse/Imagine++/

## The Game

[Wikipedia] Azul (Portuguese for blue) is an abstract strategy board game designed by Michael Kiesling and released by Plan B Games in 2017. Based on Portuguese tiles called azulejos, Azul is a game where players collect sets of similarly colored tiles which they place on their own board. When a row is filled, one of the tiles is moved into a square pattern on the right side of the board. Filling the pattern grants points depending on where tiles are placed in relation with one another on the board.

Wikipedia page of the game : https://en.wikipedia.org/wiki/Azul_(board_game)

Detailed rules of the game can be found in the "Documents" folder.

## Algorithms

Implementation of classic algorithms (MinMax, MCTS...) with some adjustments to make them work on Azul. Those algorithms are far from perfect, but still strong enough to compete against a good player (MCTS with 1 second by move has been undefeated by everyone who tested it during development).

## GUI

Thanks to a clean graphical interface you can play a game against an AI (or a friend) or watch two AI play against each other.
You can review a game you just finished and see if you played the best moves! The game is evaluated by the MCTS algorithm in real time.
