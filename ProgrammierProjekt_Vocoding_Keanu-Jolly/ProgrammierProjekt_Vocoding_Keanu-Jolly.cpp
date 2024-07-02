#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>
#include <soundtouch/SoundTouch.h>
#include <string>
#include <limits>
#include <portaudio.h>
#include <conio.h>

using namespace std;
using namespace soundtouch;

struct WAVHeader { // https://stackoverflow.com/questions/13660777/c-reading-the-data-part-of-a-wav-file
    char riff[4];
    uint32_t chunkSize;
    char wave[4];
    char fmt[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char data[4];
    uint32_t dataSize;
};

bool readWavFile(const string& fileName, vector<int16_t>& audioData, int& sampleRate, int& numChannels) {
    ifstream file(fileName, ios::binary);
    WAVHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));

    sampleRate = header.sampleRate;
    numChannels = header.numChannels;

    audioData.resize(header.dataSize / sizeof(int16_t));
    file.read(reinterpret_cast<char*>(audioData.data()), header.dataSize);

    file.close();
    return true;
}

bool writeWavFile(const string& fileName, const vector<int16_t>& audioData, int sampleRate, int numChannels) {
    ofstream file(fileName, ios::binary);

    int dataSize = static_cast<int>(audioData.size() * sizeof(int16_t));
    file.write("RIFF", 4);
    int chunkSize = 36 + dataSize;
    file.write(reinterpret_cast<const char*>(&chunkSize), 4);
    file.write("WAVEfmt ", 8);
    int subChunk1Size = 16;
    file.write(reinterpret_cast<const char*>(&subChunk1Size), 4);
    short audioFormat = 1;
    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    file.write(reinterpret_cast<const char*>(&numChannels), 2);
    file.write(reinterpret_cast<const char*>(&sampleRate), 4);
    int byteRate = sampleRate * numChannels * sizeof(int16_t);
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    short blockAlign = static_cast<short>(numChannels * sizeof(int16_t));
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    short bitsPerSample = 16;
    file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&dataSize), 4);

    file.write(reinterpret_cast<const char*>(audioData.data()), dataSize);

    file.close();
    return true;
}

static int paCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    SoundTouch* soundTouch = static_cast<SoundTouch*>(userData);

    const float* in = static_cast<const float*>(inputBuffer);
    float* out = static_cast<float*>(outputBuffer);

    soundTouch->putSamples(in, framesPerBuffer);
    soundTouch->receiveSamples(out, framesPerBuffer);

    return paContinue;
}

void processAudio(const string& inputFileName, const string& outputFileName, float pitch, float tempo) {
    vector<int16_t> audioData;
    int sampleRate, numChannels;

    readWavFile(inputFileName, audioData, sampleRate, numChannels);

    SoundTouch soundTouch;
    soundTouch.setSampleRate(sampleRate);
    soundTouch.setChannels(numChannels);
    soundTouch.setTempo(tempo);
    soundTouch.setPitch(pitch);

    vector<float> floatAudioData(audioData.begin(), audioData.end());
    soundTouch.putSamples(floatAudioData.data(), static_cast<uint>(floatAudioData.size() / numChannels));
    int numSamples = soundTouch.numSamples();
    vector<float> processedFloatData(numSamples * numChannels);
    soundTouch.receiveSamples(processedFloatData.data(), numSamples);

    vector<int16_t> processedData(processedFloatData.begin(), processedFloatData.end());

    writeWavFile(outputFileName, processedData, sampleRate, numChannels);
    cout << "[!] Done! Output saved to " << outputFileName << endl;
}

void displaySlider(float pitch) {
    int sliderLength = 20;
    int position = static_cast<int>((pitch - 0.1f) / (2.0f - 0.1f) * (sliderLength - 1));

    cout << "\033[2K\r[!] Pitch: ";
    for (int i = 0; i < sliderLength; ++i) {
        if (i == position) {
            cout << "0";
        }
        else {
            cout << "-";
        }
    }
    cout.flush();
}

void realtimeVoice(float pitch) {
    PaError err;
    PaStream* stream;
    PaStreamParameters inputParameters, outputParameters;

    SoundTouch soundTouch;
    soundTouch.setSampleRate(48000);
    soundTouch.setChannels(2);
    soundTouch.setPitch(pitch);

    err = Pa_Initialize();

    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = 2;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream, &inputParameters, &outputParameters, 48000, paFramesPerBufferUnspecified, paClipOff, paCallback, &soundTouch);
    err = Pa_StartStream(stream);

    cout << "[!] Realtime voice changer active! Press '+' to increase pitch, '-' to decrease pitch, and 'q' to stop..." << endl;
    displaySlider(pitch);
    char ch;
    while ((ch = _getch()) != 'q') {
        if (ch == '+') {
            pitch = min(2.0f, pitch + 0.1f);
            soundTouch.setPitch(pitch);
            displaySlider(pitch);
        }
        else if (ch == '-') {
            pitch = max(0.1f, pitch - 0.1f);
            soundTouch.setPitch(pitch);
            displaySlider(pitch);
        }
    }

    err = Pa_StopStream(stream);

    Pa_CloseStream(stream);
    Pa_Terminate();
}

void selectWave(float pitch, float tempo) {
    string inputFileName, outputFileName;
    cout << "[!] Enter wave file name: ";
    cin >> inputFileName;
    cout << "[!] Enter output wave file name: ";
    cin >> outputFileName;

    processAudio(inputFileName, outputFileName, pitch, tempo);
}

void displayMenu() {
    cout << "\n[+] 1: Select Wave" << endl;
    cout << "[+] 2: Realtime (Default mic)" << endl;
    cout << "\n[!] Enter your choice: ";
}

void selectPitchAndTempo(float& pitch, float& tempo, bool isRealtime) {
    cout << "[!] Enter Pitch (0.1 - 2.0): ";
    cin >> pitch;
    if (cin.fail()) {
        pitch = 1.0f;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    if (!isRealtime) {
        cout << "[!] Enter Tempo (0.1 - 10): ";
        cin >> tempo;
        if (cin.fail()) {
            tempo = 1.0f;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

int main() {
    int choice;
    float pitch = 1.0f;
    float tempo = 1.0f;

    displayMenu();
    cin >> choice;

    switch (choice) {
    case 1:
    case 2:
        selectPitchAndTempo(pitch, tempo, choice == 2);
        break;
    default:
        cerr << "[!] Unknown option selected" << endl;
        return 1;
    }

    switch (choice) {
    case 1:
        selectWave(pitch, tempo);
        break;
    case 2:
        realtimeVoice(pitch);
        break;
    }

    return 0;
}
