# Networking

The interface provided by the networking classes is based on sending events.

## Basic Connection interface
all type connections do have this properties

provides:
- sending an event(maybe send multiple events later on)
- setting an event handler, which will implicitly be run on a new thread

## Client Connection
these are connection objects used by the client

they can be obtained by calling the Constructor of ClientConnection

additionally provide:
- nothing at the moment

## Server Connection
Server connections are objects used by the server to handle clients\
Server connections are used by the ServerSocket class and don't have to be used directly\

additionally provide:
- additional information about the connected client(e.g. playerID)

## Server Socket
A Server Socket can be obtained using the Constructor

The ServerSocket object by default will store all Connections made in an instance-member and provide multiple methods to interact:

- send an Event to all Connection
- send an Event to a Connection corresponding with a specific playerId
- set a default Event handler(this will be applied to all connections not having an event handler yet and all new connections)

## Events
see Events.md