const m = 3

var a: array[m, char] = ['b', 'l', 'a']

echo a

var
  s: seq[int] = @[]   
  f = @["abc", "def"]  # сам угадывает, что это строки

s.add(8)
f.add("meow")

echo s[^1]
echo s.len
echo f[f.len - 1]

var t = (страна: "Россия", площадь: 17_000_000, класс_энергоэффективности: 'E')
echo t.страна
echo t.площадь
echo t.классэнергоэффективности

