import matplotlib.pyplot as plt
from epc import EPC
from benchmarks import sphere, rosenbrock, rastrigin

def run_experiment(func, name):

    model = EPC(func, dim=5, pop_size=25, lb=-5, ub=5,a = 1,b = 0.5)
    best, history = model.run(100)

    print(f"{name} best fitness:", func(best))

    plt.plot(history)
    plt.title(f"EPC Convergence - {name}")
    plt.xlabel("Iteration")
    plt.ylabel("Best Fitness")
    plt.show()

if __name__ == "__main__":
    run_experiment(sphere, "Sphere")
    run_experiment(rosenbrock, "Rosenbrock")
    run_experiment(rastrigin, "Rastrigin")
