#include <array>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <regex>
#include <string>
#include <queue>
#include <vector>
#include "../shared/base.hpp"

#define DEFAULT_DISTANCE INT16_MAX

struct Cube {
    int x;
    int y;
    int z;
};

enum Plane {
    X, Y, Z
};

struct Face {
    int x;
    int y;
    int z;
    Plane plane;
    bool front; // Not part of equals/hash

    Face(int x, int y, int z, Plane plane) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->plane = plane;
        this->front = true;
    }

    Face(int x, int y, int z, Plane plane, bool front) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->plane = plane;
        this->front = front;
    }
};

bool operator==(const Face& left, const Face& right) {
    return left.x == right.x && left.y == right.y && left.z == right.z && left.plane == right.plane;
}

template<> struct std::hash<Face> {
    std::size_t operator()(Face const& f) const noexcept {
        std::size_t h1 = std::hash<int>{}(f.x);
        std::size_t h2 = std::hash<int>{}(f.y);
        std::size_t h3 = std::hash<int>{}(f.z);
        std::size_t h4 = std::hash<int>{}(f.plane);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};

std::vector<Cube> parse(std::ifstream& input);

void insert_face(std::unordered_set<Face>& faces, Face face) {
    if (faces.contains(face)) {
        faces.erase(face);
    } else {
        faces.insert(face);
    }
}

void part1(std::ifstream& input) {
    std::vector<Cube> cubes = parse(input);
    std::unordered_set<Face> faces;
    for (Cube cube : cubes) {
        insert_face(faces, Face(cube.x, cube.y, cube.z, Plane::X));
        insert_face(faces, Face(cube.x, cube.y, cube.z, Plane::Y));
        insert_face(faces, Face(cube.x, cube.y, cube.z, Plane::Z));
        insert_face(faces, Face(cube.x + 1, cube.y, cube.z, Plane::X));
        insert_face(faces, Face(cube.x, cube.y + 1, cube.z, Plane::Y));
        insert_face(faces, Face(cube.x, cube.y, cube.z + 1, Plane::Z));
    }
    std::cout << faces.size() << std::endl;
}

std::vector<Face> construct_candidate_faces(Face face, Plane plane, bool use_a) {
    std::array<int, 3> offsets_a = {1, 1, 1};
    std::array<int, 3> offsets_b = {0, -1, 0};
    std::array<int, 3> offsets_plane_front = {0, 0, -1};
    std::array<int, 3> offsets_plane_back = {-1, 0, 0};
    std::array<int, 3> offsets_zero = {0, 0, 0};

    std::array<int, 3> x_offsets = offsets_zero;
    std::array<int, 3> y_offsets = offsets_zero;
    std::array<int, 3> z_offsets = offsets_zero;

    switch (face.plane) {
    case Plane::X:
        x_offsets = face.front ? offsets_plane_front : offsets_plane_back;
        break;
    case Plane::Y:
        y_offsets = face.front ? offsets_plane_front : offsets_plane_back;
        break;
    case Plane::Z:
        z_offsets = face.front ? offsets_plane_front : offsets_plane_back;
        break;
    }
    switch(plane) {
    case Plane::X:
        x_offsets = use_a ? offsets_a : offsets_b;
        break;
    case Plane::Y:
        y_offsets = use_a ? offsets_a : offsets_b;
        break;
    case Plane::Z:
        z_offsets = use_a ? offsets_a : offsets_b;
        break;
    }

    std::vector<Face> result;
    for (int i = 0; i < 3; i++) {
        result.push_back(Face(face.x + x_offsets[i], face.y + y_offsets[i], face.z + z_offsets[i], i == 1 ? face.plane : plane));
    }
    return result;
}

std::vector<std::vector<Face>> get_boundary_candidate_faces(Face face) {
    std::array<int, 3> offsets_a = {1, 1, 1};
    std::array<int, 3> offsets_b = {0, -1, 0};
    std::array<int, 3> offsets_plane_front = {0, 0, -1};
    std::array<int, 3> offsets_plane_back = {-1, 0, 0};
    std::array<int, 3> offsets_zero = {0, 0, 0};

    std::vector<std::vector<Face>> results (4);
    if (face.plane == Plane::X) {
        results.push_back(construct_candidate_faces(face, Plane::Y, true));
        results.push_back(construct_candidate_faces(face, Plane::Y, false));
        results.push_back(construct_candidate_faces(face, Plane::Z, true));
        results.push_back(construct_candidate_faces(face, Plane::Z, false));
    } else if (face.plane == Plane::Y) {
        results.push_back(construct_candidate_faces(face, Plane::X, true));
        results.push_back(construct_candidate_faces(face, Plane::X, false));
        results.push_back(construct_candidate_faces(face, Plane::Z, true));
        results.push_back(construct_candidate_faces(face, Plane::Z, false));
    } else if (face.plane == Plane::Z) {
        results.push_back(construct_candidate_faces(face, Plane::X, true));
        results.push_back(construct_candidate_faces(face, Plane::X, false));
        results.push_back(construct_candidate_faces(face, Plane::Y, true));
        results.push_back(construct_candidate_faces(face, Plane::Y, false));
    }

    return results;
}

void part2(std::ifstream& input) {
    std::vector<Cube> cubes = parse(input);
    std::unordered_set<Face> faces;
    for (Cube cube : cubes) {
        insert_face(faces, Face(cube.x, cube.y, cube.z, Plane::X, false));
        insert_face(faces, Face(cube.x, cube.y, cube.z, Plane::Y, false));
        insert_face(faces, Face(cube.x, cube.y, cube.z, Plane::Z, false));
        insert_face(faces, Face(cube.x + 1, cube.y, cube.z, Plane::X, true));
        insert_face(faces, Face(cube.x, cube.y + 1, cube.z, Plane::Y, true));
        insert_face(faces, Face(cube.x, cube.y, cube.z + 1, Plane::Z, true));
    }

    // An extreme coordinate must be on the outside
    Face starting_face = Face(INT8_MAX, INT8_MAX, INT8_MAX, Plane::X);
    for (Face face : faces) {
        if (face.plane == Plane::X && face.x < starting_face.x) {
            starting_face = face;
        }
    }

    std::unordered_set<Face> exterior_faces;
    exterior_faces.insert(starting_face);
    std::queue<Face> queue;
    queue.push(starting_face);

    while (!queue.empty()) {
        Face face = queue.front();
        queue.pop();

        for (std::vector<Face>& candidates : get_boundary_candidate_faces(face)) {
            for (Face candidate : candidates) {
                if (faces.contains(candidate)) {
                    candidate = *faces.find(candidate);
                    if (!exterior_faces.contains(candidate)) {
                        exterior_faces.insert(candidate);
                        queue.push(candidate);
                    }
                    break;
                }
            }
        }
    }
    std::cout << exterior_faces.size() << std::endl;
}

std::vector<Cube> parse(std::ifstream& input) {
    std::vector<Cube> cubes;
    std::string buf;
    while (std::getline(input, buf, ',')) {
        int x = std::stoi(buf);
        std::getline(input, buf, ',');
        int y = std::stoi(buf);
        std::getline(input, buf);
        int z = std::stoi(buf);
        cubes.push_back(Cube{
            .x = x,
            .y = y,
            .z = z,
        });
    }
    return cubes;
}
