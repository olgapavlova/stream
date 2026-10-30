import strutils, std/unicode, sequtils

let content = readFile("my_file.txt").strip()
let lines = content.splitLines()
echo lines

echo "Введите следующую строку:"
let next_line = readLine(stdin)
echo "Вот что вы ввели: ", next_line

echo lines.mapIt(it.runeLen)
