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

This codebase is derived from the Diku/ROM/ROT MUD family and remains subject to their original license terms (including attribution and non-commercial restrictions). See the original license files and headers in the source for details.

Additional changes and extensions by Michael Tiller are provided under the MIT License.

See `LICENSE` for the MIT license text.


## Support

For technical issues or questions about the codebase, please refer to the source code comments and existing documentation within the project files.

---

*Lost Legends - Where heroes are forged and the adventure never ends.*