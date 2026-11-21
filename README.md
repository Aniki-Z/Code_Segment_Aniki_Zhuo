# Zijie (Aniki) Zhuo | Gameplay Systems & Technical Design

I build gameplay systems and prototype the choices they give players. Start with two Unity C# code samples, or try a playable build of Sumo Slam.

| Start here | What to look for |
| --- | --- |
| [Player systems: movement and dash](Segment/Runtime/Player/PlayerController.cs) | A Unity controller switches between idle, movement, dash, and stun. See how the dash cooldown limits when the player can use it again. |
| [Item systems: what fits in the inventory?](Segment/Runtime/Inventory/Inventory.cs) | When a player picks up a stack, the inventory fills matching slots first, then empty ones. It returns the quantity that actually fit. |
| [Sumo Slam: playable builds](SumoSlam/) | A two-person Unity 2D fighting game. I designed combat abilities, implemented hit detection, and added visual and audio feedback. This folder contains the Windows and macOS builds. |

[Randomazing: full source and controls](Randomazing/README.md) · [Hamster Mapster: project and my role](https://www.anikiz.com/hamster-mapster) · [Portfolio](https://www.anikiz.com/)