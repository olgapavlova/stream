var counter = 1

proc inc_counter() =
  echo counter
  counter += 1

proc show_counter(cnt: int) =
  echo cnt

proc show_and_inc_counter(cnt: var int) =
  echo cnt
  inc cnt

inc_counter()
inc_counter()
inc_counter()
echo ""

show_counter(counter)
show_counter(counter)
echo ""

show_and_inc_counter(counter)
show_and_inc_counter(counter)


proc deg(ex: var int, d: int) = 
  var mult = ex
  for i in 1 ..< d:
    ex *= mult

var e = 7
e.deg(5)
echo e 
