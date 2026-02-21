#pragma once
#include "polytope.hh"
#include <iostream>
#include <vector>
#include <map>
#include <cstdint>
#include <cstddef>
#include <bit>

struct PlyGeometry {
	using Vertex = std::array<double, 3>;
	using Face = std::vector<uint32_t>;
	std::vector<Vertex> vertices;
	std::vector<Face> faces;

	PlyGeometry(Vertex v={}, Face f={}): vertices{v}, faces{f} {}

	template <std::convertible_to<Coordinate<double, 3>> Coord>
	PlyGeometry(Polytope<Coord> const& polytope) {
		std::map<Vertex, uint32_t> indexOf {};
		for (auto vertex : polytope.facesOfRank(0)) {
			for (auto point : vertex.points()) {
				Vertex v {point.array()};
				indexOf[v] = vertices.size();
				vertices.push_back(v);
			}
		}
		for (auto face : polytope.facesOfRank(2)) {
			// Get point locations.
			std::vector<Vertex> v {};
			for (auto point : face.points()) v.push_back(point.array());
			// Sort the points cyclically.
			stdr::partial_sort(v, v.begin()+2);
			stdr::sort(v.begin()+2, v.end(), stdr::greater {},
				[&, dir = sub(v[1], v[0])] (Vertex vi) {
					auto n = sub(vi, v[0]);
					// n/|n| · dir
					return dot(n, dir) / std::sqrt(dot(n, n));
				}
			);
			// Store the points as indices.
			std::vector<uint32_t> newFace {};
			for (auto vertex : v) newFace.push_back(indexOf[vertex]);
			faces.push_back(newFace);
		}
	}

	// TODO: write istream parser.
	friend std::ostream& operator<<(std::ostream& os, PlyGeometry const& data) {
		os << "ply\n";
		if (auto endian = getEndian()) {
			os << "format " << endian << " 1.0\n";
		}
		else std::cerr << "Unsupported platform.\n";
		os << "element vertex " << data.vertices.size() << "\n";
		os << "property double x\n";
		os << "property double y\n";
		os << "property double z\n";
		os << "element face " << data.faces.size() << "\n";
		// NOTE: "uint" can be optimized if size is a constraint.
		os << "property list uchar uint vertex_indices\n";
		os << "end_header\n";
		for (auto const& vertex : data.vertices) {
			writeBytes<uint64_t>(os, vertex[0]);
			writeBytes<uint64_t>(os, vertex[1]);
			writeBytes<uint64_t>(os, vertex[2]);
		}
		for (auto const& face : data.faces) {
			writeBytes<uint8_t>(os, face.size());
			for (std::size_t i : face) {
				writeBytes<uint32_t>(os, i);
			}
		}
		return os;
	}

private:
	static Vertex sub(Vertex a, Vertex b) {
		return {a[0]-b[0], a[1]-b[1], a[2]-b[2]};
	}
	static double dot(Vertex a, Vertex b) {
		return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
	}

	static char const* getEndian() {
		switch (std::endian::native) {
			case std::endian::little: return "binary_little_endian";
			case std::endian::big: return "binary_big_endian";
			default: return {};
		}
	}

	template <std::integral I, class From>
	static void writeBytes(std::ostream& os, From x) {
		I asInteger {};
		if constexpr (std::floating_point<From> && sizeof(I) == sizeof(From))
			asInteger = std::bit_cast<I>(x);
		else if constexpr (std::integral<From> && sizeof(I) <= 4)
			asInteger = x;
		else static_assert("Unsupported numeric type.\n");

		for (int i=0; i<sizeof(I); i++) {
			os.put(asInteger >> (8*i) & 0xFF);
		}
	}
};
