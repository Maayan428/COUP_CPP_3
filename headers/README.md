
# 🕹️ COUP Game – System Programming Project (C++ + SFML)

This project is a C++ implementation of the strategic board game **Coup**, designed as part of a university-level **System Programming** assignment.  
It includes full game logic, dynamic role-based behaviors, a test suite using **doctest**, and a graphical interface built with **SFML**.

---

## 🧱 Project Structure

- `main.cpp` – Runs a CLI demo of the game.
- `run_gui.cpp` – Launches the graphical version of the game.
- `Game.hpp / Game.cpp` – Core engine: manages turns, players, actions, and victory conditions.
- `Player.hpp / Player.cpp` – Shared logic for all players: state, coin management, and common actions.
- `Roles/` – Contains the unique behavior of each role (`Governor`, `Spy`, `Judge`, etc.), each inheriting from `Player`.
- `Tests/test.cpp` – A suite of unit tests for validating the game logic using the **doctest** framework.
- `GUI/` – Handles the SFML-based visual interface: user input, rendering, and interaction.
- `Makefile` – Compilation & execution automation.

---

## 🧠 Gameplay Overview

Each player starts with a hidden role and takes turns performing actions:
- **Gather** (1 coin),  
- **Tax** (2-3 coins, depending on role),  
- **Bribe** (pay 4 coins for extra turn),  
- **Arrest** (steal coins with risks),  
- **Sanction** (block other players),  
- **Coup** (eliminate another player).

Roles like **Spy**, **General**, or **Baron** can **block or enhance** these actions, adding bluffing and strategic depth.

Victory is declared when only one active player remains.

---

## ✅ Unit Testing

The project includes tests that cover:

- Basic player actions and edge cases
- Sanction and arrest logic
- Turn rotation and skipping eliminated players
- Win condition detection

Tests are located in `Tests/test.cpp` and use the lightweight [doctest](https://github.com/doctest/doctest) framework.

To run tests:

```bash
make test
```

---

## 🖼️ Graphical Interface (SFML)

A graphical version of the game is available via **SFML**, providing:

- Welcome screen with role assignment
- Visual display of players around a table
- Real-time turn flow and coin count
- Interactive targeting and input windows

To launch:

```bash
make gui
```

---

## 🔧 Build & Run Instructions

Use the provided `Makefile` to compile and run:

```bash
make Main       # CLI version (text-based demo)
make test       # Run unit tests
make gui        # Run the graphical version
make valgrind   # Check for memory leaks
make clean      # Clean compiled files
```

Ensure you have SFML and doctest installed beforehand.

---

## 📚 Requirements

- `C++17` or later
- [SFML 2.5+](https://www.sfml-dev.org/)
- [`doctest.h`](https://github.com/doctest/doctest) – place in `/usr/include/doctest` or provide local path

---

## 👩‍💻 Author

Maayan Turgeman  
📧 maayan428@gmail.com

---