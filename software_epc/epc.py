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
        self.penguinPosition = np.random.uniform(self.lb, self.ub, (pop_size, dim))
        self.fitness_vals = np.array([fitness(ind) for ind in self.penguinPosition])
        self.best_position_idx = np.argmin(self.fitness_vals)
        self.best_position = self.penguinPosition[self.best_position_idx].copy()

    def step(self):
        self.best_position_idx = np.argmin(self.fitness_vals)
        self.best_position = self.penguinPosition[self.best_position_idx].copy()

        newPenguinPositions = self.penguinPosition.copy()

        for i in range(self.pop_size):
            if i == self.best_position_idx:
                continue

            currentPosition = self.penguinPosition[i]
            D = np.abs(self.best_position - currentPosition)  

            l = np.random.uniform(-1, 1)
            b = 1.0 
            spiral_factor = np.exp(b * l) * np.cos(2 * np.pi * l)

            new_pos = self.best_position + D * spiral_factor

            random_strength = 0.5 * (1 - self.current_iter / self.max_iter) if hasattr(self, 'current_iter') else 0.1
            new_pos += random_strength * np.random.uniform(-1, 1, self.dim)

            new_pos = np.clip(new_pos, self.lb, self.ub)

            new_fit = self.fitness(new_pos)
            current_fit = self.fitness_vals[i]
            newPenguinPositions[i] = new_pos

        self.penguinPosition = newPenguinPositions
        self.fitness_vals = np.array([self.fitness(ind) for ind in self.penguinPosition])

    def run(self, iterations):
        self.max_iter = iterations
        history = [np.min(self.fitness_vals)]
        for t in range(iterations):
            self.current_iter = t
            self.step()
            history.append(np.min(self.fitness_vals))
        return self.best_position, history