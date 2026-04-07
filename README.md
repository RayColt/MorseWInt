# MorseWInt
Morse INT, Win32 + CMD Line in one app<br>
Do not forget to set your SaveDir in morsewav.h at line 29!!<br>
RUN and compile the project in DEBUG/x86 mode!!<br>

<img src=https://github.com/RayColt/MorseWInt/blob/master/.gitfiles/x86.jpg />
<h1>Pre Release</h1>
<img src=https://github.com/RayColt/MorseWInt/blob/master/.gitfiles/1.jpg />
<img src=https://github.com/RayColt/MorseWInt/blob/master/.gitfiles/2.jpg />

new 6 bit assignments for lowercase (a -> z), to morse passwords and safe urls:<br>
a ->	000000<br>
b ->	000001<br>
c ->	000010<br>
d ->	000011<br>
e ->	000100<br>
f ->	000101<br>
g ->	000110<br>
h ->	000111<br>
i ->	001000<br>
j ->	001001<br>
k ->	001010<br>
l ->	001011<br>
m ->	0011000<br>
n ->	0011010<br>
o ->	001110<br>
p ->	001111<br>
q ->	010000<br>
r ->	010001<br>
s ->	0100100<br>
t ->	010011<br>
u ->	010100<br>
v ->	0101010<br>
w ->	010110<br>
x ->	010111<br>
y ->	011000<br>
z ->	011001<br><br>

added extra zero at:<br>
001100 -> "?" and "m"<br>
001101 -> "_" and "n"<br>
010010 -> " (double-quote) and "s"<br>
010101 -> "." and "v"<br>
<br>
// Replaced existing lowercase entries for m,n,s,v with 4-bit candidates<br>
morse_map.insert(pair<string, string>("m", "0011"));   // ..-- new short code for 'm'<br>
morse_map.insert(pair<string, string>("n", "0101"));   // .-.- new short code for 'n'<br>
morse_map.insert(pair<string, string>("s", "1110"));   // ---. new short code for 's'<br>
morse_map.insert(pair<string, string>("v", "1111"));   // ---- new short code for 'v'<br>



