# ROV Minimap ESP - Zygisk Module

> Show enemy players on minimap in Arena of Valor (ROV)

## Features

- 🎯 **Enemy on Minimap** - See all enemy heroes on the minimap
- 👁 **Fog Bypass** - Disable fog of war for enemy visibility
- ❤️ **Enemy Health** - Display enemy health status
- 📍 **Real-time Tracking** - Live position updates

## Requirements

- Rooted Android device
- Magisk (v20.4+) or KernelSU
- ARM64 device (arm64-v8a)
- Android 9+ (API 29+)

## Installation

1. Download `rov-minimap-esp-v1.0.0.zip`
2. Open **Magisk Manager**
3. Go to **Modules** → **Install from Storage**
4. Select the downloaded ZIP
5. **Reboot** your device

## Usage

1. Open **Arena of Valor** (ROV)
2. The module auto-injects when the game starts
3. **3-finger tap** anywhere on screen to toggle the mod menu
4. Go to **ESP** tab → Enable **"Show Enemies on Minimap"**

## Build from Source

### Prerequisites

- Android NDK (r25+)
- CMake 3.14+
- Git

### Build

```bash
chmod +x build.sh
./build.sh
```

The output ZIP will be at `build/rov-minimap-esp-v1.0.0.zip`

## Project Structure

```
rov-minimap-esp/
├── src/
│   ├── main.cpp          # Entry point & JNI exports
│   ├── il2cpp.h/cpp      # IL2CPP API wrapper
│   ├── hacks.h/cpp       # Game hack implementation
│   └── menu.h/cpp        # ImGui mod menu
├── overlay/
│   ├── overlay.h/cpp     # OpenGL overlay system
│   └── libs/             # ImGui & Dobby
├── module.prop           # Magisk module metadata
├── customize.sh          # Installation script
├── CMakeLists.txt        # Build configuration
└── build.sh              # Build script
```

## How It Works

1. **Zygisk Injection** - Module loads into ROV's process via Magisk/Zygisk
2. **IL2CPP Resolution** - Finds game classes and methods dynamically
3. **Fog Bypass** - Hooks visibility/fog-of-war system to reveal enemies
4. **Minimap Hook** - Injects enemy markers into minimap rendering
5. **Overlay Menu** - ImGui-based menu with 3-finger gesture activation

## Target

| Property | Value |
|----------|-------|
| Package | `com.garena.game.kgth` |
| Engine | Unity (IL2CPP) |
| Arch | ARM64 |

## ⚠️ Disclaimer

**THIS IS FOR EDUCATIONAL PURPOSES ONLY**

- Using this mod violates the game's Terms of Service
- Your account **WILL** be banned if detected
- Use only on test/private accounts
- The author is not responsible for any consequences

## License

This project is provided as-is for educational purposes.
Do not use for malicious purposes.
