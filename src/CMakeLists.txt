# Compilamos main.cpp y los modulos migrados en esta fase (Fase 2)
idf_component_register(SRCS "main.cpp"
                            "status_led.cpp"
                            "auto_flash.cpp"
                            "serial_console.cpp"
                       INCLUDE_DIRS "." "../include")
