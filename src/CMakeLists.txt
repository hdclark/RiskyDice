


# Executable.
add_executable (dice
    Dice.cc
)
target_link_libraries (dice
    sfml-graphics 
    sfml-window 
    sfml-system
    z
    Threads::Threads
)

# Installation info.
install(TARGETS 
    dice
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

