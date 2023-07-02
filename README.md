# c_ugui
Framework-agnostic immediate-mode GUI library written in C

# Architecture

The library itself is only a state machine, the rendering is performed by the user.

I provide one built-in renderer using `raylib`, which should suffice for most use-cases.
