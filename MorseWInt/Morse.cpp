#include "morse.h"

/**
* C++ Morse Class
*
* @author Ray Colt <ray_colt@pentagon.mil>
* @copyright Copyright (c) 1978, 2026 Ray Colt
* @license MIT License
*
* You can damage your hearing or your speakers if you play tones at extreme volumes!
* This program will not allow to play morse < 37 Hz and > 8,000 Hz.
**/
using namespace std;

/**
* Constructor
*/
Morse::Morse()
{
	fill_morse_maps();
}

/**
* Fill Multimaps with morse tables
*/
multimap <string, string> morse_map;
multimap <string, string> morse_map_reversed;

void Morse::fill_morse_maps()
{
	morse_map.insert(pair <string, string>(" ", ""));        // SPACE (0b1)
	morse_map.insert(pair <string, string>("!", "101011"));  // -.-.--
	morse_map.insert(pair <string, string>("$", "0001001")); // ...-..-
	morse_map.insert(pair <string, string>("\"", "010010")); // .-..-.

	morse_map.insert(pair <string, string>("'", "011110"));  // .----.
	morse_map.insert(pair <string, string>("(", "10110"));   // -.--.
	morse_map.insert(pair <string, string>(")", "101101"));  // -.--.-

	morse_map.insert(pair <string, string>(",", "110011"));  // --..--
	morse_map.insert(pair <string, string>("-", "100001"));  // -....-
	morse_map.insert(pair <string, string>(".", "010101"));  // .-.-.-
	morse_map.insert(pair <string, string>("/", "10010"));   // -..-.

	morse_map.insert(pair <string, string>("0", "11111"));   // -----
	morse_map.insert(pair <string, string>("1", "01111"));   // .----
	morse_map.insert(pair <string, string>("2", "00111"));   // ..---
	morse_map.insert(pair <string, string>("3", "00011"));   // ...--
	morse_map.insert(pair <string, string>("4", "00001"));   // ....-
	morse_map.insert(pair <string, string>("5", "00000"));   // .....
	morse_map.insert(pair <string, string>("6", "10000"));   // -....
	morse_map.insert(pair <string, string>("7", "11000"));   // --...
	morse_map.insert(pair <string, string>("8", "11100"));   // ---..
	morse_map.insert(pair <string, string>("9", "11110"));   // ----.

	morse_map.insert(pair <string, string>(":", "111000"));  // ---...
	morse_map.insert(pair <string, string>(";", "101010"));  // -.-.-.
	morse_map.insert(pair <string, string>("=", "10001"));   // -...-
	morse_map.insert(pair <string, string>("?", "001100"));  // ..--..
	morse_map.insert(pair <string, string>("@", "011010"));  // .--.-.

	morse_map.insert(pair <string, string>("A", "01"));      // .-
	morse_map.insert(pair <string, string>("B", "1000"));    // -...
	morse_map.insert(pair <string, string>("C", "1010"));    // -.-.
	morse_map.insert(pair <string, string>("D", "100"));     // -..
	morse_map.insert(pair <string, string>("E", "0"));       // .
	morse_map.insert(pair <string, string>("F", "0010"));    // ..-.
	morse_map.insert(pair <string, string>("G", "110"));     // --.
	morse_map.insert(pair <string, string>("H", "0000"));    // ....
	morse_map.insert(pair <string, string>("I", "00"));      // ..
	morse_map.insert(pair <string, string>("J", "0111"));    // .---
	morse_map.insert(pair <string, string>("K", "101"));     // -.-
	morse_map.insert(pair <string, string>("L", "0100"));    // .-..
	morse_map.insert(pair <string, string>("M", "11"));      // --
	morse_map.insert(pair <string, string>("N", "10"));      // -.
	morse_map.insert(pair <string, string>("O", "111"));     // ---
	morse_map.insert(pair <string, string>("P", "0110"));    // .--.
	morse_map.insert(pair <string, string>("Q", "1101"));    // --.-
	morse_map.insert(pair <string, string>("R", "010"));     // .-.
	morse_map.insert(pair <string, string>("S", "000"));     // ...
	morse_map.insert(pair <string, string>("T", "1"));       // -
	morse_map.insert(pair <string, string>("U", "001"));     // ..-
	morse_map.insert(pair <string, string>("V", "0001"));    // ...-
	morse_map.insert(pair <string, string>("W", "011"));     // .--
	morse_map.insert(pair <string, string>("X", "1001"));    // -..-
	morse_map.insert(pair <string, string>("Y", "1011"));    // -.--
	morse_map.insert(pair <string, string>("Z", "1100"));    // --..
	morse_map.insert(pair <string, string>("_", "001101"));  // ..--.-
	morse_map.insert(pair <string, string>("ERR", "00000000")); // ........
	// invert morse_map
	for (const auto& it : morse_map)
	{
		morse_map_reversed.insert(make_pair(it.second, it.first));
	}
}

/**
* Get binary morse code (dit/dah) for a given character
*
* @param character
* @return string
*/
string Morse::getBinChar(string character)
{
	return morse_map.find(character)->second;
}

/**
* Get morse code (dit/dah) for a given character
*
* @param character
* @return string
*/
string Morse::getMorse(string character)
{
	return strtr(morse_map.find(character)->second, "01", ".-");
}

/**
* Get character for given morse code
*
* @param morse
* @return string
*/
string Morse::getCharacter(string morse)
{
	string key = strtr(morse, ".-", "01");
	auto it = morse_map_reversed.find(key);
	if (it != morse_map_reversed.end())
	{
		return it->second;
	}
	return "-1";
}

/**
* Get binary morse code for given string
*
* @param str
* @return string
*/
string Morse::morse_binary(string str)
{
	string line = "";
	str = fix_input(str);
	str = regex_replace(str, regex("\\s{2,}"), " ");
	str = regex_replace(str, regex("[\t]+"), " ");
	for (size_t i = 0; i < str.length(); i++)
	{
		string chr = str.substr(i, 1);
		line += getBinChar(stringToUpper(chr));
		line += " ";
	}
	return trim(line);
}

/**
* Get morse code for given string
*
* @param str
* @return string
*/
string Morse::morse_encode(string str)
{
	string line = "";
	str = fix_input(str);
	str = regex_replace(str, regex("\\s{2,}"), " ");
	str = regex_replace(str, regex("[\t]+"), " ");
	for (size_t i = 0; i < str.length(); i++)
	{
		string chr = str.substr(i, 1);
		line += getMorse(stringToUpper(chr));
		line += " ";
	}
	return trim(line);
}

/**
* Get character string for given morse code
*
* @param str
* @return string
*/
string Morse::morse_decode(string str)
{
	string line = "";
	str = regex_replace(str, regex("[\t]+"), " ");
	if (regex_match(str, regex("[10\\s\\.\\-]+")))
	{
		vector<string> morsecodes = explode(str, ' ');
		for (auto morse : morsecodes)
		{
			if (morse.empty())
			{
				// a word separator
				line += " ";
				continue;
			}
			if (morse.size() < 9)
			{
				string ch = getCharacter(morse);
				if (ch == "-1")
				{
					line += "?";
				}
				else
				{
					line += ch;
				}
			}
			else
			{
				line += "?";
			}
		}
		return regex_replace(line, regex("\\s{2,}"), " ");
	}
	else
	{
		return "INPUT-ERROR";
	}
}

/**
* (\  /)
* ( .  .)
* Get hexadecimal morse code for given string
*
* @param str
* @param modus
* @return string
*/
string Morse::bin_morse_hexdecimal(string str, int modus)
{
	string str1, str2;
	const char* a[] = { "2E ", "2D ", "30 ", "31 " };
	if (modus == 0) { str1 = a[0]; str2 = a[1]; };
	if (modus == 1) { str1 = a[2]; str2 = a[3]; };
	string line = morse_binary(str);
	line = regex_replace(line, regex("  "), "A");
	line = regex_replace(line, regex(" "), "K");
	line = regex_replace(line, regex("0"), str1);
	line = regex_replace(line, regex("1"), str2);
	line = regex_replace(line, regex("A"), "20 20 ");
	line = regex_replace(line, regex("K"), "20 ");
	return trim(line);
}

/**
* (\  /)
* ( .  .)
* Get txt for given hexadecimal morse code
*
* @param str
* @param modus
* @return string
*/
string Morse::hexdecimal_bin_txt(string str, int modus)
{
	string str1, str2;
	if (regex_match(str, regex("[20|30|31|2D|2E|\\s]+")))
	{
		const char* a[] = { "2E", "2D", "30", "31" };
		if (modus == 0) { str1 = a[0]; str2 = a[1]; };
		if (modus == 1) { str1 = a[2]; str2 = a[3]; };
		string line = remove_whitespaces(str);
		line = regex_replace(line, regex("2020"), "  ");
		line = regex_replace(line, regex("20"), " ");
		line = regex_replace(line, regex(str1), "0");
		line = regex_replace(line, regex(str2), "1");
		string s = morse_decode(trim(line));
		return s;
	}
	else
	{
		return "INPUT-ERROR";
	}
}

/**
* A function that converts a string to uppercase letters
*
* @param str
* @return string
*/
string Morse::stringToUpper(string str)
{
	transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

/**
* Convert a string array into a string <br>
*
* @param vstr
* @return string
*/
string Morse::stringArrToString(vector<string> vstr)
{
	string scr = "";
	if (!vstr.empty())
	{
		for (size_t i = 0; i < vstr.size(); i++)
		{
			scr += vstr[i];
		}
	}
	return scr;
}

/**
* Similar to strtr in php, characters in 'from' will be <br>
* replaced by characters in 'to' in the same <br>
* order character by character.
*
* @param str
* @param from
* @param to
* @return string
*/
string Morse::strtr(string str, string from, string to)
{
	vector<string> out;
	for (size_t i = 0, len = str.length(); i < len; i++)
	{
		char c = str.at(i);
		int p = (int)from.find(c);
		if (p >= 0)
		{
			char t = to.at(p);
			out.push_back(string(1, t));
		}
	}
	return !out.empty() ? stringArrToString(out) : str;
}

/**
* trimp automatically strips space at the start and end of a given string <br>
*
* @param str
* @return string
*/
string Morse::trim(const string& str)
{
	size_t first = str.find_first_not_of(' ');
	if (string::npos == first)
	{
		return str;
	}
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

/**
* Similar to explode in php, a split function on a delimiter <br>
*
* @param s
* @param c
* @return vector
*/
const vector<string> Morse::explode(const string& s, const char& c)
{
	string buff;
	vector<string> vstr;
	for (auto str : s)
	{
		if (str != c)
		{
			buff += str;
		}
		else
		{
			vstr.push_back(buff);
			buff = "";
		}
	}
	if (buff != "")
		vstr.push_back(buff);
	return vstr;
}

/**
* Fix input with whitespace to reduce errors
* info: regex specialChars{ R"([-[\]{}()*+?.,\^$|#\s])" };
*
* @param str
* @return string
*/
string Morse::fix_input(string str)
{
	string ret = "";
	regex e("[^a-zA-Z0-9!'\"@/_=\\s\\$\\(\\)\\,\\.\\:\\;\\?\\-]+");
	sregex_token_iterator iter(str.begin(), str.end(), e, -1), end;
	vector<string> vec(iter, end);
	for (auto a : vec)
	{
		ret += a + " ";
	}
	return trim(ret);
}

/**
* Remove all whitespaces for given string
*
* @param str
* @return string
*/
string Morse::remove_whitespaces(string str)
{
	str.erase(remove(str.begin(), str.end(), ' '), str.end());
	return str;
}

/**
* Calculate words per second to the duration in milliseconds
*
* Dit: 1 unit
* Dah: 3 units
* Intra-character space: 1 unit
* Inter-character space: 3 Farnsworth-units
* Word space: longer than 7 Farnsworth-units
* Standard word Paris is 50 units, elements. (with one extra word space)
*
* @param wpm - words per minute
* @return double
*/
double Morse::duration_milliseconds(double wpm)
{
	double ms = 0.0;
	if (wpm > 0.0)
	{
		//50 elements per word
		double wps = wpm / 60.0; //words per second
		double eps = 50 * wps; //elements per second
		ms = 1000.0 / eps; //milliseconds per element
		return ms;
	}
	else return ms;
}
