#include <cmath>
#include <cstdio>
#include <cstdlib>   // card > aek.ppm
#include <fstream>

class vec3 {
public:
    float x, y, z;

    vec3(): x(0), y(0), z(0) {
    }

    vec3(const float a, const float b, const float c) {
        x = a;
        y = b;
        z = c;
    }

    vec3 operator+(const vec3 r) const {
        return {x + r.x
                 , y + r.y, z + r.z};
    }

    vec3 operator*(float r) const {
        return
                {x * r, y * r, z * r};
    }

    float operator%(const vec3 r) const {
        return x * r.x + y * r.y + z * r.z;
    }

    vec3 operator^(const vec3 r) const {
        return {y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x};
    }

    vec3 operator!() const {
        return *this * (1 / sqrt(*this % * this));
    }

    vec3 operator-(const vec3 r) const {
        return {x - r.x, y - r.y, z - r.z};
    }
};

int SPHERE_LOCATIONS[] = {
    247570,
    280596,
    280600,
    249748,
    18578,
    18577,
    231184,
    16,
    16
};

float R() {
    return static_cast<float>(rand()) / RAND_MAX;
}

const auto SPHERES_CENTRAL_POINT = vec3(0, 0, 4);
const auto SPHERE_RADIUS = 0.5f;

constexpr auto INTERSECT_TYPE_UPWARD = 0;
constexpr auto INTERSECT_TYPE_DOWNWARD = 1;
constexpr auto INTERSECT_TYPE_SPHERE = 2;

int T(const vec3 rayOrigin, const vec3 rayDirection, float &t, vec3 &newDirection) {
    t = MAXFLOAT;
    int intersectType = INTERSECT_TYPE_UPWARD;

    const float condition1 = -rayOrigin.z / rayDirection.z;

    if (condition1 > .01) {
        t = condition1;
        newDirection = vec3(0, 0, 1);
        intersectType = INTERSECT_TYPE_DOWNWARD;
    }

    for (int j = 0; j < 9; j++) {
        for (int k = 0; k < 20; k++) {
            bool hasSphereInLocation = SPHERE_LOCATIONS[j] & 1 << k;

            if (!hasSphereInLocation) {
                continue;
            }

            const auto sphereCenter = vec3(static_cast<float>(k), 0, static_cast<float>(j));

            auto rayFromRayOriginToSphereCenter = rayOrigin - SPHERES_CENTRAL_POINT - sphereCenter;

            const float b = rayFromRayOriginToSphereCenter % rayDirection;
            const float q = b * b - (rayFromRayOriginToSphereCenter % rayFromRayOriginToSphereCenter - SPHERE_RADIUS);

            if (q <= 0) {
                continue;
            }

            float s = -static_cast<float>(b + sqrt(q));

            if (s < t && s > .01) {
                t = s;
                newDirection = !(rayFromRayOriginToSphereCenter + rayDirection * t);
                intersectType = INTERSECT_TYPE_SPHERE;
            }
        }
    }

    return intersectType;
}

const auto FLOOR_COLOR_ONE = vec3(3, 1, 1);
const auto FLOOR_COLOR_TWO = vec3(3, 2, 3);
const auto LIGHT_POSITION = vec3(10, 10, 25);
const auto HORIZON_COLOR = vec3(.1, .9, .4);

constexpr auto BOUNCE_ATTENUATION = 0.7;
constexpr auto HORIZON_ATTENUATION_EXP = 3;
constexpr auto FLOOR_TILING_SCALE_FACTOR = .2;

vec3 SampleRay(const vec3 origin, const vec3 direction) {
    float t;
    vec3 nextDirection;

    int intersectType = T(origin, direction, t, nextDirection);

    if (intersectType == INTERSECT_TYPE_UPWARD) {
        return HORIZON_COLOR * pow(1 - direction.z, HORIZON_ATTENUATION_EXP);
    }

    vec3 h = origin + direction * t;

    const vec3 l = !(LIGHT_POSITION - h);

    const vec3 r = direction + nextDirection * (nextDirection % direction * -2);

    float b = l % nextDirection;

    if (b < 0) {
        b = 0;
    }

    if (T(h, l, t, nextDirection) != 0) {
        b = 0;
    }

    if (intersectType == INTERSECT_TYPE_DOWNWARD) {
        return (static_cast<int>(ceil(h.x * FLOOR_TILING_SCALE_FACTOR) + ceil(h.y * FLOOR_TILING_SCALE_FACTOR)) & 1
                    ? FLOOR_COLOR_ONE
                    : FLOOR_COLOR_TWO) * (b * .2 + .1);
    }

    float p = pow(l % r * (b > 0), 99);

    // other intersect type
    return vec3(p, p, p) + SampleRay(h, r) * BOUNCE_ATTENUATION;
}

constexpr auto N_ITERATIONS = 64;
constexpr auto WIDTH = 800;
constexpr auto HEIGHT = 800;

constexpr auto SCALING_FACTOR = .002;

int main() {
    printf("P6 %i %i 255 ", WIDTH, HEIGHT);

    // x is left/right, left is positive right is negative
    // y is in/out, out is positive in is negative
    // z is up/down, up is positive down is negative

    const auto cameraDirection = !vec3(0, -1, 0);
    const auto cameraUpVector = !(vec3(0, 0, 1) ^ cameraDirection) * SCALING_FACTOR;
    const auto rightVector = !(cameraDirection ^ cameraUpVector) * SCALING_FACTOR;
    const auto cameraOrigin = vec3(3, 12, 8);

    const auto c = (cameraUpVector + rightVector) * -256 + cameraDirection;

    for (int y = WIDTH; y--;) {
        for (int x = HEIGHT; x--;) {
            vec3 p(10, 10, 10);

            for (int r = 0; r < N_ITERATIONS; r++) {
                vec3 delta = cameraUpVector * (R() - .5) * 99 + rightVector * (R() - .5) * 99;

                vec3 aaa = cameraOrigin + delta;
                vec3 bbb = !(delta * -1 + (cameraUpVector * (R() + static_cast<float>(x)) + rightVector * (static_cast<float>(y) + R()) + c) * 16);

                p = p + SampleRay(aaa, bbb) * 3.5;
            }

            printf("%c%c%c", static_cast<int>(p.x), static_cast<int>(p.y), static_cast<int>(p.z));
        }
    }

    return 0;
}
