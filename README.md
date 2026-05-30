# Möbius

[![arXiv](https://img.shields.io/badge/arXiv-2605.29905-b31b1b.svg)](https://arxiv.org/abs/2605.29905)

A C++ library and collection of interactive visualizations for **Möbius (inversive) geometry** in the plane. The same algebraic model represents Euclidean lines and circles, hyperbolic geodesics in the Poincaré disk, triangle centers, and classical incidence theorems—so you can drag vertices, change angles, and watch bisectors, incircles, and Ceva ratios update in real time.

The project pairs a small computational core (`moebius` static library) with GLUT/GLUI demos. Companion Jupyter notebooks explore related formulas symbolically.

## Companion paper

This code accompanies the paper

> **Ivan Livinsky.** *On Ceva's and Menelaus's Theorems for a Möbius triangle.* arXiv preprint, 2026. [arXiv:2605.29905](https://arxiv.org/abs/2605.29905)

The paper generalizes the classical Ceva and Menelaus theorems to curvilinear triangles bounded by circular arcs, introduces trilinear coordinates for such triangles, and defines the incenter, excenters, and orthocenter for any proper Möbius triangle. The demos in this repository illustrate those constructions interactively—see the `ceva*`, `menelaus`, `triangle`, and `proper` examples in particular.

If you use this code in academic work, please cite the paper:

```bibtex
@misc{livinsky2026moebius,
  author       = {Ivan Livinsky},
  title        = {On Ceva's and Menelaus's Theorems for a M\"obius triangle},
  year         = {2026},
  eprint       = {2605.29905},
  archivePrefix= {arXiv},
  primaryClass = {math.MG},
  url          = {https://arxiv.org/abs/2605.29905}
}
```

---

## Mathematical model

### Cycles

The basic object is an oriented **cycle**: a line or circle given in unified form

$$
a(x^2 + y^2) + bx + cy + d = 0
$$

with $a = 0$ for lines. The implementation stores \((a,b,c,d)\), center \(O\), radius \(R\), and orientation. Cycles can be inverted in one another, intersected (`X ^ Y`), and compared via Möbius invariants (`sin`, `cos`, and `operator*` between cycles).

### Triangles

A `Triangle` is defined by three vertices and three **prescribed angles** $\alpha, \beta, \gamma$ (not necessarily the Euclidean angles of the drawn vertices). On each `recompute()` the library:

1. Reads Euclidean side lengths and orientation from the vertex positions.
2. Solves for auxiliary angles $a_0, b_0, c_0$ and oriented side cycles `aa`, `bb`, `cc`.
3. Builds $\omega$-cycles, pseudo-altitude feet $H_a, H_b, H_c$, and incircle / excircle families.

Set `hyperbolic = true` in the constructor to use hyperbolic side cycles and angle formulas instead of the Euclidean Möbius triangle model.

### Digons

A `Digon` is a two-vertex figure with a prescribed angle between its sides—useful as a minimal building block before full triangles.

---

## Screenshots and interaction

Each demo opens a main graphics window and a **GLUI** control panel. Common controls:

| Input | Action |
|--------|--------|
| **Left drag** on a vertex | Move the point |
| **Mouse wheel** on a vertex (where implemented) | Adjust the angle at that vertex |
| **Checkboxes** in GLUI | Toggle bisectors, altitudes, incircle, etc. |
| **`q` / Esc** | Quit |

Vertices highlight in red when the cursor is near them. Coordinates use a bottom-left origin (`gluOrtho2D`); mouse handling flips the GLUT \(y\)-axis where needed.

---

## Building

### Requirements

- CMake ≥ 3.10
- C++17 compiler (GCC or Clang)
- OpenGL and GLUT (e.g. `freeglut3-dev` on Debian/Ubuntu)
- [GLUI](https://github.com/libglui/glui) — headers and `libglui`

On Ubuntu/Debian:

```bash
sudo apt install build-essential cmake libgl1-mesa-dev freeglut3-dev libglui-dev
```

If GLUI is installed in a non-standard prefix, set `CMAKE_PREFIX_PATH` or adjust `cmake/FindGLUI.cmake`.

### Compile

```bash
git clone <repository-url>
cd Moebius
mkdir -p build && cd build
cmake ..
cmake --build .
```

All example binaries are written to `build/examples/`.

### Run

```bash
./examples/triangle
./examples/ceva
./examples/hyperbolic
```

---

## Interactive demos

Demos are standalone executables; add new ones by dropping a `.cpp` file into `examples/` (CMake picks it up automatically).

### Triangles and triangle geometry

| Program | Focus |
|---------|--------|
| **`digon`** | Digon between two points; adjustable opening angle and bisector. |
| **`triangle`** | General Möbius triangle: drag vertices, scroll to change $\alpha,\beta,\gamma$, toggle bisectors, altitudes, pseudoaltitudes, circumcircle, incircle, excircles, Euler circle. |
| **`hyperbolic`** | Hyperbolic triangle: Poincaré model sides, pseudoaltitudes, hyperbolic incircle/excircles. |
| **`euclidean`** | Classical Euclidean triangle toolbox: medians, Lemoine point, Toricelli (isogonic) points, Kiepert hyperbola, Apollonius (isodynamic) points, Brocard axis, plus standard circles. |
| **`ceva`** | Ceva’s theorem with adjustable barycentric weights $\lambda,\mu,\nu$; optional hyperbolic mode. |
| **`menelaus`** | Menelaus’ theorem; hyperbolic toggle. |
| **`isogonal`** | Isogonal conjugation: triangle plus a fourth point \(P\), with bisectors and circumcircle. |

---

## Library layout

```
Moebius/
├── inc/geometry.h      # Point, Vector, Cycle, Triangle, Digon
├── src/
│   ├── geometry.cpp    # Intersections, Apollonius, splits, triangle logic
│   └── draw.cpp        # OpenGL drawing, arc tessellation, TikZ export helpers
├── examples/*.cpp      # One executable per file
├── cmake/FindGLUI.cmake
└── *.ipynb             # Symbolic notebooks (not required to build)
```

### Linking against `moebius`

The static library is built automatically. An example target is equivalent to:

```cmake
target_link_libraries(my_app moebius ${GLUI_LIBRARIES} OpenGL::GL OpenGL::GLU GLUT::GLUT)
```

Include `inc/` and call `Triangle::recompute()` before any `draw_*()` method.

---

## Design notes

- **Angles vs. positions.** Vertex positions fix the Euclidean shape; \(\alpha,\beta,\gamma\) impose the Möbius (or hyperbolic) angle data used to place oriented side cycles. That separation is what lets the demos explore non-Euclidean angle assignments while still drawing in the plane.
- **Orientation.** `Orientation` (`ABC` / `ACB`) tracks winding; side cycles and fills in `draw_body()` depend on it.
- **New demos.** Copy `examples/triangle.cpp`, link against `moebius`, and use external `bool` flags with `draw_*()` toggles rather than storing display state on `Triangle`.

---

## License

No license file is included in the repository yet. If you intend to distribute or reuse the code, add a `LICENSE` file and state terms here.
