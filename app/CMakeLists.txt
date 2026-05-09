# Declare an executable target
add_executable(bit_chess_cpp_app)

# Add source files to the executable target
target_sources(bit_chess_cpp_app
  PRIVATE
    bit_chess_cpp_app.cpp
)

# Link the executable against the library
target_link_libraries(bit_chess_cpp_app
  PRIVATE
    bit_chess_cpp
)
