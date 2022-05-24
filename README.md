# Console Rogue
Martin Gráf

---

# Controls and goals

Use __arrow keys__ to move and __spacebar__ to interract with the environment.

Interraction is either an attack of the closest enemy within reach, - G characters - picking up an upgrade, or taking an exit - E character.

One press of spacebar can perform only one of the above. The actions take priority in the mentioned order.

Goal of the game is to ascend 3 levels of randomly generated floors.

---

# Used libraries

## Libtcod

An older, but still supported and evolving library providing a useful console tool that's easy to manipulate and provides better UI features.

In the project, it is only really used as a rendering and graphical tool.

https://libtcod.readthedocs.io/en/latest/index.html

## SDL2

A library providing tools for low-level access to the computers' resources. It is only really used to read user input in the project

https://www.libsdl.org/

---

# Overview of classes

Implementations of classes are found in their respective files, but their signatures can all be found in the Header file ***GameState.h*** of course.

## The customization classes:

*Any of these classes can be used to change most aspects of the look of the game and possibly even balancing*

---

### ***Palette***

As the name suggests, the class only contains definitions of colors of individual defined entities and their shades for possible field of vision changes.

### ***Tileset***

Much the same as Palette, it is used to store what characters are used to represent what entities.

---

## Game.cpp

This class is listed first and has its own section due to being the class in which all the encapsulated logic is connected and all the low-level work is taken care of or at least interfaced for the other classes which it typically also stores. In short, this class stores the overall Game state.

Job of the Game class typically includes but is not limited to:
 - Read user input
 - Determine how to react to the user input
 - Determine to what class instances the input should be passed
 - Call functions in sections that aren't logically related, but should happen silmoutaneously (ie - update player stats rendered once an enemy attacks the player on the board)

---

## Play Area classes

### Map.cpp

This class holds the state of play area - the section of the console the player can move around in and interract with. It retains positions of individual tiles, determines sightblockers, keeps track of the state of active vision, has functions for rendering the map, has functions for coloring the map, resetting new play areas, and is in charge of the Level lifecycle.

### Level.cpp

This class populates a playArea with new randomly-generated environments, enemies, pickups, and exits. It provides an interface for accessing rooms and Actor entities independently of their position on the map, updating enemy behaviour, changing colors, difficulty levels, spawn rates, and so on. 

### RoomGenerator.cpp

Provides an interface used by Level.cpp. It calculates specific coordinates, diameters, actor and pickup positions, and possible overlaps for individual room type entities.

---

## Information classes

### EventSection.cpp

A simple class tasked with displaying event messages passed to it in the appropriate section of the play console.

### StatSection.cpp

Similar to EventSection, its only job is to render players' stats and update their values when asked to.

---

## Actor entities

### Actor.cpp

A base class for Player. A simple data storage for actor stats and position.

### Player.cpp

Differs from Actor only in more specific stats and the ability to calculate active line of sight of self.

---

## Enums

There isn't much to be said here. They are simply used for differentiation of actor entities, pickup entities, directions of movement, and room entities.