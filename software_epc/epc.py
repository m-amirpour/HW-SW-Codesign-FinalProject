import numpy as np

def distance(x1, x2):
    return np.linalg.norm(x1 - x2)

class EPC:
    def __init__(self, fitness, dim, pop_size, lb, ub):
        self.fitness = fitness
        self.dim = dim
        self.pop_size = pop_size
        self.lb = np.array(lb)
        self.ub = np.array(ub)
        self.population = np.random.uniform(self.lb, self.ub, (pop_size, dim))
        self.fitness_vals = np.array([fitness(ind) for ind in self.population])
        self.best_idx = np.argmin(self.fitness_vals)
        self.best_pos = self.population[self.best_idx].copy()

    def step(self):
        # Find current best
        self.best_idx = np.argmin(self.fitness_vals)
        self.best_pos = self.population[self.best_idx].copy()

        new_population = self.population.copy()

        for i in range(self.pop_size):
            if i == self.best_idx:
                continue

            current = self.population[i]
            D = np.abs(self.best_pos - current)  # Distance vector to best

            # Spiral parameters (adjustable)
            l = np.random.uniform(-1, 1)  # Random for spiral shape
            b = 1.0  # Spiral constant
            spiral_factor = np.exp(b * l) * np.cos(2 * np.pi * l)

            # New position: spiral around best + decreasing random perturbation
            new_pos = self.best_pos + D * spiral_factor

            # Add exploration random (decreasing over time - track via external or param)
            random_strength = 0.5 * (1 - self.current_iter / self.max_iter) if hasattr(self, 'current_iter') else 0.1
            new_pos += random_strength * np.random.uniform(-1, 1, self.dim)

            new_pos = np.clip(new_pos, self.lb, self.ub)

            new_fit = self.fitness(new_pos)
            if new_fit < self.fitness_vals[i]:
                new_population[i] = new_pos

        self.population = new_population
        self.fitness_vals = np.array([self.fitness(ind) for ind in self.population])

    def run(self, iterations):
        self.max_iter = iterations
        history = [np.min(self.fitness_vals)]
        for t in range(iterations):
            self.current_iter = t
            self.step()
            history.append(np.min(self.fitness_vals))
        return self.best_pos, history