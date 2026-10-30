import std/dom

let heading = document.createElement("h1")
heading.textContent = "Всем привет!"
document.body.appendChild(heading)
