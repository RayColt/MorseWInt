#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include <windows.h>
#include <string>
#include <iostream>
#include <map>
#include <iterator>
#include <vector>
#include <regex>


/**
* C++ MorseWav.h file
*/
class Morse
{
public:
	const std::string error_in;
	double frequency_in_hertz;
	double words_per_minute;
	//double max_frequency_in_hertz;
	//double min_frequency_in_hertz;
	double samples_per_second;

	Morse();
	std::multimap <std::string, std::string> morse_map;
	std::multimap <std::string, std::string> morse_map_reversed;
	void fill_morse_maps();

	std::string getBinChar(std::string character);
	std::string getMorse(std::string character);
	std::string getCharacter(std::string morse);

	std::string morse_encode(std::string str);
	std::string morse_decode(std::string str);
	std::string morse_binary(std::string str);
	std::string bin_morse_hexdecimal(std::string str, int modus);
	std::string hexdecimal_bin_txt(std::string str, int modus);

	std::string stringArrToString(std::vector<std::string> vstr);
	std::string strtr(std::string str, std::string from, std::string to);
	std::string trim(const std::string& str);
	const std::vector<std::string> explode(const std::string& s, const char& c);
	std::string fix_input(std::string str);
	std::string remove_whitespaces(std::string str);
	std::string stringToUpper(std::string str);

	double duration_milliseconds(double wpm);
};
