# En el CMakeLists temporal solo compilamos main.cpp porque los demas archivos estan en Arduino puro y romperan el build
# Todo el resto del codigo debe ser migrado en una PR posterior.
idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "." "../include")
