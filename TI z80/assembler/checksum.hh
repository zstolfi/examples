#pragma once
#include <iostream>

template<typename T>
concept ByteContainer = std::is_same_v<typename T::value_type,std::byte>;

// output single byte
std::ostream& operator<<(std::ostream& os, const std::byte& b) {
	return os.put((char)b);
}

// output array/vector of bytes
template <ByteContainer T>
std::ostream& operator<<(std::ostream& os, const T& bytes) {
	for (std::byte b : bytes) { os << b; }
	return os;
}



class Checksum {
private:
	std::ostream* os;
public:
	int value=0;

	friend Checksum& operator<<(std::ostream& os, Checksum& cs) {
		cs.os = &os;
		return cs;
	}

	template <ByteContainer T>
	friend Checksum& operator<<(Checksum& cs, const T& bytes) {
		for (std::byte b : bytes) {
			cs.value += (int)b;
		}
		(*cs.os) << bytes;
		return cs;
	}
};