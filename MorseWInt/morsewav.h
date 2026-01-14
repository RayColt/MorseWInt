#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#define _USE_MATH_DEFINES // Required for MSVC/Windows

#include <cmath>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <direct.h>
#include <errno.h>
#define NOMINMAX
#include <windows.h>

class MorseWav
{
private:
	const std::string SaveDir = "C:\\Users\\User\\Desktop\\wav-files-morse\\"; // output directory - use this format
	const char* MorseCode;     // morse code string
	int NumChannels;           // 1 = mono, 2 = stereo
	double Wpm;                // words per minute
	double Tone;               // tone frequency in hertz
	double Sps;                // samples per second
	double Eps;                // elements per second (frequency of morse coding)
	double Bit;                // seconds per element (period of morse coding)
	std::vector<int16_t> pcm;  // PCM data array
	long PcmCount;             // number of PCM samples
	long WaveSize;             // size of the wave file in bytes
	double Amplitude = 0.8;    // 80% of max volume (0.0 to 1.0)

public:
	/**
	* Constructor / Destructor
	*/
	MorseWav(const char* morsecode, double tone, double wpm, double samples_per_second, int modus);
	~MorseWav() = default;

private:
	/**
	* Write wav file
	*
	* @param filename
	* @param pcmData
	*/
	void WriteWav(
		const char* filename,
		const std::vector<int16_t>& pcmData
	);

	/**
	* Get binary morse code (dit/dah) for a given character.
	* Generate one quantum of silence or tone in PCM/WAV array.
	* sine wave: y(t) = amplitude * sin(2 * PI * frequency * time), time = s / sample_rate
	*
	* @param silence
	*/
	void Tones(int silence);

	/**
	* Define dit, dah, end of letter, end of word.
	*
	* The rules of 1/3/7 and 1/2/4(more suitable for common microphones, like webcams and phones):
	* Morse code is: tone for one unit (dit) or three units (dah)
	* followed by the sum of one unit of silence (always),
	* plus two units of silence (if end of letter, one space),
	* plus four units of silence (if also end of word).
	*/
	void Dit();
	void Dah();
	void Space();

	/**
	* Morse code tone generator
	*
	* @param code
	*/
	void MorseTones(const char* code);
};

