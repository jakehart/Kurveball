# ╭╰ Kurveball ╭╰

<iframe width="1030" height="579" src="https://www.youtube.com/embed/Nun1smLHx3c" title="Kurveball jump example" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

Kurveball is a curve-driven movement and animation library designed to replace complex imperative movement code with intuitive visual graphs. 

Instead of writing code to create movement, you define a velocity curve: a simple graph where the X-axis is Time and the Y-axis is Speed. Kurveball reads this graph, performs the necessary calculus integration, and drives your character's position and rotation automatically. Motion can be looped, time-stretched, speed-stretched, bound to 3D splines, masked by axis, and more.

## 🪶 Philosophy

Character movement is a complex math problem, but *designers* should see it as a visual art. They should be able to tweak a jump, dash, or slide by shaping a curve, *not* by memorizing dozens of physics parameters or changing numbers by trial and error.

---

## 📐 How to Use in Unreal Engine

Just put Kurveball under your Source directory and add VelocityCurveComponent to your actor. This wrapper automatically applies the velocity curves' position and rotation to your actor. With the component added, you can call all of the API functions (like StartVelocityCurve) directly from Unreal Blueprint. Kurveball is compatible with Unreal's CurveFloat type, so you can use Unreal's native editor to draw your velocity curves. No conversion needed.

Wrappers for other engines are on the roadmap! Godot is next in line after Unreal.

---

## 🔩 Other Platforms and Engines

Kurveball is not dependent on any specific platform or engine. It doesn't care which axis is up, what the world units are, or how your engine works. Just be consistent with your units and axes, start your mechanic with `Kurveball::StartVelocityCurve()`, and call `Kurveball::TickPlayback()`. The library handles the rest, storing the result in `VelocityCurveContext`.

---

## 📦 Architecture & Extensibility

Kurveball is split into two layers:
1.  **Core (`Kurveball` namespace):** Pure C++ templates. **No dependencies on any specific engine.** Handles math, Bezier curves, integration, and data structures.
2.  **Wrappers:** Engine-specific adapters. Translate engine types (`FVector`, `UCurveFloat`) into Core types.

**Adding a New Engine:**
To port to a custom engine:
1.  `#include Kurveball/Source/KurveballAll.h`
2.  Implement a wrapper for `VelocityCurveContext`.
3.  Map your engine's vector types to `Kurveball::Float3`.
4.  Implement a `CurveSampler` for your engine's curve asset type.

---

## 📄 License
MIT Non-AI License. Copyright (c) 2026 Jake Hart.

This means that you may *not* use this code to *train* AI, or in any form of AI research. However, you *are* allowed to use the code in game projects that *use* AI. See LICENSE.md for details.

---

## 🤝 Contributing
If you find bugs, have ideas for new features, or want to help build a wrapper for your favorite engine, issues and PRs are welcome!

---

### 💡 Why use Kurveball?
*   **For Designers:** You'll be able to create movement and camera mechanics independently and without intervention from engineers. Iterate on movement feel in seconds by shaping a curve.
*   **For Programmers:** You'll be freed from having to implement code for individual character movement mechanics or cameras.
*   **For Artists:** Create precise, reproducible animations that sync perfectly with gameplay.