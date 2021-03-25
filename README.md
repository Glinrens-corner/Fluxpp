# Fluxpp
A c++ ui framework inspired by Facebooks Flux architecture as well as React.js and Redux

It focuses on composability, debugability and dynamizism. 
Perfomance is a secondary concern.

## General Idea and Architecture

The general idea is that a widget (normally) represents a rectangular area of the screen. 
Internally it is programmed as a function that 'returns' all the widgets which subdive the area of the parent.
The widget can also subscribe to a piece of state ( a slice) and set event handler for gui_events.
The event_handler translates gui_events into application events which are dispatched to a state slice.
The state slice is then updated through a user-supplied reducer function and the rerenders all widgets which were subscribed to this slice.

![]( Fluxpp_architecture1.svg)

