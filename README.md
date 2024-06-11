# ProgrammierProjekt_Vocoding_Keanu-Jolly

## Voice Changer in C++ für das Programmierprojekt

### Verwendug des voicechangers

#### INFO
- Das Programm wurde nur unter Windows getestet
- Das Programm verwendet das Windows standart Eingabe und Ausgabegerät
- Bei Realtime wenn möglich keinen speed festlegen, da z.B. ein niedriger speed zu starken Verzögerungen führen kann

#### Anforderungen
- soundtorch.dll und portaudio.dll im selben Verzeichnis wie die .exe
- Für nicht realtime funktion eine .wav Datei im selben Ordner

#### Funktion 1 (NICHT REALTIME)
1. Programmpunkt 1 wählen
2. Pitch und Speed eingeben (1 = default)
3. .wav file in die Konsole ziehen
4. Output file wird im folder erzeugt

#### Funktion 2 (REALTIME)
1. Programmpunkt 2 wählen
2. Pitch eingeben (1 = default)
3. Einfach reden
4. Zum beenden enter drücken


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
