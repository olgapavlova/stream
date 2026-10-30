for n in 3 ..< 7: 
  echo n

let string = "abracadabra"

for l in string:
  echo l

for i, l in string:
  echo "Символ ", i, " → ", l

import std/unicode

let s = "Привет"

for c in s.runes:
  echo c
