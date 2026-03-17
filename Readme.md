# Logistic Map, Inverse Cobweb Diagram and Controlled Chaos (simplified)

https://youtu.be/06u9cFxS_h0

## Logistic Function

Let $f(x)$ be defined as

$f(x) = r x (1 - x)$

where:

- $x \in [0,1]$
- $r \in [0,4]$

Function $f(x)$ is an endofunction on the interval [0,1]

## Inverse Logistic Function

The inverse function of $y = f(x)$ is:

$f^{-1}(y) = \frac{1}{2}\left(1\pm\sqrt{1-4\frac{y}{r}}\right)$

## Intersection points

For $0 \leq r \le 1$ there is only one intersection point 
- $X_0 = 0$

For $1 < r \le 3$ there are two intersection points:

- $X_0 = 0$
- $X_1 = \frac{r-1}{r}$

For $3 < r \le 4$ there are four intersection points:

- $X_0 = 0$
- $X_1 = \frac{r-1}{r}$
- $X_2 = \frac{1}{2r}(r+1 - \sqrt{r^2 - 2r - 3})$
- $X_3 = \frac{1}{2r}(r+1 + \sqrt{r^2 - 2r - 3})$

## Iteration sequence

If a procedure repeatedly feeds the output back into the function, the operation internally alternates between $f(x)$ and $f^{-1}(x)$, i.e.

$$
x_{n+1} =
\begin{cases}
f(x_n), & \text{if } n = 2k,\ k \in W \\
f^{-1}(x_n), & \text{if } n = 2k + 1,\ k \in W
\end{cases}
$$

For $x_0 \in [0,1]$:

$x_1 = f(x_0)$

$x_2 = f^{-1}(x_1) = f^{-1}(f(x_0))$ 

$x_3 = f(x_2) = f(f^{-1}(f(x_0)))$

$x_4 = f^{-1}(x_3) = f^{-1}(f(f^{-1}(f(x_0))))$

For the Logistic function the sequence becomes:

$$
x_{n+1} =
\begin{cases}
r x_n (1 - x_n), & \text{if } n = 2k,\ k \in W \\
\frac{1}{2}\left(1 \pm \sqrt{1 - \frac{4x_n}{r}}\right), & \text{if } n = 2k + 1,\ k \in W
\end{cases}
$$

## Inverse Cobweb Diagram

A Cobweb Diagram is a geometric way to visualize repeated function iteration. 

It usually shows how values move ("bounce") between the function curve and the diagonal line $y = x$

Instead of using diagonal line $y = x$, let's use inverse function curve instead:

$y = \frac{1}{2}\left(1\pm\sqrt{1-4\frac{x}{r}}\right)$

And watch how values bounce between $f(x)$ and $f^{-1}(x)$

## Stabile Iteration Cycle with three nodes
For certain value of r (r-oscillate, $r_o$) the iteration sequence "oscillates" between three values.
To calculate $r_o$ we need to solve the two equations:

eq 1: $f(x) = 1/2$

It has two solutions for r>2:
$x_{o1} = \frac{r-\sqrt{(r-2)r}}{2r}$
$x_{o2} = \frac{r+\sqrt{(r-2)r}}{2r}$

eq 2: $f^{-1}(x_{o1}) = f(1/2)$

Solving by r, we get $r_o$

$r_o = 1 + \sqrt{1 + \frac{1}{3}\left(8 + (800 - 96\sqrt{69})^{1/3} + 2 \cdot 2^{2/3}(25 + 3\sqrt{69})^{1/3}\right)}$

$r_o \approx 3.8318740552833155684103627754961065557978278526036946304788904477$

## Bifurcation diagram for $f(x) = rx(1-x)$
## Bifurcation diagram for $f(x) = \frac{r}{4} sin (\pi x)$

