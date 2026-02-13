#pragma once
#include "math.hh"
#include "uniqueVector.hh"
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <ranges>
namespace stdr = std::ranges;
namespace stdv = std::views;

constexpr struct FromRegular_Arg    {} FromRegular    {};
constexpr struct FromConvexHull_Arg {} FromConvexHull {};
constexpr double Epsilon {1e-10};

struct Shape {
	using VertexIndex = std::size_t;
	using EdgeIndex = std::size_t;
	using FaceIndex = std::size_t;

	using Vertex = Point;
	using Edge = Set<VertexIndex>;
	using Face = Set<EdgeIndex>;
	UnorderedSet<Vertex> vertices {};
	UnorderedSet<Edge> edges {};
	UnorderedSet<Face> faces {};

	Shape() = default;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

public:
	// Construct from regularly spaced vertices.
	Shape(FromRegular_Arg, stdr::input_range auto points) {
		vertices = UnorderedSet<Vertex> {points.begin(), points.end()};
		// The edges are the nearest other vertices to any vertex.
		for (VertexIndex vi=0; vi<vertices.size(); vi++) {
			// Order all vertices by distance (including self).
			std::multimap<double, VertexIndex> distanceOrder {};
			for (VertexIndex vj=0; vj<vertices.size(); vj++) {
				double dist = measure1D(vertices[vi], vertices[vj]);
				distanceOrder.insert({dist, vj});
			}
			// Skip ourself (distance 0) and insert the next closest.
			double minOtherDist = std::next(distanceOrder.begin(), 1)->first;
			auto lower = distanceOrder.lower_bound(minOtherDist);
			auto upper = distanceOrder.upper_bound(minOtherDist + Epsilon);
			for (auto it=lower; it!=upper; ++it) edges.insert({vi, it->second});
		}
		// Faces are always on the convex hull of the shape.
		for (EdgeIndex ei=0; ei<edges.size(); ei++) {
			// Only two coplanar loops will be on the convex hull.
			for (VertexIndex vi : connectedVertices(edges[ei][0])) {
				Set<VertexIndex> plane = edges[ei];
				if (plane.insert(vi); plane.size() < 3) continue;
				auto possibleFace = coplanarVertices(plane);
				if (isOnConvexHull(possibleFace)) {
					// Add all edges on this face's perimeter.
					faces.insert(edgesInVertexSet(possibleFace));
				}
			}
		}
	}

private:
	Set<VertexIndex> connectedVertices(VertexIndex vi) const {
		Set<VertexIndex> result {};
		for (Edge const& e : edges) {
			if (e[0] == vi) result.insert(e[1]);
			if (e[1] == vi) result.insert(e[0]);
		}
		return result;
	}

	Set<VertexIndex> coplanarVertices(Set<VertexIndex> planeVertices) const {
		Set<VertexIndex> result {};
		Point tangent = normalOfVertices(planeVertices);
		for (VertexIndex vi=0; vi<vertices.size(); vi++) {
			Point dir = sub(vertices[vi], vertices[planeVertices[0]]);
			if (std::abs(dot(dir, tangent)) < Epsilon) result.insert(vi);

		}
		return result;
	}

	bool isOnConvexHull(Set<VertexIndex> planeVertices) const {
		bool anyPositive = false;
		bool anyNegative = false;
		Point tangent = normalOfVertices(planeVertices);
		for (VertexIndex vi=0; vi<vertices.size(); vi++) {
			Point dir = sub(vertices[vi], vertices[planeVertices[0]]);
			double h = dot(dir, tangent);
			if (h > +Epsilon) anyPositive = true;
			if (h < -Epsilon) anyNegative = true;
		}
		return !anyPositive || !anyNegative;
	}

	Set<EdgeIndex> edgesInVertexSet(Set<VertexIndex> viSet) const {
		Set<EdgeIndex> result {};
		for (EdgeIndex ei=0; ei<edges.size(); ei++) {
			if (viSet.contains(edges[ei][0]) && viSet.contains(edges[ei][1])) {
				result.insert(ei);
			}
		}
		return result;
	}

	Point normalOfVertices(Set<VertexIndex> viSet) const {
		return normalFromPlane(std::vector<Point> {
			vertices[viSet[0]],
			vertices[viSet[1]],
			vertices[viSet[2]],
		});
	}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

public:
	// Construct convex hull, assuming most input points are on the hull.
	Shape(FromConvexHull_Arg, stdr::input_range auto points) {
		auto& v = vertices;
		// Add points one-by-one, every step will be a valid hull.
		for (Point p : points) switch (v.size()) {
		case 0:
			v.insert(p);
			break;
		case 1:
			// Make sure the points don't occupy the same space.
			if (measure1D(p, v[0]) > Epsilon*Epsilon) {
				v.insert(p);
				edges.insert({0, 1});
			}
			break;
		case 2:
			// Not colinear.
			if (measure2D(p, v[0], v[1]) > Epsilon*Epsilon) {
				v.insert(p);
				edges.insert({0, 2});
				edges.insert({1, 2});
				faces.insert({0, 1, 2});
			}
			// Colinear, discard the middle point of the three.
			else {
				Point direction = norm(sub(v[1], v[0]));
				bool insideV0 = dot(sub(p, v[0]),  direction) > -Epsilon;
				bool insideV1 = dot(sub(p, v[1]), -direction) > -Epsilon;
				if (!insideV0 &&  insideV1) v.reassign(0, p);
				if ( insideV0 && !insideV1) v.reassign(1, p);
			}
			break;
		default:
			if (faces.size() == 1) {
				// Not coplanar, create pyramid.
				if (measure3D(p, v[0], v[1], v[2]) > Epsilon*Epsilon) {
					VertexIndex vi = v.insert(p);
					for (EdgeIndex ei0=0; ei0<faces[0].size(); ei0++) {
						EdgeIndex ei1 = edges.insert({edges[ei0][0], vi});
						EdgeIndex ei2 = edges.insert({edges[ei0][1], vi});
						faces.insert({ei0, ei1, ei2});
					}
					break;
				}
				// Coplanar, add to the face if we're not inside it
				std::set<EdgeIndex> visibleEdges {};
				std::map<VertexIndex, int> vertexCounts {};
				for (EdgeIndex ei=0; ei<faces[0].size(); ei++) {
					Point outwards = normalOfEdge(FaceIndex {0}, ei);
					Point onEdge = v[edges[ei][0]];
					if (dot(sub(p, onEdge), outwards) > -Epsilon) {
						visibleEdges.insert(ei);
						vertexCounts[edges[ei][0]]++;
						vertexCounts[edges[ei][1]]++;
					}
				}
				Set<VertexIndex> horizon {};
				for (auto [vi, count] : vertexCounts) {
					if (count == 1) horizon.insert(vi);
				}
				VertexIndex vi = v.insert(p);
				EdgeIndex ei0 = edges.insert({horizon[0], vi});
				EdgeIndex ei1 = edges.insert({horizon[1], vi});
				Face newFace = {ei0, ei1};
				for (EdgeIndex ei : faces[0]) {
					if (!visibleEdges.contains(ei)) newFace.insert(ei);
				}
				faces.insert(newFace);
				removeFaces({0});
				removeOrphans();
				break;
			}
			// Multiple faces.
			// This is pretty much duplicate code for the 2D case.
			std::set<FaceIndex> visibleFaces {};
			std::map<EdgeIndex, int> edgeCounts {};
			for (FaceIndex fi=0; fi<faces.size(); fi++) {
				Point outwards = normalOfFace(fi);
				Point onFace = v[edges[faces[fi][0]][0]];
				if (dot(sub(p, onFace), outwards) > -Epsilon) {
					visibleFaces.insert(fi);
					for (EdgeIndex ei : faces[fi]) edgeCounts[ei]++;
				}
			}
			Set<EdgeIndex> horizon {};
			for (auto [ei, count] : edgeCounts) {
				if (count == 1) horizon.insert(ei);
			}
			VertexIndex vi = v.insert(p);
			for (EdgeIndex ei0 : horizon) {
				EdgeIndex ei1 = edges.insert({edges[ei0][0], vi});
				EdgeIndex ei2 = edges.insert({edges[ei0][1], vi});
				faces.insert({ei0, ei1, ei2});
			}
			removeFaces(visibleFaces);
			combineCoplanarFaces();
			removeOrphans();
			break;
		}
	}

private:
	Point normalOfEdge(FaceIndex fi, EdgeIndex ei) {
		Point v0 = vertices[edges[ei][0]], v1 = vertices[edges[ei][1]];
		Point inside = insideOfFace(fi);
		Point normal = cross(normalOfFace(fi), sub(v0, v1));
		if (dot(normal, sub(v0, inside)) < 0) normal = -normal;
		return norm(normal);
	}

	Point insideOfFace(FaceIndex fi) {
		// We only need to average three veretices to find a point inside.
		Set<Point> v {};
		for (EdgeIndex ei : faces[fi]) for (VertexIndex vi : edges[ei]) {
			v.insert(vertices[vi]);
			if (v.size() == 3) {
				return scale(add(add(v[0], v[1]), v[2]), 1.0/3);
			}
		}
		return {};
	}

	Point normalOfFace(FaceIndex fi) {
		Set<Point> v {
			vertices[edges[faces[fi][0]][0]], vertices[edges[faces[fi][0]][1]],
			vertices[edges[faces[fi][1]][0]], vertices[edges[faces[fi][1]][1]],
		};
		Point inside = insideOfShape();
		Point normal = cross(sub(v[0], v[1]), sub(v[0], v[2]));
		if (dot(normal, sub(v[0], inside)) < 0) normal = -normal;
		return norm(normal);
	}

	Point insideOfShape() {
		Point sum {};
		for (Vertex v : vertices) sum = add(sum, v);
		return scale(sum, 1.0/vertices.size());
	}

	void removeFaces(std::set<FaceIndex> fiSet) {
		std::size_t newSize = faces.size() - fiSet.size();
		for (FaceIndex fi=0, step=0; fi<newSize; fi++) {
			while (fiSet.contains(fi + step)) step++;
			faces.reassign(fi, faces[fi + step]);
		}
		faces.resize(newSize);
	}

	void combineCoplanarFaces() {
		std::set<FaceIndex> toRemove {};
		// Group faces by equivalent normals.
		auto Spaced = [](Point a, Point b) {
			if (measure1D(a, b) < Epsilon*Epsilon) return false;
			else return a < b;
		};
		std::map<Point, Set<FaceIndex>, decltype(Spaced)> planeGroups {};
		for (FaceIndex fi=0; fi<faces.size(); fi++) {
			planeGroups[normalOfFace(fi)].insert(fi);
		}
		// Merge faces which share normals.
		for (auto [ _ , faceIndices] : planeGroups) {
			if (faceIndices.size() == 1) continue;
			std::map<EdgeIndex, int> edgeCounts {};
			for (FaceIndex fi : faceIndices) {
				toRemove.insert(fi);
				for (EdgeIndex ei : faces[fi]) {
					edgeCounts[ei]++;
				}
			}
			Face newFace {};
			for (auto [ei, count] : edgeCounts) {
				if (count == 1) newFace.insert(ei);
			}
			faces.insert(newFace);
		}
		removeFaces(toRemove);
	}

	void removeOrphans() {
		// Every reachable element will be spared.
		Set<VertexIndex> usedVertices {};
		Set<EdgeIndex> usedEdges {};
		for (auto face : faces) {
			for (EdgeIndex ei : face) {
				usedEdges.insert(ei);
				for (VertexIndex vi : edges[ei]) {
					usedVertices.insert(vi);
				}
			}
		}
		// Keep track of old and new indices.
		std::map<VertexIndex, VertexIndex> viMap {};
		std::map<EdgeIndex  , EdgeIndex  > eiMap {};
		for (VertexIndex vi=0; vi<vertices.size(); vi++) {
			if (usedVertices.contains(vi)) viMap[vi] = viMap.size();
		}
		for (EdgeIndex ei=0; ei<edges.size(); ei++) {
			if (usedEdges.contains(ei)) eiMap[ei] = eiMap.size();
		}
		// Remove vertices.
		for (VertexIndex vi=0, step=0; vi<usedVertices.size(); vi++) {
			while (!usedVertices.contains(vi + step)) step++;
			vertices.reassign(vi, vertices[vi + step]);
		}
		vertices.resize(usedVertices.size());
		// Remove edges.
		for (EdgeIndex ei=0, step=0; ei<usedEdges.size(); ei++) {
			while (!usedEdges.contains(ei + step)) step++;
			edges.reassign(ei, edges[ei + step]);
		}
		edges.resize(usedEdges.size());
		// Reindex vertices.
		for (auto& edge : edges) for (VertexIndex& vi : edge) {
			vi = viMap[vi];
		}
		// Reindex edges.
		for (auto& face : faces) for (EdgeIndex& ei : face) {
			ei = eiMap[ei];
		}
	}
};
