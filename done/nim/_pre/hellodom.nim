import std/dom

let heading = document.createElement("h1")
heading.textContent = "Hello from Nim!"
document.body.appendChild(heading)
