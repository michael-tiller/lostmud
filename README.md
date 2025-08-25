# Lost Legends MUD

A derivative MUD (Multi-User Dungeon) based on the Diku/ROM/ROT family, featuring a rich fantasy world with multiple character classes, skills, and an online building system.

## Overview

Lost Legends is a text-based online roleplaying game that runs on a C89 codebase. Players can create characters, explore areas, fight monsters, cast spells, and interact with other players in a persistent fantasy world.

## Game Features

- **Multi-Tiers**: Reroll for enhanced abilities
- **201 Levels**: Double your pleasure, double your fun.
- **Custom world**: Entirely new and unique game world.
- **OLC**: Souped up custom online editor with support for helps, skills, and even classes.

### Technical Features
- **Multi-platform**: Compiles on Linux (GCC) and Windows (MSVC)
- **Custom Memory Management**: Efficient allocator system for game objects
- **Linked List Architecture**: Optimized data structures for game entities
- **Telnet Support**: Network protocol for client connections

## Building

### Prerequisites
- GCC (Linux) or MSVC (Windows)
- Standard C libraries
- Network socket support

### Compilation
```bash
cd src
make
make move
make start
```

The resulting binary will be named according to your platform.

## Running

### Starting the Server
```bash
./lostmud [port]
```

Default port is typically 4000 if not specified.

### Connecting
Players can connect using any telnet client:
```bash
telnet localhost 4000
```

Or use specialized MUD clients like:
- MUSHclient
- ZMUD
- TinTin++
- Mudlet

## Gameplay

### Character Creation
1. Connect to the server
2. Choose a character class
3. Begin your adventure in the starting area

### Basic Commands
- `look`: Examine your surroundings
- `north/south/east/west`: Move between rooms
- `inventory`: Check your possessions
- `score`: View character statistics
- `help`: Access the help system

### Advancement
- Gain experience through combat and quests
- Learn new skills and spells
- Acquire better equipment
- Join clans and form alliances

## Administration

### OLC Commands
- `redit`: Edit room descriptions
- `medit`: Modify mobile definitions
- `oedit`: Edit object properties
- `sedit`: Create and edit shops
- `skedit`: Create and edit skills
- `cedit`: Create and edit classes

### Building Areas
1. Use OLC commands to create content
2. Save areas to `.are` files
3. Areas are automatically loaded on server restart

## Contributing

This is a legacy codebase that follows strict C89 standards. When contributing:

- Maintain existing code structure and conventions
- Use the custom memory management system (`alloc_mem`, `free_mem`)
- Preserve global variables and linked list patterns
- Follow the established indentation and formatting rules
- Test changes on both Linux and Windows platforms

## License

This project is derived from the Diku/ROM/ROT MUD family. Please respect the original MIT License terms.

MIT License

Copyright (c) 2022 metagrue

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


## Support

For technical issues or questions about the codebase, please refer to the source code comments and existing documentation within the project files.

---

*Lost Legends - Where heroes are forged and the adventure never ends.*