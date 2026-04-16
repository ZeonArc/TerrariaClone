<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=timeGradient&height=200&section=header&text=TerrariaClone-CPP&fontSize=50&fontAlignY=35&desc=Custom%202D%20Voxel%20Engine&descAlignY=55&descSize=20" alt="Header Image">

  <p>
    <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=for-the-badge&logo=c%2B%2B" alt="C++17">
    <img src="https://img.shields.io/badge/OpenGL-3.3_Core-5586A4.svg?style=for-the-badge&logo=opengl" alt="OpenGL 3.3">
    <a href="https://github.com/YOUR_USERNAME/TerrariaClone-CPP/blob/main/LICENSE">
      <img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge" alt="License: MIT">
    </a>
    <img src="https://img.shields.io/badge/Platform-Windows-0078d7.svg?style=for-the-badge&logo=windows" alt="Windows">
    <img src="https://img.shields.io/badge/Status-Active_Development-brightgreen.svg?style=for-the-badge" alt="Status">
  </p>
</div>

> A custom-built, high-performance 2D voxel game engine written completely from scratch in C++ and Modern OpenGL.

## 📖 About The Project

Rather than using an off-the-shelf engine, this project was developed from the ground up to explore **low-level graphics programming**, **memory management**, and **game engine architecture**. It serves as a deep dive into the underlying mechanics of game development, focusing on high-performance rendering and systems-level design.

It features an infinite procedurally generated world, a custom physics simulation, and a highly optimized chunk-based rendering pipeline.

## ✨ Engine Features

* **Custom Batch Renderer:** Uses Vertex Array Objects (VAOs) and VBOs to pack thousands of tile vertices into a single draw call, effortlessly maintaining 120+ FPS.
* **Texture Atlasing:** Dynamically calculates UV coordinates to slice and map sprites from a master sprite sheet, minimizing texture bindings.
* **Infinite Procedural Generation:** Integrates FastNoiseLite using Fractal Brownian Motion (FBM) for rolling surface hills and Ridged OpenSimplex2 noise for interconnected underground cave systems.
* **AABB Physics & Collision:** A bespoke 2D physics loop featuring axis-separated collision detection, kinematic gravity, and anti-snagging epsilons for fluid, responsive player movement.
* **World Interaction:** Dynamic Screen-to-World coordinate matrix translation allowing real-time modification of the chunk mesh (mining and placing blocks).
* **Chunk Memory Management:** A dynamic World manager that generates terrain based on the camera's X-coordinate and safely unloads distant chunks to prevent memory leaks.

## 🛠️ Tech Stack & Dependencies

This engine was built with a lightweight, carefully selected tech stack to maximize performance and developer control:

| Technology | Description |
| :--- | :--- |
| **Language** | C++ |
| **Graphics API** | OpenGL 3.3 Core Profile |
| **Mathematics** | GLM (OpenGL Mathematics) |
| **Windowing & Input**| GLFW |
| **Extension Loader** | GLAD |
| **Texture Loading** | `stb_image` |
| **Procedural Noise** | FastNoiseLite |

## 🎮 Controls

| Action | Input / Key |
| :--- | :--- |
| **Move Left / Right** | <kbd>A</kbd> / <kbd>D</kbd> |
| **Jump** | <kbd>SPACE</kbd> |
| **Mine Block** *(Replace with Air)* | <kbd>Left Click</kbd> |
| **Place Block** *(Wood)* | <kbd>Right Click</kbd> |
| **Toggle Wireframe** *(Debug Mode)*| <kbd>F1</kbd> |
| **Exit Game** | <kbd>ESC</kbd> |

## 🚀 Getting Started

To run this project locally on your Windows machine, follow these steps:

### Prerequisites
* **Visual Studio** (2019 or 2022 recommended)
* C++ Desktop Development Workload installed

### Installation & Build
1. **Clone the repository:**
   ```bash
   git clone [https://www.github.com/ZeonArc/TerrariaClone](https://github.com/ZeonArc/TerrariaClone.git)
