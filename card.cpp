#include <cstdlib>   // card > aek.ppm
#include <cstdio>
#include <cmath>
#include <fstream>
#include <iostream>

typedef int i;
typedef float f;

class vec3 {
    public:
    f x, y, z;

    vec3(): x(0), y(0), z(0) {
    }

    vec3(const f a, const f b, const f c) {
        x = a;
        y = b;
        z = c;
    }

    vec3 operator+(const vec3 r) const {
        return {x + r.x
                 , y + r.y, z + r.z};
    }

    vec3 operator*(f r) const {
        return
                {x * r, y * r, z * r};
    }

    f operator%(const vec3 r) const {
        return x * r.x + y * r.y + z * r.z;
    }

    vec3 operator^(const vec3 r) const {
        return {y * r.z - z * r.y, z * r.x - x * r.z, x * r.
                                                       y - y * r.x};
    }

    vec3 operator!() const {
        return *this * (1 / sqrt(*this % * this));
    }

    vec3 operator-(const vec3 r) const {
        return {x - r.x, y - r.y, z - r.z};
    }
};

i G[] = {
    247570, 280596, 280600,
    249748, 18578, 18577, 231184, 16, 16
};

f R() {
    return (f) rand() / RAND_MAX;
}

i T(vec3 origin, vec3 direction, f &t, vec3 &n) {
    t = 1e9;
    i m = 0;

    f p = -origin.z / direction.z;

    if (.01 < p) {
        t = p;
        n = vec3(0, 0, 1);
        m = 1;
    }

    for (i k = 19; k--;) {
        for (i j = 9; j--;) {
            if (!(G[j] & 1 << k)) {
                continue;
            }
            
            vec3 p = origin + vec3(-k, 0, -j - 4);

            f b = p % direction;
            f q = b * b - (p % p - 1);

            if (q > 0) {
                f s = -b - sqrt(q);

                if (s < t && s > .01) {
                    t = s;
                    n = !(p + direction * t);
                    m = 2;
                }
            }
        }
    }
    return m;
}

const auto FLOOR_COLOR_ONE = vec3(3, 1, 1);
const auto FLOOR_COLOR_TWO = vec3(3, 3, 3);
const float BOUNCE_ATTENUATION = 0.7f;
const auto LIGHT_POSITION = vec3(9, 9, 16);

vec3 colorIteration(vec3 origin, vec3 direction) {
    f t;
    vec3 n;

    i intersectType = T(origin, direction, t, n);

    if (intersectType == 0) {
        return vec3(.1, .9, .4) * pow(1 - direction.z, 4);
    }

    vec3 h = origin + direction * t;

    const vec3 l = !(LIGHT_POSITION - h);

    const vec3 r = direction + n * (n % direction * -2);

    f b = l % n;

    if (b < 0 || T(h, l, t, n)) {
        b = 0;
    }

    f p = pow(l % r * (b > 0), 99);

    if (intersectType == 1) {
        h = h * .2;
        return (static_cast<i>(ceil(h.x) + ceil(h.y)) & 1
                    ? FLOOR_COLOR_ONE
                    : FLOOR_COLOR_TWO) * (b * .2 + .1);
    }

    return vec3(p, p, p) + colorIteration(h, r) * BOUNCE_ATTENUATION;
}

constexpr auto N_ITERATIONS = 64;
constexpr auto WIDTH = 800;
constexpr auto HEIGHT = 800;

i main() {
    printf("P6 %i %i 255 ", WIDTH, HEIGHT);

    const auto cameraDirection = !vec3(0,-1, 0);
    const auto cameraUpVector = !(vec3(0, 0, 1) ^ cameraDirection) * .002;
    const auto rightVector = !(cameraDirection ^ cameraUpVector) * .002;
    const auto cameraOrigin = vec3(6, 16, 8);

    const auto c = (cameraUpVector + rightVector) * -256 + cameraDirection;

    for (i y = WIDTH; y--;) {
        for (i x = HEIGHT; x--;) {
            vec3 p(1, 1, 1);

            for (i r = N_ITERATIONS; r--;) {
                vec3 delta = cameraUpVector * (R() - .5) * 99 + rightVector * (R() - .5) * 99;

                p = colorIteration(cameraOrigin + delta, !(delta * -1 + (cameraUpVector * (R() + static_cast<f>(x)) + rightVector * (static_cast<f>(y) + R()) + c) * 16)) * 3.5 + p;
            }

            printf("%c%c%c", static_cast<i>(p.x), static_cast<i>(p.y), static_cast<i>(p.z));
        }
    }

    return 0;
}
