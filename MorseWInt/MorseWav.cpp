#include "morsewav.h"
#include <shellapi.h>
#include "mmeapi.h "
#pragma comment(lib, "Shell32.lib")

using namespace std;

/**
* Constructor
*/
MorseWav::MorseWav(const char* morsecode, double tone, double wpm, double samples_per_second, int modus, bool show, bool open)
{
    MorseWav::CreateFullPath();
    MorseCode = morsecode;
    NumChannels = modus;
    Wpm = wpm;
    Tone = tone;
    Sps = samples_per_second;

    // Note 60 seconds = 1 minute and 50 elements = 1 morse word.
    Eps = Wpm / 1.2;    // elements per second (frequency of morse coding)
    Bit = 1.2 / Wpm;    // seconds per element (period of morse coding)

	cout << "wave: " << Sps << " Hz (-sps:" << Sps << ")\n";
	cout << "tone: " << Tone << " Hz (-tone:" << Tone << ")\n";
	cout << "code: " << Eps << " Hz (-wpm:" << Wpm << ")\n";

    MorseWav::MorseTones(MorseCode);
    MorseWav::WriteWav(pcm);

	int mod = (NumChannels == 2) ? 2 : 1;
	cout << PcmCount * mod << " PCM samples";
	cout << " (" << ((double)PcmCount / Sps) << " s @ " << (Sps / 1e3) << " kHz)";
	cout << " written to\n " << FullPath << " (" << (WaveSize / 1024.0) << " kB)\n";

    if (open)
    {
        /* IF SHELLAPI DOES NOT WORK, USE SYSTEM COMMAND
		 * BUT THIS OPENS A NEW CONSOLE WINDOW WHICH IS ANNOYING
        string str = FullPath;
        str += " /play /close ";
        str += FullPath;
        const char* c = str.c_str();
        printf("** %s\n", c);
        system(c);*/
        ShellExecuteA(NULL, "open", FullPath.c_str(), NULL, NULL, show);
    }
}

/**
* Get full save path
*/
string MorseWav::GetFullPath()
{
	return FullPath;
}

void MorseWav::CreateFullPath()
{
    string filename = "morse_";
    filename += to_string(time(NULL));
    filename += ".wav";

    FullPath = SaveDir + filename;
}

/**
* Get GetPcmCount
*/
long MorseWav::GetPcmCount()
{
    return PcmCount;
}

/**
* Get GetWaveSize
*/
long MorseWav::GetWaveSize()
{
    return WaveSize;
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
    double seconds = Bit;   // seconds to generate for this quantum
    size_t numsamples = static_cast<size_t>(round(seconds * Sps)); // samples per channel
    if (numsamples == 0) return;

    const bool stereo = (NumChannels == 2);
    size_t samplesToAdd = numsamples * (stereo ? 2u : 1u);

    // Resize once up-front and write by index (avoids push_back overhead and reallocations)
    size_t old = pcm.size();
    pcm.resize(old + samplesToAdd);

    constexpr double twoPi = 2.0 * M_PI;
    constexpr int16_t maxInt16 = numeric_limits<int16_t>::max();
    const double amp = Amplitude * static_cast<double>(maxInt16);

    if (silence == 0)
    {
        // Fast path: fill zeros for silence
        if (stereo)
        {
            for (size_t i = 0; i < numsamples; ++i)
            {
                pcm[old + i * 2]     = 0;
                pcm[old + i * 2 + 1] = 0;
            }
        }
        else
        {
            for (size_t i = 0; i < numsamples; ++i)
            {
                pcm[old + i] = 0;
            }
        }
        PcmCount += static_cast<long>(numsamples); // PcmCount counts frames (samples-per-channel)
        return;
    }

    // Tone generation: efficient recursive oscillator (no sin per-sample)
    const double omega = (twoPi * Tone) / Sps;
    const double coeff = 2.0 * cos(omega);

    // initialize two starting values for the recurrence using the persistent phase
    double y_prev = sin(phase);
    double y_cur  = sin(phase + omega);

    if (stereo)
    {
        for (size_t i = 0; i < numsamples; ++i)
        {
            double sample = (i == 0) ? y_prev : y_cur;
            double scaled = sample * amp;
            if (scaled > maxInt16) scaled = maxInt16;
            else if (scaled < -maxInt16) scaled = -maxInt16;
            int16_t outSample = static_cast<int16_t>(scaled);

            pcm[old + i * 2]     = outSample;
            pcm[old + i * 2 + 1] = outSample;

            double y_next = coeff * y_cur - y_prev;
            y_prev = y_cur;
            y_cur = y_next;
        }
    }
    else
    {
        for (size_t i = 0; i < numsamples; ++i)
        {
            double sample = (i == 0) ? y_prev : y_cur;
            double scaled = sample * amp;
            if (scaled > maxInt16) scaled = maxInt16;
            else if (scaled < -maxInt16) scaled = -maxInt16;
            int16_t outSample = static_cast<int16_t>(scaled);

            pcm[old + i] = outSample;

            double y_next = coeff * y_cur - y_prev;
            y_prev = y_cur;
            y_cur = y_next;
        }
    }

    PcmCount += static_cast<long>(numsamples); // increment frames (samples per channel)

    // advance phase by the generated duration so next Tones() call is continuous
    double phaseAdvance = numsamples * omega;
    phase += phaseAdvance;
    // keep phase normalized to [0, 2*pi)
    while (phase >= twoPi) phase -= twoPi;
    while (phase < 0.0) phase += twoPi;
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

/**
* Write wav file
*
* @param filename
* @param pcmData
*/
void MorseWav::WriteWav(const vector<int16_t> &pcmdata)
{
    long data_size, wave_size, riff_size;
    int fmt_size = 16;
    FILE* file = NULL;

    WAVEFORMATEX wfx = { 0 }; // mmeapi.h defines a WAVEFORMAT, but we need WAVEFORMATEX with cbSize
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = NumChannels; // 1 or 2 ~ mono or stereo
    wfx.wBitsPerSample = 16; // 8 or 16
    wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) / 8;
    wfx.nSamplesPerSec = (DWORD)Sps;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    wave_size = sizeof wfx;
    data_size = (PcmCount * wfx.wBitsPerSample * wfx.nChannels) / 8;
    riff_size = fmt_size + wave_size + data_size; // 36 + data_size
	WaveSize = riff_size + 8; // 44 + dataSize

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
            throw runtime_error("Error creating directory");
            //exit(1);
        }
    }
    // Open file for binary writing
    ofstream out(FullPath, ios::binary);
    if (!out.is_open())
    {
        cerr << "Failed to open file: " << FullPath << '\n';
        // optionally inspect errno: perror("open");
        throw runtime_error("Error opening file or directory");
        //exit(1);
    }

    // RIFF header
    out.write("RIFF", 4);
    out.write(reinterpret_cast<const char*>(&riff_size), 4);
    out.write("WAVE", 4);

    // fmt subchunk
    out.write("fmt ", 4);
    out.write(reinterpret_cast<const char*>(&wave_size), 4);
    out.write(reinterpret_cast<const char*>(&wfx), wave_size);

    // data subchunk
    out.write("data", 4);
    out.write(reinterpret_cast<const char*>(&data_size), 4);
    out.write(reinterpret_cast<const char*>(pcmdata.data()), data_size);

    out.flush();
    out.close();
}
