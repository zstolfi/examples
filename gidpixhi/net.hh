#pragma once
#include "polytope.hh"
#include <map>

template <class Coord>
class Net {
	using T = Coord::value_type;
	static constexpr auto N = Coord::dimension;
	using Polytope = Polytope<Coord>;
	using Face = Polytope::Face;

	struct Facet {
		Polytope polytope {};
		Coord normal {};
		std::vector<Face> ridges {};

		Facet const* parent {nullptr};
		Coord commonPoint {};
	};

	std::vector<Facet> facets {};

public:
	Net() = default;
	Net(Polytope const& polytope) {
		// Explode N-dimensional polytope into (N-1)-dimensional facets.
		for (auto face : polytope.facesOfRank(N-1)) {
			facets.push_back({
				.polytope = Polytope {face},
				.normal = face.normal(polytope.greatest()),
				.ridges = face.lesserFacesOfRank(N-2),
			});
		}
		// Sort every facet by how upwards they face.
		stdr::sort(facets, stdr::greater {}, upComponent);
		// Create tree with facets[0] as the root.
		for (std::size_t i=0; Facet const& parent : facets) {
			for (Facet& child : facets | stdv::drop(++i)) {
				if (child.parent != nullptr) continue;
				if (auto ridge = commonRidge(parent, child)) {
					child.parent = &parent;
					child.commonPoint = ridge->center();
				}
			}
		}
	}

	std::vector<Polytope> interpolate(double t) {
		std::vector<Polytope> result {};
		for (Facet f : facets) {
			for (Facet const* node = &f; node; node = node->parent) {
				Coord normal = node->normal;
				Coord target = node->parent? node->parent->normal: Up();
				Coord middle = angleInterpolate(t, normal, target);
				// Define rotation matrix.
				// https://math.stackexchange.com/a/4167838/1743259
				Coord u = normal, v = middle;
				Matrix<T, N, N> matrix {FromDiagonal};
				matrix -= outer(u+v, u+v) / (1 + dot(u, v));
				matrix += 2 * outer(v, u);
				f.polytope.transform([&](Coord c) {
					return matrix * (c - node->commonPoint) + node->commonPoint;
				});
			}
			result.push_back(f.polytope);
		}
		return result;
	}

private:
	// TODO: Make this a constexpr member.
	static Coord Up() {
		Coord c {};
		c[N-1] = T{1};
		return c;
	}

	static T upComponent(Facet const& f) {
		return f.normal[N-1];
	}

	std::optional<Face> commonRidge(Facet const& f1, Facet const& f2) {
		for (Face const& common : intersect(f1.ridges, f2.ridges)) {
			return common;
		}
		return std::nullopt;
	}

	Coord angleInterpolate(double t, Coord const& a, Coord const& b) {
		T angle = acos(dot(a, b));
		if (angle < Epsilon<T>) return lerp(t, a, b);
		// 'u' does the job of 't' but better.
		// https://www.desmos.com/calculator/l8iytcwfei
		double u = 0.5 + 0.5*tan(angle*t - angle/2)/tan(angle/2);
		Coord result = lerp(u, a, b);
		result /= length(result);
		return result;
	}
};
