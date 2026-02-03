import numpy as np

def distance(x1, x2):
    return np.linalg.norm(x1 - x2)

def spiral_move(xi, xj, Q, a=1, b=0.5):

    dim = len(xi)
    x_new = np.copy(xi)

    if dim >= 2:
        theta_i = np.arctan2(xi[1], xi[0])
        theta_j = np.arctan2(xj[1], xj[0])

        theta_k = (1/b) * np.log(
            (1-Q)*np.exp(b*theta_i) + Q*np.exp(b*theta_j)
        )

        r_k = a * np.exp(b * theta_k)

        x_new[0] = r_k * np.cos(theta_k)
        x_new[1] = r_k * np.sin(theta_k)

    if dim > 2:
        x_new[2:] = xi[2:] + Q*(xj[2:] - xi[2:])

    return x_new


class EPC:

    def __init__(self, fitness, dim, pop_size, lb, ub):

        self.fitness = fitness
        self.dim = dim
        self.pop_size = pop_size
        self.lb = lb
        self.ub = ub

        self.mu = 0.05
        self.m = 0.5

        self.population = np.random.uniform(lb, ub, (pop_size, dim))
        self.fitness_vals = np.array([fitness(p) for p in self.population])

    def step(self):

        for i in range(self.pop_size):
            for j in range(self.pop_size):

                if self.fitness_vals[j] < self.fitness_vals[i]:

                    dij = distance(self.population[i], self.population[j])
                    Q = np.exp(-self.mu * dij)

                    new_pos = spiral_move(
                        self.population[i],
                        self.population[j],
                        Q
                    )

                    new_pos += self.m * np.random.uniform(-1,1,self.dim)
                    new_pos = np.clip(new_pos, self.lb, self.ub)

                    new_fit = self.fitness(new_pos)

                    if new_fit < self.fitness_vals[i]:
                        self.population[i] = new_pos
                        self.fitness_vals[i] = new_fit

        self.mu *= 0.99
        self.m *= 0.99

    def run(self, iterations):

        history = []

        for _ in range(iterations):
            self.step()
            history.append(np.min(self.fitness_vals))

        best_idx = np.argmin(self.fitness_vals)
        return self.population[best_idx], history
