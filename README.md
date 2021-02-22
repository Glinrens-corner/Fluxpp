# Fluxpp
An c++ gui framework inspired by Facebooks Flux architecture and React.js and Redux
![]( Fluxpp_architecture1.svg)

The Fluxpp framework has two user configurable parts: The State and the Widget Tree.

## The Widget Tree

In Fluxpp a Widget generally represents a rectangular area of screen space in which it can locate subwidgets.

Any user defined Widget consists of three main parts:
1. The Render function. This is a pure function which takes the state the widget depends upon and returns sub widgets and their positons within the current widget.
1. An array of EventHandlers translating any Gui-Events into app-events and deciding if the event should propagate to subwidgets.

