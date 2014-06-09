Roguelike-Game-Engine
=====================

After using the Unity engine for hobby game development for a few years I decided that I wanted to try my hand at coding my own game using a built from scratch game engine so that I could learn from the ground up how games are made.

This game is written in C++ and uses the libtcod library which provides the rendering features needed for the game. 

#Play the game!
The current version of the game can be found
[here](https://dl.dropboxusercontent.com/u/6917435/PaulT%20RL%20Game%20v0.1.zip)

Game Controls:
Use the arrow keys to move around
You will attack by moving into the same square as an enemy

i-open inventory
	Choose the item you would like to use by pressing the correspong letter, esc to cancel
o-open chest when standing over a chest
g-grab an item on the ground beneath you
d-select an item to drop on the ground
< or > - move up or down stairs to a new level in the dungeon

#Screenshots:

Main Menu
![Main Menu](http://i.imgur.com/qyFil1a.png)

Fighting some Orcs!
![Main Menu](http://i.imgur.com/6Ea6mki.png)

Opening a chest and finding a spell!
![Opening Chest](http://i.imgur.com/6Ea6mki.png)

Tasks:
[x] Allow for multiple persistent levels of the dungeon
[x] Add chests that can contain multiple items
[ ] Add a minimap
[ ] Add a special overworld level which allows the player to travel between multiple dungeons
[ ] Improve monster ai
[ ] Add light intensity around the player (the map gets darker further away from the player, simulating a torch)
