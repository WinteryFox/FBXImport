#include "../include/FBX/TriangulateProcess.h"

namespace FBX {
    void TriangulateProcess::execute(const std::shared_ptr<Mesh> &mesh) {
        std::vector<Face> faces;
        mesh->indexCount = 0;

        for (auto &face : mesh->faces) {
            if (face.indices.size() <= 3) {
                faces.push_back(face);
                mesh->indexCount += face.indices.size();
            } else if (face.indices.size() == 4) {
                uint32_t startVertex = 0;
                for (uint32_t i = 0; i < 4; i++) {
                    const auto &v0 = mesh->vertices[face[(i + 3) % 4]];
                    const auto &v1 = mesh->vertices[face[(i + 2) % 4]];
                    const auto &v2 = mesh->vertices[face[(i + 1) % 4]];
                    const auto &v = mesh->vertices[face[i]];

                    const auto left = v0 - v;
                    const auto diag = v1 - v;
                    const auto right = v2 - v;

                    const float angle = std::acos(left * diag) + std::acos(right * diag);
                    if (angle > M_PI) {
                        startVertex = i;
                        break;
                    }
                }

                faces.emplace_back(
                        std::vector<uint32_t>{
                                face[startVertex],
                                face[(startVertex + 1) % 4],
                                face[(startVertex + 2) % 4]
                        }
                );
                faces.emplace_back(
                        std::vector<uint32_t>{
                                face[startVertex],
                                face[(startVertex + 2) % 4],
                                face[(startVertex + 3) % 4]
                        }
                );
                mesh->indexCount += 6;
            } else {
                std::vector<Vector3> vertices;
                vertices.reserve(face.indices.size());
                for (const auto i : face.indices)
                    vertices.push_back(mesh->vertices[i]);

                Vector3 normal{};
                for (const auto i : face.indices)
                    normal -= mesh->normals[i];
                normal.normalize();

                const float ax = (normal.x > 0 ? normal.x : -normal.x);
                const float ay = (normal.y > 0 ? normal.y : -normal.y);
                const float az = (normal.z > 0 ? normal.z : -normal.z);

                unsigned int ac = 0, bc = 1;
                float inv = normal.z;
                if (ax > ay) {
                    if (ax > az) {
                        ac = 1;
                        bc = 2;
                        inv = normal.x;
                    }
                } else if (ay > az) {
                    ac = 2;
                    bc = 0;
                    inv = normal.y;
                }

                if (inv < 0.0f)
                    std::swap(ac, bc);

                int num = face.indices.size(), ear = 0, tmp, prev = num - 1, next = 0, max = num;
                std::vector<uint32_t> done;
                std::vector<Vector2> vertices2D;
                vertices2D.reserve(vertices.size());
                for (tmp = 0; tmp < max; tmp++) {
                    vertices2D.emplace_back(vertices[tmp][ac], vertices[tmp][bc]);
                    done.push_back(false);
                }

                while (num > 3) {
                    int numFound = 0;
                    for (ear = next;; prev = ear, ear = next) {
                        for (next = ear + 1; done[(next >= max ? next = 0 : next)]; next++);
                        if (next < ear)
                            if (++numFound == 2)
                                break;

                        Vector2 pnt1 = vertices2D[ear], pnt0 = vertices2D[prev], pnt2 = vertices2D[next];

                        if (onLeftSideOfLine(pnt0, pnt2, pnt1))
                            continue;

                        for (tmp = 0; tmp < max; tmp++) {
                            const Vector2 vtmp = vertices2D[tmp];
                            if (vtmp != pnt1 && vtmp != pnt2 && vtmp != pnt0 && isInTriangle(pnt0, pnt1, pnt2, vtmp))
                                break;
                        }

                        if (tmp != max)
                            continue;

                        break;
                    }

                    if (++numFound == 2) {
                        num = 0;
                        break;
                    }

                    faces.emplace_back(
                            std::vector<uint32_t>{
                                    static_cast<unsigned int>(face[prev]),
                                    static_cast<unsigned int>(face[ear]),
                                    static_cast<unsigned int>(face[next])
                            }
                    );
                    mesh->indexCount += 3;

                    done[ear] = true;
                    num--;
                }

                if (num > 0) {
                    Face f{std::vector<uint32_t>(3)};
                    for (tmp = 0; done[tmp]; tmp++)
                        f.indices[0] = face[tmp];

                    for (++tmp = 0; done[tmp]; tmp++)
                        f.indices[1] = face[tmp];

                    for (++tmp = 0; done[tmp]; tmp++)
                        f.indices[2] = face[tmp];
                    faces.push_back(f);
                    mesh->indexCount += 3;
                }
            }
        }

        mesh->faces = faces;
    }

    bool TriangulateProcess::isInTriangle(const Vector2 &point, const Vector2 &a, const Vector2 &b, const Vector2 &c) {
        float denominator = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);

        if (denominator == 0)
            return true;
        denominator = 1.0f / denominator;
        float alpha = denominator * ((b.y - c.y) * (point.x - c.x) + (c.x - b.x) * (point.y - c.y));
        float beta = denominator * ((c.y - a.y) * (point.x - c.x) + (a.x - c.x) * (point.y - c.y));
        float gamma = 1.0f - alpha - beta;

        return !(alpha < 0 || beta < 0 || gamma < 0);
    }

    bool TriangulateProcess::onLeftSideOfLine(const Vector2 &a, const Vector2 &b, const Vector2 &c) {
        return 0.5f * (a.x * ((double) c.y - b.y) + b.x * ((double) a.y - c.y) + c.x * ((double) b.y - a.y)) > 0;
    }
}