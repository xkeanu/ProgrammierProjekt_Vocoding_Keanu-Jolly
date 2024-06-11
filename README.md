# ProgrammierProjekt_Vocoding_Keanu-Jolly

## Voice Changer in C++ für das Programmierprojekt

### BUILD - WINDOWS

#### Voraussetzungen
1. **Visual Studio:**
   - Installiere Visual Studio mit dem package "Desktopentwicklung mit C++".

2. **vcpkg installieren:**
   - Anweisungen zum Installieren von vcpkg: [Getting Started with vcpkg](https://github.com/microsoft/vcpkg#quick-start-windows)

#### Abhängigkeiten installieren
1. Öffne die Eingabeaufforderung und navigiere zu deinem vcpkg-Verzeichnis.
2. Installiere die benötigten Bibliotheken:

   ```sh
   .\vcpkg\vcpkg install soundtouch
   .\vcpkg\vcpkg install portaudio
   .\vcpkg\vcpkg integrate install
    ```
    
#### Builden

1. Projekt in vs öffnen
2. Projekt wie gewohnt builden
