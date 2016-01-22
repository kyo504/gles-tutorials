#include <cstdlib>
#include <iostream>

#include <glm/vec2.hpp>// glm::vec2
#include <glm/integer.hpp>// glm::uint

std::size_t const VertexCount = 4;
// Float quad geometry
std::size_t const PositionSizeF32 = VertexCount * sizeof(glm::vec2);
glm::vec2 const PositionDataF32[VertexCount] = {
	glm::vec2(-1.0f,-1.0f),
	glm::vec2( 1.0f,-1.0f),
	glm::vec2( 1.0f, 1.0f),
	glm::vec2(-1.0f, 1.0f)
};

// 8 bits signed integer quad geometry
std::size_t const PositionSizeI8 = VertexCount * sizeof(glm::i8vec2);
glm::i8vec2 const PositionDataI8[VertexCount] = {
	glm::i8vec2(-1,-1),
	glm::i8vec2( 1,-1),
	glm::i8vec2( 1, 1),
	glm::i8vec2(-1, 1)
};

// 32 bits signed integer quad geometry
std::size_t const PositionSizeI32 = VertexCount * sizeof(glm::i32vec2);
glm::i32vec2 const PositionDataI32[VertexCount] = {
	glm::i32vec2 (-1,-1),
	glm::i32vec2 ( 1,-1),
	glm::i32vec2 ( 1, 1),
	glm::i32vec2 (-1, 1)
};

int main(int argc, char *argv[]) {
	std::cout << "Welcome to the QNX Momentics IDE" << std::endl;


	return EXIT_SUCCESS;
}
