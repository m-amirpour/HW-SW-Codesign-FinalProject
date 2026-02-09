import numpy as np

def distance(x1, x2):
    return np.linalg.norm(x1 - x2)

def random_dim_pairs(dim):

    indices = np.arange(dim)
    np.random.shuffle(indices)

    pairs = []
    leftover = None

    for i in range(0, dim - 1, 2):
        pairs.append((indices[i], indices[i+1]))

    if dim % 2 == 1:
        leftover = indices[-1]

    return pairs, leftover

def get_new_position(current_pair, best_pair, Q, b, a):

    (x_i, y_i) = current_pair
    (x_j, y_j) = best_pair

    theta_i = np.arctan2(y_i, x_i)
    theta_j = np.arctan2(y_j, x_j)

    # Correct EPC weighting
    theta_k = (1/b) * np.log(
        (1 - Q) * np.exp(b * theta_i) +
        Q * np.exp(b * theta_j)
    )

    r_k = a * np.exp(b * theta_k)

    x_k = r_k * np.cos(theta_k)
    y_k = r_k * np.sin(theta_k)

    return x_k, y_k

def get_adjusted_position(currentPosition, best_position, Q, b, a):

    dim = len(currentPosition)
    pairs, leftover = random_dim_pairs(dim)

    new_position = currentPosition.copy()

    for (i, j) in pairs:

        new_x, new_y = get_new_position(
            (currentPosition[i], currentPosition[j]),
            (best_position[i], best_position[j]),
            Q, b, a
        )

        new_position[i] = new_x
        new_position[j] = new_y

    # Handle leftover dimension linearly
    if leftover is not None:
        k = leftover
        new_position[k] = (
            currentPosition[k] +
            Q * (best_position[k] - currentPosition[k])
        )

    return new_position


class EPC:

    def __init__(self, benchmark, dim, pop_size, lb, ub, a, b):

        self.fitness = benchmark
        self.dim = dim
        self.pop_size = pop_size

        # Ensure bounds are vectors
        self.lb = np.full(dim, lb) if np.isscalar(lb) else np.array(lb)
        self.ub = np.full(dim, ub) if np.isscalar(ub) else np.array(ub)

        self.a = a
        self.b = b

        self.penguinPosition = np.random.uniform(
            self.lb, self.ub, (pop_size, dim)
        )

        self.fitness_vals = np.array(
            [benchmark(ind) for ind in self.penguinPosition]
        )


    # Heat attenuation
    def heat(self, D):

        baseU = 0.05

        # linear decay
        u = baseU * (1 - (self.current_iter / self.max_iter))

        # quadratic decay
        u = baseU * (1 - (self.current_iter / self.max_iter)) ** 2

        return np.exp(-u * D)


    def step(self):

        best_idx = np.argmin(self.fitness_vals)
        best_position = self.penguinPosition[best_idx].copy()

        newPositions = self.penguinPosition.copy()

        for i in range(self.pop_size):

            if i == best_idx:
                continue

            current = self.penguinPosition[i]

            D = distance(best_position, current)
            Q = self.heat(D)

            new_pos = get_adjusted_position(
                current, best_position, Q, self.b, self.a
            )

            # Exploration noise
            exploration_rate = 0.5 * (1 - self.current_iter / self.max_iter)

            noise = np.random.uniform(-1, 1, self.dim) * \
                    (self.ub - self.lb) * exploration_rate

            new_pos += noise

            new_pos = np.clip(new_pos, self.lb, self.ub)

            newPositions[i] = new_pos

        self.penguinPosition = newPositions

        self.fitness_vals = np.array(
            [self.fitness(ind) for ind in self.penguinPosition]
        )


    def run(self, iterations):

        self.max_iter = iterations
        history = [np.min(self.fitness_vals)]

        for t in range(iterations):
            self.current_iter = t
            self.step()
            history.append(np.min(self.fitness_vals))

        best_idx = np.argmin(self.fitness_vals)
        best_position = self.penguinPosition[best_idx]

        return best_position, history
