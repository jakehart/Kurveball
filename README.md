# CurveLib
CurveLib is a designer-driven root motion library, used primarily for the development of character movement and parkour mechanics. It is currently under development, with all basic features already implemented. I am now focusing on details and engine wrappers.

# How to Use in Unreal Engine
Just put CurveLib under your Source directory and add VelocityCurveComponent to your actor. This wrapper automatically applies the velocity curves' position and rotation to your actor, making your life easier. With the component added, you can call all of the API functions (like StartVelocityCurve) directly from Unreal Blueprint.

Wrappers for other engines are on the roadmap! Godot is next in line after Unreal.

# Philosophy
Each movement mechanic is represented as a graph of speed over time: x is time (from left to right), and y is speed. Each curve can be looped, time-stretched, speed-stretched, 

# Platforms and Engines
CurveLib is completely platform-agnostic. It doesn't care which axis is up, what the world units are, or how your engine works. Just be consistent with your units and axes, and you'll be off to the races.
