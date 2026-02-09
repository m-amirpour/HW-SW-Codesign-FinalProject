import numpy as np

def distance(x1, x2):
    return np.linalg.norm(x1 - x2)

def random_dim_pairs(dim):
    indices = np.arange(dim)
    np.random.shuffle(indices)
    
    pairs = []
    for i in range(0, dim - 1, 2):
        pairs.append((indices[i], indices[i+1]))
    
    return pairs


def get_adjusted_position(currentPosition, best_position, Q,b,a):
    dim = len(currentPosition)
    pairs = random_dim_pairs(dim)
    current_value_pairs = []
    best_value_pairs = []
    for (i, j) in pairs:
        current_value_pairs.append((currentPosition[i], currentPosition[j]))
        best_value_pairs.append((best_position[i], best_position[j]))
    new_dim_pairs = []
    for (x_i,y_i), (x_j, y_j) in zip(current_value_pairs, best_value_pairs):
        new_x, new_y = get_new_position((x_i, y_i), (x_j, y_j), Q,b,a)
        new_dim_pairs.append((new_x, new_y))
    new_position = currentPosition.copy()
    for (i, j), (new_x, new_y) in zip(pairs, new_dim_pairs):
        new_position[i] = new_x
        new_position[j] = new_y
    return new_position

def get_new_position(current_position_pair, best_position_pair, Q,b,a):
    (x_i, y_i) = current_position_pair
    (x_j, y_j) = best_position_pair
    theta_i = np.arctan2(y_i, x_i)
    theta_j = np.arctan2(y_j, x_j)
    theta_k = (1/b) * np.log((1 - Q) * np.exp(b * theta_j) + Q * np.exp(b * theta_i))
    r_k = a * np.exp(b * theta_k)
    x_k = r_k * np.cos(theta_k)
    y_k = r_k * np.sin(theta_k)
    return x_k, y_k



class EPC:
    def __init__(self, benchmark, dim, pop_size, lb, ub,a,b):
        self.fitness = benchmark
        self.dim = dim
        self.pop_size = pop_size
        self.lb = np.array(lb)
        self.ub = np.array(ub)
        self.penguinPosition = np.random.uniform(self.lb, self.ub, (pop_size, dim))
        self.fitness_vals = np.array([benchmark(ind) for ind in self.penguinPosition])
        self.best_position_idx = np.argmin(self.fitness_vals)
        self.best_position = self.penguinPosition[self.best_position_idx].copy()
        self.a = a
        self.b = b
    
    def heat(self,D):
        baseU = 0.05
        u = (baseU * (1 - (self.current_iter / self.max_iter)))
        return np.exp(-u * D)
    
    def step(self):
        self.best_position_idx = np.argmin(self.fitness_vals)
        self.best_position = self.penguinPosition[self.best_position_idx].copy()

        newPenguinPositions = self.penguinPosition.copy()

        for i in range(self.pop_size):
            if i == self.best_position_idx:
                continue

            currentPosition = self.penguinPosition[i]

            D = distance(self.best_position,currentPosition)
            Q = self.heat(D)

            new_pos = get_adjusted_position(currentPosition, self.best_position, Q,self.b,self.a)
                
            exploration_rate = 0.5 * (1 - self.current_iter / self.max_iter)
            noise = np.random.uniform(-1, 1, self.dim) * (self.ub - self.lb) * exploration_rate
            new_pos = new_pos + noise
                
            new_pos = np.clip(new_pos, self.lb, self.ub)

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