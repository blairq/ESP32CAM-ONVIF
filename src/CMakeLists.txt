# Compilamos main.cpp y los modulos migrados
idf_component_register(SRCS "main.cpp"
                            "status_led.cpp"
                            "auto_flash.cpp"
                            "serial_console.cpp"
                            "wifi_manager.cpp"
                       INCLUDE_DIRS "." "../include")
