# MorseWInt
Morse INT, Win32 + CMD Line in one app<br>
Do not forget to set your SaveDir in morsewav.h at line 29!!<br>
RUN and compile the project in DEBUG/x86 mode!!<br>

<img src=https://github.com/RayColt/MorseWInt/blob/master/.gitfiles/x86.jpg />
<h1>Pre Release</h1>
<img src=https://github.com/RayColt/MorseWInt/blob/master/.gitfiles/1.jpg />
<img src=https://github.com/RayColt/MorseWInt/blob/master/.gitfiles/2.jpg />

new 6 bit assignments for lowercase (a -> z), based upon ASCII, to morse modern passwords and urls:<br>
if (!uppercase)<br>
{<br>
	morse_map.insert(pair<string, string>("a", "1100001")); // --...-<br>
	morse_map.insert(pair<string, string>("b", "100010")); // -...-.<br>
	morse_map.insert(pair<string, string>("c", "100011")); // -...--<br>
	morse_map.insert(pair<string, string>("d", "100100")); //-..-.. <br>
	morse_map.insert(pair<string, string>("e", "100101")); // -..-.-<br>
	morse_map.insert(pair<string, string>("f", "100110")); // -..--.<br>
	morse_map.insert(pair<string, string>("g", "100111")); // -..---<br>
	morse_map.insert(pair<string, string>("h", "101000")); // -.-...<br>
	morse_map.insert(pair<string, string>("i", "101001")); // -.-..-<br>
	morse_map.insert(pair<string, string>("j", "1101010")); // --.-.-.<br>
	morse_map.insert(pair<string, string>("k", "1101011")); // --.-.--<br>
	morse_map.insert(pair<string, string>("l", "101100")); // -.--..<br>
	morse_map.insert(pair<string, string>("m", "1101101")); // --.--.-<br>
	morse_map.insert(pair<string, string>("n", "101110")); // -.---.<br>
	morse_map.insert(pair<string, string>("o", "101111")); // -.----<br>
	morse_map.insert(pair<string, string>("p", "110000")); // --....<br>
	morse_map.insert(pair<string, string>("q", "110001")); // --...-<br>
	morse_map.insert(pair<string, string>("r", "110010")); // --..-.<br>
	morse_map.insert(pair<string, string>("s", "1110011")); // ---..--<br>
	morse_map.insert(pair<string, string>("t", "110100")); // --.-..<br>
	morse_map.insert(pair<string, string>("u", "110101")); // --.-.-<br>
	morse_map.insert(pair<string, string>("v", "110110")); // --.--.<br>
	morse_map.insert(pair<string, string>("w", "110111")); // --.---<br>
	morse_map.insert(pair<string, string>("x", "1111000")); // ----...<br>
	morse_map.insert(pair<string, string>("y", "111001")); // ---..-<br>
	morse_map.insert(pair<string, string>("z", "111010")); // ---.-.<br>
}<br>



