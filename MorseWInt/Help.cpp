#include "help.h"
/**
* C++ Help Class
*/
using namespace std;

/**
* Constructor
*/
//Help::Help() {}

/**
* Get help text for morse usage
*
* @return string
*/
string Help::GetHelpTxt()
{
	string str = "";
	str += "MORSE Encoder / Decoder Utility   Version 1.0\n";
	str += "----------------------------------------------\n";
	str += "Converts text to and from Morse code in multiple formats.\n";
	str += "Lowercase input is automatically converted to uppercase.\n";
	str += "\n";
	str += "SUPPORTED CHARACTERS(Url Safe):\n";
	str += "A-Z  0-9  !$ ' \" ( ) , . _ - / : ; = ? @\n";
	str += "\n";
	str += "USAGE:\n";
	str += ".\\morse.exe                           Console\n";
	str += ".\\morse.exe [-mode:] \"text or morse\"  Command line\n";
	str += "\n";
	str += "MODES:\n";
	str += "-h, -help        Display this help screen\n";
	str += "e, d             Morse Normal(. - <space>)\n";
	str += "b, d             Binary Morse(0 1 <space>)\n";
	str += "he, hd           Hex Morse(2E 2D 20)\n";
	str += "hb, hbd          Hex Binary Morse(30 31 20)\n";
	str += "\n";
	str += "AUDIO OUTPUT:\n";
	str += "ew               Morse to WAV(Stereo)   Creates WAV file\n";
	str += "ewm              Morse to WAV(Mono)     Creates WAV file\n";
	str += "es               Morse to Windows Beep  No SPS required\n";
	str += "\n";
	str += "EXAMPLES:\n";
	str += ".\\morse.exe d \"... ---  ...  ---\"\n";
	str += "(Double quotes preserve double spaces used for word breaks)\n";
	str += "\nPlace morse.exe in system32 and you can type:\n";
	str += ".\\morse ewm -wpm:50 -hz:880 -sps:11025 paris paris paris\n";
	str += "\n";
	str += "SOUND SETTINGS:\n";
	str += "Tone(Hz)         20 - 8000 Hz\n";
	str += "WPM              0 - 50 words per minute\n";
	str += "SPS              8000 - 48000 samples per second\n";
	str += "\n";
	str += "For tone ideas, consider musical note frequencies.\n";
	return str;
}