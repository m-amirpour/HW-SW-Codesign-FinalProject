import matplotlib.pyplot as plt
import numpy as np
from epc import EPC
from benchmarks import sphere, rosenbrock, rastrigin


def run_experiment(func, name, runs=5):

    all_histories = []

    for r in range(runs):

        model = EPC(
            benchmark=func,
            dim=10,
            pop_size=50,
            lb=-5.12,
            ub=5.12,
            a=1,
            b=0.3
        )

        best, history = model.run(500)
        all_histories.append(history)

    mean_history = np.mean(all_histories, axis=0)

    print(f"{name} best fitness:", func(best))

    plt.plot(mean_history)
    plt.title(f"EPC Convergence - {name}")
    plt.xlabel("Iteration")
    plt.ylabel("Best Fitness")
    plt.show()


if __name__ == "__main__":

    run_experiment(sphere, "Sphere")
    run_experiment(rosenbrock, "Rosenbrock")
    run_experiment(rastrigin, "Rastrigin")
