# Console Rogue
Martin Gráf

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

## Game

This class is listed first and has its own section due to being the class in which all the encapsulated logic is connected and all the low-level work is taken care of or at least interfaced for the other classes which it typically also stores. In short, this class stores the overall Game state.

**The constructor**

The constructor simply initialises the console, populates it with respective sections - which it also initialises - and the class is then ready to map the appropriate responses to their respective user inputs.

**Member variables**

 - console - A libtcod utility. It stores the current console state.
 - context - Stores console settings and provides an interface to manipulate the console.
 - playArea - A Map object. It stores the state and interfaces methods for manipulating it. See below for detailed information.