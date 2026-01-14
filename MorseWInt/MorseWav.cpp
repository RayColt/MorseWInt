#include "morsewav.h"
#include <shellapi.h>
#pragma comment(lib, "Shell32.lib")

using namespace std;

/**
* Constructor
*/
MorseWav::MorseWav(const char* morsecode, double tone, double wpm, double samples_per_second, int modus) :     
    MorseCode(morsecode),
    NumChannels(modus),
    Wpm(wpm),
    Tone(tone),
    Sps(samples_per_second),
    PcmCount(0),
	WaveSize(0)
{
    string filename = "morse_";
    filename += to_string(time(NULL));
    filename += ".wav";
    string fp = SaveDir + filename;

    // Note 60 seconds = 1 minute and 50 elements = 1 morse word.
    Eps = Wpm / 1.2;    // elements per second (frequency of morse coding)
    Bit = 1.2 / Wpm;    // seconds per element (period of morse coding)
    
    printf("wave: %9.3lf Hz (-sps:%lg)\n", Sps, Sps);
    printf("tone: %9.3lf Hz (-tone:%lg)\n", Tone, Tone);
    printf("code: %9.3lf Hz (-wpm:%lg)\n", Eps, Wpm);

    MorseWav::MorseTones(MorseCode);
    MorseWav::WriteWav(filename.c_str(), pcm);

    printf("%ld PCM samples", PcmCount);
    printf(" (%.1lf s @ %.1lf kHz)", (double)PcmCount / Sps, Sps / 1e3);
    printf(" written to %s (%.1f kB)\n", fp.c_str(), WaveSize / 1024.0);

    if (1)
    {
        /* IF SHELLAPI DOES NOT WORK, USE SYSTEM COMMAND
		 * BUT THIS OPENS A NEW CONSOLE WINDOW WHICH IS ANNOYING
        string str = Path;
        str += " /play /close ";
        str += Path;
        const char* c = str.c_str();
        printf("** %s\n", c);
        system(c);*/
        ShellExecuteA(NULL, "open", fp.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}

/**
* Get binary morse code (dit/dah) for a given character.
* Generate one quantum of silence or tone in PCM/WAV array.
* sine wave: y(t) = amplitude * sin(2 * PI * frequency * time), time = s / sample_rate
*
* @param silence
*/
void MorseWav::Tones(int silence)
{
    double seconds = Bit;   // keep seconds explicit
    size_t numsamples = static_cast<size_t>(std::round(seconds * Sps)); // number of samples to generate
    const double twoPiF = 2 * M_PI * Tone;
    const double twoPi = 2.0 * M_PI;
    constexpr int maxInt16 = std::numeric_limits<int16_t>::max(); // maximum for signed 16‑bit PCM

    pcm.reserve(pcm.size() + numsamples);
    if (NumChannels == 2)
    {
        for (size_t i = 0; i < numsamples; ++i)
        {
            double t = static_cast<double>(i) / Sps; // time in seconds
            double sampleL = std::sin(silence * twoPiF * t) * Amplitude;
            double sampleR = std::sin(silence * twoPiF * t) * Amplitude;

            // Clamp samples between -1.0 and 1.0 to avoid overflow when converting to int16_t
            int16_t intSampleL = static_cast<int16_t>(clamp(sampleL, -1.0, 1.0) * maxInt16);
            int16_t intSampleR = static_cast<int16_t>(clamp(sampleR, -1.0, 1.0) * maxInt16);

            pcm.push_back(intSampleL);
            pcm.push_back(intSampleR);
            PcmCount++;
        }
    }
    else
    {
        for (size_t i = 0; i < numsamples; ++i)
        {
            double t = static_cast<double>(i) / Sps;
            double sampleL = std::sin(silence * twoPiF * t) * Amplitude;

            // Clamp samples between -1.0 and 1.0 to avoid overflow when converting to int16_t
            int16_t intSampleL = static_cast<int16_t>(clamp(sampleL, -1.0, 1.0) * maxInt16);

            pcm.push_back(intSampleL);
            PcmCount++;
        }
    }
}

/**
* Define dit, dah, end of letter, end of word.
*
* The rules of 1/3/7 and 1/2/4(more suitable for common microphones, like webcams and phones):
* Morse code is: tone for one unit (dit) or three units (dah)
* followed by the sum of one unit of silence (always),
* plus two units of silence (if end of letter, one space),
* plus four units of silence (if also end of word).
*/
void MorseWav::Dit() { Tones(1); Tones(0); }
void MorseWav::Dah() { Tones(1); Tones(1); Tones(1); Tones(0); }
void MorseWav::Space() { Tones(0); Tones(0); }

/**
* Morse code tone generator
* 
* @param code
*/
void MorseWav::MorseTones(const char* code)
{
    char c;
    while ((c = *code++) != '\0')
    {
        //printf("%c", c);
        if (c == '.') Dit();
        if (c == '-') Dah();
        if (c == ' ') Space();
    }
}
 
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef struct _wave
{
    WORD  wFormatTag;      // format type
    WORD  nChannels;       // number of channels (i.e. mono, stereo...)
    DWORD nSamplesPerSec;  // sample rate
    DWORD nAvgBytesPerSec; // for buffer estimation
    WORD  nBlockAlign;     // block size of data
    WORD  wBitsPerSample;  // number of bits per sample of mono data
    WORD  cbSize;          // size, in bytes, of extra format information 
} WAVE;

/**
* Write wav file
*
* @param filename
* @param pcmData
*/
void MorseWav::WriteWav(const char* filename, const std::vector<int16_t> &pcmdata)
{
    long data_size, wave_size, riff_size;
    int fmt_size = 16;
    FILE* file = NULL;

	WAVE wave = { 0 };
    wave.wFormatTag = 0x1;
    wave.nChannels = NumChannels; // 1 or 2 ~ mono or stereo
    wave.wBitsPerSample = 16; // 8 or 16
    wave.nBlockAlign = (wave.wBitsPerSample * wave.nChannels) / 8;
    wave.nSamplesPerSec = (DWORD)Sps;
    wave.nAvgBytesPerSec = wave.nSamplesPerSec * wave.nBlockAlign;
    wave.cbSize = 0;

    wave_size = sizeof wave;
    data_size = (PcmCount * wave.wBitsPerSample * wave.nChannels) / 8;
    riff_size = fmt_size + wave_size + data_size; // 36 + data_size
	WaveSize = riff_size + 8;

    // Try to create the directory
    if (_mkdir(SaveDir.c_str()) == 0)
    {
        cerr << "Directory created successfully.\n";
    }
    else
    {
        if (errno == EEXIST)
        {
            cerr << "Directory already exists.\n";
        }
        else
        {
            cerr << "Error creating directory\n";
            exit(1);
        }
    }
    // Open file for binary writing
    ofstream out((SaveDir + filename), std::ios::binary);
    if (!out.is_open())
    {
        cerr << "Failed to open file: " << SaveDir + filename << '\n';
        // optionally inspect errno: std::perror("open");
        exit(1);
    }

    // RIFF header
    out.write("RIFF", 4);
    out.write(reinterpret_cast<const char*>(&riff_size), 4);
    out.write("WAVE", 4);

    // fmt subchunk
    out.write("fmt ", 4);
    out.write(reinterpret_cast<const char*>(&wave_size), 4);
    out.write(reinterpret_cast<const char*>(&wave), wave_size);

    // data subchunk
    out.write("data", 4);
    out.write(reinterpret_cast<const char*>(&data_size), 4);
    out.write(reinterpret_cast<const char*>(pcmdata.data()), data_size);

    out.flush();
    out.close();
}
