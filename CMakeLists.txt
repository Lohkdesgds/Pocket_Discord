#set(CERTS "")
#
#if(NOT CMAKE_BUILD_EARLY_EXPANSION AND EXISTS config AND NOT CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY)
#    list(APPEND CERTS cert/gateway.pem cert/api.pem)
#
#    if(NOT EXISTS ${COMPONENT_DIR}/cert/gateway.pem OR NOT EXISTS ${COMPONENT_DIR}/cert/api.pem)
#        string(CONCAT certs_missing_message
#            "\n ****************************************************************************"
#            "\n "
#            "\n Discord certificates missing. To generate certs please run the script:"
#            "\n ${COMPONENT_DIR}/certgen.sh"
#            "\n "
#            "\n On Windows script should be automatically open in Git-Bash by double-click on it."
#            "\n If not, open Git-Bash (on Windows) and run script manually."
#            "\n "
#            "\n ****************************************************************************"
#        )
#
#        string(ASCII 27 ESCAPE)
#        message(FATAL_ERROR "${ESCAPE}[33m ${certs_missing_message} ${ESCAPE}[0m")
#    endif()
#endif()

idf_component_register(
    SRCS 
        src/useful.cpp
        src/smartjsonalloc.cpp
        src/ledhandler.cpp
        src/eventhandler.cpp
        src/heapstring.cpp
        src/filehandler.cpp
        src/functionwrapper.cpp
        src/discord/gateway.cpp
        src/discord/core.cpp
    INCLUDE_DIRS include deps
    REQUIRES esp_wifi esp_websocket_client esp_http_client lwip esp_netif
    PRIV_REQUIRES app_update nvs_flash fatfs
    #EMBED_TXTFILES ${CERTS}
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)

component_compile_options(-std=gnu++17)