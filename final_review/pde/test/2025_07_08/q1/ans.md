# Exercise 1 - Unsteady Stokes Problem with Reaction Term

## 1.1 [3 pt] Write the weak formulation of (1)

Let $V = [H^1_{\Gamma_D}(\Omega)]^2 = \{\mathbf{v} \in [H^1(\Omega)]^2 : \mathbf{v} = \mathbf{0} \text{ on } \Gamma_D\}$ and $Q = L^2(\Omega)$.

The weak formulation reads: For all $t \in (0, T)$, find $\mathbf{u}(t) \in V$ and $p(t) \in Q$ such that for all $\mathbf{v} \in V$ and $q \in Q$:

$$\int_\Omega \frac{\partial \mathbf{u}}{\partial t} \cdot \mathbf{v} \, d\mathbf{x} + \alpha \int_\Omega \mathbf{u} \cdot \mathbf{v} \, d\mathbf{x} + \mu \int_\Omega \nabla \mathbf{u} : \nabla \mathbf{v} \, d\mathbf{x} - \int_\Omega p \, \nabla \cdot \mathbf{v} \, d\mathbf{x} = \int_{\Gamma_N} \boldsymbol{\psi} \cdot \mathbf{v} \, d\sigma$$

$$\int_\Omega q \, \nabla \cdot \mathbf{u} \, d\mathbf{x} = 0$$

where the Neumann boundary condition $\mu \frac{\partial \mathbf{u}}{\partial \mathbf{n}} - p\mathbf{n} = \boldsymbol{\psi}$ has been naturally incorporated through integration by parts.

---

## 1.2 [1 pt] Write the spatial approximation of (1) using Taylor-Hood finite element spaces of degree 3 for velocity and 2 for pressure

Let $V_h \subset V$ and $Q_h \subset Q$ be the Taylor-Hood finite element spaces:
- Velocity space: $V_h = [X_h^3(\Omega)]^2 \cap V$ (piecewise cubic polynomials, i.e., P3)
- Pressure space: $Q_h = X_h^2(\Omega) \cap Q$ (piecewise quadratic polynomials, i.e., P2)

Let $\{\boldsymbol{\phi}_i\}_{i=1}^{N_u}$ be the basis functions for the velocity space and $\{\psi_j\}_{j=1}^{N_p}$ be those for the pressure space.

The semi-discrete problem reads: Find $\mathbf{u}_h(t) = \sum_{j=1}^{N_u} U_j(t) \boldsymbol{\phi}_j \in V_h$ and $p_h(t) = \sum_{j=1}^{N_p} P_j(t) \psi_j \in Q_h$ such that:

$$M \frac{d\mathbf{U}}{dt} + \alpha M \mathbf{U} + A \mathbf{U} + B^T \mathbf{P} = \mathbf{F}$$

$$B \mathbf{U} = \mathbf{0}$$

where:
- $M_{ij} = \int_\Omega \boldsymbol{\phi}_i \cdot \boldsymbol{\phi}_j \, d\mathbf{x}$ (velocity mass matrix)
- $A_{ij} = \mu \int_\Omega \nabla \boldsymbol{\phi}_i : \nabla \boldsymbol{\phi}_j \, d\mathbf{x}$ (velocity stiffness matrix)
- $B_{ij} = -\int_\Omega \psi_i \, \nabla \cdot \boldsymbol{\phi}_j \, d\mathbf{x}$ (divergence matrix)
- $F_i = \int_{\Gamma_N} \boldsymbol{\psi} \cdot \boldsymbol{\phi}_i \, d\sigma$ (right-hand side)

---

## 1.3 [2 pt] Approximate the problem obtained at point 1.2 by the implicit backward Euler method in time

Partition the time interval $(0, T]$ into $N_T$ subintervals with time step $\Delta t = T / N_T$.

Denote $\mathbf{U}^n \approx \mathbf{U}(t^n)$ and $\mathbf{P}^n \approx \mathbf{P}(t^n)$, where $t^n = n \Delta t$.

**Backward Euler discretization**:

$$M \frac{\mathbf{U}^{n+1} - \mathbf{U}^n}{\Delta t} + \alpha M \mathbf{U}^{n+1} + A \mathbf{U}^{n+1} + B^T \mathbf{P}^{n+1} = \mathbf{F}^{n+1}$$

$$B \mathbf{U}^{n+1} = \mathbf{0}$$

Rearranging into linear system form:

$$\begin{bmatrix} \frac{1}{\Delta t} M + \alpha M + A & B^T \\ B & 0 \end{bmatrix} \begin{bmatrix} \mathbf{U}^{n+1} \\ \mathbf{P}^{n+1} \end{bmatrix} = \begin{bmatrix} \frac{1}{\Delta t} M \mathbf{U}^n + \mathbf{F}^{n+1} \\ \mathbf{0} \end{bmatrix}$$

Or in compact notation:

$$\begin{bmatrix} \frac{1}{\Delta t} M + \alpha M + A & B^T \\ B & 0 \end{bmatrix} \begin{bmatrix} \mathbf{U}^{n+1} \\ \mathbf{P}^{n+1} \end{bmatrix} = \begin{bmatrix} \mathbf{b}^{n+1} \\ \mathbf{0} \end{bmatrix}$$

where $\mathbf{b}^{n+1} = \frac{1}{\Delta t} M \mathbf{U}^n + \mathbf{F}^{n+1}$.

---

## 1.4 [2 pt] Write the expected error estimate for the problem obtained in point 1.3 in terms of Δt and h

For Taylor-Hood P3/P2 elements combined with implicit Euler time discretization, the error estimates are:

**Velocity error**:
$$\|\mathbf{u}(T) - \mathbf{u}_h^{N_T}\|_{L^2(\Omega)} \leq C \left( \Delta t + h^4 \right)$$

$$\|\mathbf{u}(T) - \mathbf{u}_h^{N_T}\|_{H^1(\Omega)} \leq C \left( \Delta t + h^3 \right)$$

**Pressure error**:
$$\|p(T) - p_h^{N_T}\|_{L^2(\Omega)} \leq C \left( \Delta t + h^3 \right)$$

**Explanation**:
- **Time error**: Backward Euler is a first-order method, hence error is $O(\Delta t)$
- **Spatial error**:
  - Velocity uses P3 elements: $L^2$ error is $O(h^{3+1}) = O(h^4)$, $H^1$ error is $O(h^3)$
  - Pressure uses P2 elements: $L^2$ error is $O(h^{2+1}) = O(h^3)$

The total error is dominated by the larger of the temporal and spatial errors.

---

## 1.7 [2 pt] Now set α = 0 and solve the problem again, using finite elements of degree 1 for both velocity and pressure. Upload a plot of the solution, and discuss the results in light of the theory.

When $\alpha = 0$ and both velocity and pressure use P1 elements, this violates the **inf-sup (LBB) stability condition**.

**Theoretical analysis**:
- Taylor-Hood elements require the velocity degree to be one higher than the pressure degree (e.g., P2-P1 or P3-P2)
- The P1-P1 combination **does not satisfy** the inf-sup condition
- This leads to:
  1. **Pressure oscillations** (spurious pressure oscillations / checkerboard pattern)
  2. **Inaccurate velocity field**
  3. **Solution may not converge** or converge to the wrong solution

**Expected observations**:
- The pressure field will exhibit non-physical checkerboard oscillation patterns
- The velocity field may show spurious vortices or lack smoothness
- Convergence rate will decrease or fail completely

**Conclusion**: This verifies the importance of the inf-sup condition for stability in mixed problems in finite element theory. P1-P1 elements are not suitable for Stokes/Navier-Stokes problems.

---

## 1.8 [2 pt] Describe the preconditioning strategy you used to solve the linear system at each time step. How does the number of GMRES iterations change between timesteps? Justify it in light of the observed solution.

**Preconditioning strategy**:

We use a block-triangular preconditioner:
$$P = \begin{bmatrix} \frac{1}{\Delta t} M + \alpha M + A & 0 \\ B & -\frac{1}{\mu} M_p \end{bmatrix}$$

where $M_p$ is the pressure mass matrix with entries $(M_p)_{ij} = \int_\Omega \psi_i \psi_j \, d\mathbf{x}$.

**Application of the preconditioner**:
$$P^{-1} = \begin{bmatrix} \left(\frac{1}{\Delta t} M + \alpha M + A\right)^{-1} & 0 \\ \mu M_p^{-1} B \left(\frac{1}{\Delta t} M + \alpha M + A\right)^{-1} & -\mu M_p^{-1} \end{bmatrix}$$

In practice, the inner inverses are approximated using CG iterations with ILU preconditioning.

**GMRES iteration count behavior**:

1. **Initial time steps**: Higher iteration count
   - Initial condition $\mathbf{u} = \mathbf{0}$ differs significantly from the steady state
   - Large gradients in the solution

2. **Middle time steps**: Gradually decreasing iteration count
   - Solution progressively approaches steady state
   - Previous time step solution serves as a good initial guess

3. **Near steady state**: Lowest iteration count
   - $\mathbf{U}^{n+1} \approx \mathbf{U}^n$
   - System matrix condition number remains stable

**Physical justification**: When $\alpha > 0$, the system has a damping term, causing the solution to decay and approach steady state. As the solution approaches steady state, the change between time steps decreases, making the linear system easier to solve.