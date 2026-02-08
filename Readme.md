# Jello on a Train: 3D Physics Simulation

A real-time 3D physics simulation built in **C++** using **openFrameworks**. The project demonstrates complex physical interactions between a soft-body "Jello" mass and a moving train car traversing procedurally generated terrain.

## 📽 Demo Video
[![Jello Physics Demo](https://img.youtube.com/vi/_RVCY_j9rXw/maxresdefault.jpg)](https://youtu.be/_RVCY_j9rXw)
*(Click the image to play the video on YouTube)*

## 🛠 Technical Implementation

### Soft-Body Physics (Mass-Spring System)
The "Jello" is modeled as a 4x4x4 grid of 64 individual physics particles.
* **Mass-Spring Network**: Each particle is connected to up to 13 neighbors through a network of springs governed by **Hooke's Law** ($F = -k \Delta x$).
* **Numerical Integration**: Particle motion is computed using **Euler Integration**, where acceleration is derived from net force ($a = F/m$), subsequently updating velocity and position over discrete time steps.
* **Damping & Energy Loss**: To maintain simulation stability and prevent infinite oscillation, the system implements an exponential damping factor on particle velocity.

### Procedural Terrain & Dynamics
* **Perlin Noise Terrain**: The environment features an infinite, procedurally generated slope created using `ofNoise` for smooth, natural-looking hills.
* **Normal Force & Friction**: The train car maintains contact with the terrain by calculating surface normals from slope tangents, allowing for realistic gravity and normal force application.
* **Constraint Satisfaction**: The simulation ensures the Jello remains anchored to the train deck while responding to the car's acceleration and the external influence of gravity ($9.81 m/s^2$).

### Interactive Features
* **Cinematic Camera**: A dynamic camera system that can toggle between a fixed "free" view and a cinematic "follow" mode that tracks the train's movement.
* **Real-time Interaction**: Users can apply manual forces to the train or induce "jiggle" effects on the Jello particles through direct keyboard input.

## ⌨️ Controls
| Key | Action |
| :--- | :--- |
| **A / D** | Move the Train car Left or Right |
| **C** | Jiggle the Jello (Vertical Squish) |
| **R** | Jiggle the Jello (Randomized Force) |
| **F** | Toggle Camera Mode (Cinematic vs. Free) |

## 📦 Build Requirements
* **Environment**: Developed in Visual Studio 2022.
* **Framework**: Requires [openFrameworks v0.12.1+](https://openframeworks.cc/).

* **Libraries**: Standard openFrameworks core libraries and GLM for vector mathematics.
