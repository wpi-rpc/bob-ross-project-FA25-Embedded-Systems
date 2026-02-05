idf_component_register(SRCS "main.cpp" "edge_path_planning.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES esp_http_server esp_wifi nvs_flash opencv zlib)