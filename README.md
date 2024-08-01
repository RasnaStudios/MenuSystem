# MenuSystem

A basic template for implementing multiplayer game sessions in Unreal Engine 5.4.

## Overview

This project provides a foundational structure for handling multiplayer sessions in Unreal Engine games. While the user interface is minimal, the underlying system offers a robust starting point for developers looking to implement multiplayer functionality.

https://github.com/user-attachments/assets/086e9d37-a7d5-47e7-994e-a39a6d9524e7

## Test the executable

Download the zip file from the [release page](https://github.com/RasnaStudios/MenuSystem/releases) depending on your platform.
To test this package, you need to have separate computers with a different steam account respectively. Steam app/client must be open.

When opening the app, you can choose either Host or Join. One computer must Host a session. The other one can then Join.
After some seconds the joiner would appear in the host game.

## Features

- Basic session creation and management
- Steam integration for online multiplayer
- Template for lobby system
- Cross-platform support (Windows, Mac, Linux)

## Getting Started

1. Clone the repository
2. Open the project in Unreal Engine 5.4
3. Build and run the project

## Dependencies

- Unreal Engine 5.4
- OnlineSubsystem
- OnlineSubsystemSteam

## Project Structure

- `Source/MenuSystem`: Contains the C++ source code for the session management system
- `Plugins/MultiplayerSessions`: Custom plugin for handling multiplayer sessions
- `Content`: Holds all the Unreal Engine assets, including basic UI elements

## Configuration

The project is configured to support up to 100 players in a single session. You can modify this in the `DefaultGame.ini` file.

## Platforms

This project is configured to target:
- Windows
- Mac
- Linux
- Linux ARM64

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Note

This template provides a basic structure for multiplayer sessions. It's intended as a starting point for developers to build upon and customize according to their specific game requirements.

## Acknowledgements

This project is inspired by Stephen Ulibarri's course on Unreal Engine multiplayer game development.

## About

Developed by Rasna Studios. Copyright (c) 2023-2024 Rasna Studios. All rights reserved.
