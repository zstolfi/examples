#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <bit>

using ByteString = std::vector<std::byte>;

char const* getEndian() {
	switch (std::endian::native) {
		case std::endian::little: return "binary_little_endian";
		case std::endian::big: return "binary_big_endian";
		default: return {};
	}
}

template <std::integral I, class From>
ByteString asBytes(From x) {
	I asInteger {};
	if constexpr (std::floating_point<From> && sizeof(I) == sizeof(From))
		asInteger = std::bit_cast<I>(x);
	else if constexpr (std::integral<From> && sizeof(I) <= 4)
		asInteger = x;
	else static_assert("Unsupported numeric type.\n");

	ByteString result (sizeof(I), std::byte {});
	for (int i=0; i<sizeof(I); i++) {
		result[i] = std::byte (asInteger >> (8*i) & 0xFF);
	}
	return result;
}

std::ostream& operator<<(std::ostream& os, ByteString const& v) {
	for (auto byte : v) os.put(std::to_integer<char>(byte));
	return os;
}

void writePly(std::ostream& output, Shape const& shape) {
	output << "ply\n";
	if (auto endian = getEndian()) {
		output << "format " << endian << " 1.0\n";
	}
	else std::cerr << "Unsupported platform.\n";
	output << "element vertex " << shape.vertices.size() << "\n";
	output << "property float x\n";
	output << "property float y\n";
	output << "property float z\n";
	output << "element face " << shape.faces.size() << "\n";
	// TODO: possibly optimize "uint" based on vertex count
	output << "property list uchar uint vertex_indices\n";
	output << "end_header\n";
	for (auto const& vertex : shape.vertices) {
		output << asBytes<uint32_t>(vertex.x);
		output << asBytes<uint32_t>(vertex.y);
		output << asBytes<uint32_t>(vertex.z);
	}
	for (auto const& face : shape.faces) {
		output << asBytes<uint8_t>(face.size());
		for (std::size_t i : face) {
			output << asBytes<uint32_t>(i);
		}
	}
}
