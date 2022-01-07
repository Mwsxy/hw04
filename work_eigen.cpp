#include <Eigen/Core>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <cmath>

float frand() {
    return (float)rand() / RAND_MAX * 2 - 1;
}

// struct Star {
//     float px, py, pz;
//     float vx, vy, vz;
//     float mass;
// };

// std::vector<Star> stars;


constexpr int NSTAR = 48;
using Mat3 = Eigen::Matrix<float, 3, NSTAR>;
Mat3 points, velocity;
Eigen::Matrix<float, NSTAR, 1> mass;



void init() {
    for (int i = 0; i < NSTAR; i++) {
        points(0, i) = frand();
        points(1, i) = frand();
        points(2, i) = frand();

        velocity(0, i) = frand();
        velocity(1, i) = frand();
        velocity(2, i) = frand();

        mass(i) = frand() + 1;
    }
}

float G = 0.001;
float eps = 0.001;
float dt = 0.01;

void step() {
    for (int i = 0; i < NSTAR; i++) {
        auto star = points.col(i);

        for (int j = 0; j < NSTAR; j++) {
            auto d = points.col(j) - points.col(i);
            float d2 = d.squaredNorm()+eps;
            d2 *= std::sqrt(d2);
            velocity.col(i) += d * mass(j) * G * dt / d2;
        }
    }
    points += velocity * dt;

    // for (auto &star: stars) {
    //     for (auto &other: stars) {
    //         float dx = other.px - star.px;
    //         float dy = other.py - star.py;
    //         float dz = other.pz - star.pz;
    //         float d2 = dx * dx + dy * dy + dz * dz + eps * eps;
    //         d2 *= sqrt(d2);
    //         star.vx += dx * other.mass * G * dt / d2;
    //         star.vy += dy * other.mass * G * dt / d2;
    //         star.vz += dz * other.mass * G * dt / d2;
    //     }
    // }
    // for (auto &star: stars) {
    //     star.px += star.vx * dt;
    //     star.py += star.vy * dt;
    //     star.pz += star.vz * dt;
    // }
}

float calc() {
    float energy = 0;
    for (int i = 0; i < NSTAR; i++) {
        float v2 = velocity.col(i).squaredNorm()+eps;
        energy += mass(i) * v2 / 2;
        for (int j = 0; j < NSTAR; j++) {
            auto d = points.col(j) - points.col(i);
            float d2 = d.norm()+eps;
            energy -= mass(j) * mass(i) * G / d2 /2 ;
        }
    }
    return energy;
}

template <class Func>
long benchmark(Func const &func) {
    auto t0 = std::chrono::steady_clock::now();
    func();
    auto t1 = std::chrono::steady_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    return dt.count();
}

int main() {
    init();
    printf("Initial energy: %f\n", calc());
    auto dt = benchmark([&] {
        for (int i = 0; i < 100000; i++)
            step();
    });
    printf("Final energy: %f\n", calc());
    printf("Time elapsed: %ld ms\n", dt);
    return 0;
}
