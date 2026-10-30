import strutils, sequtils
import std/unicode

let content = readFile("my_file.txt").strip()
let lines = content.splitLines()
echo lines

let lenlines = lines.mapIt(it.runeLen)
echo lenlines
