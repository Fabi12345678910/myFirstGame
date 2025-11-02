# Redesign
## Model(src/model/...)
Contains all data available about the current game(e.g Player(x,y,height,gender)). Does not offer any availability other than getting and setting values, no Dependencies to other things
### Gamestate Class
contains all GameObjects and others
### Stage
List of StageObject, spawnpoints etc.
### GameObject(Interface)
id, position, size, texture
### Collidable(Interface) :GameObject
### Movable(Interface) : GameObject
velocity
### StageObject :: GameObject
### Player :: Movable,Collidable
gravity, health, onGround
## Operations(src/operations/...)
No data, just operations(pure functions)
PlayerOperations.move(), PlayerOperations.jump(), PlayerOperations.applyGravity(), CollitionOperations etc.
## Drawing(src/rendering/...)
drawPlayer,draw Objects etc...
## EventHandler(src/eventHandler/...)
## InputTranslator(src/inputTranslator)
helper functions like Player& getPlayer(int playerId)
