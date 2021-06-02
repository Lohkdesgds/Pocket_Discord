set(CERTS "")

if(NOT CMAKE_BUILD_EARLY_EXPANSION AND EXISTS config AND NOT CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY)
    list(APPEND CERTS cert/gateway.pem cert/api.pem)

    if(NOT EXISTS ${COMPONENT_DIR}/cert/gateway.pem OR NOT EXISTS ${COMPONENT_DIR}/cert/api.pem)
        string(CONCAT certs_missing_message
            "\n ****************************************************************************"
            "\n "
            "\n Discord certificates missing. To generate certs please run the script:"
            "\n ${COMPONENT_DIR}/certgen.sh"
            "\n "
            "\n On Windows script should be automatically open in Git-Bash by double-click on it."
            "\n If not, open Git-Bash (on Windows) and run script manually."
            "\n "
            "\n ****************************************************************************"
        )

        string(ASCII 27 ESCAPE)
        message(FATAL_ERROR "${ESCAPE}[33m ${certs_missing_message} ${ESCAPE}[0m")
    endif()
endif()

idf_component_register(
    SRCS 
        src/tasker.cpp
        src/printer.cpp
        src/led_control.cpp
        src/memory_file.cpp
        src/memory_json.cpp
        src/sd_card.cpp
        src/discord/core.cpp
        src/discord/raw/gateway.cpp
        src/discord/raw/http_interface.cpp
        src/discord/raw/time_iso.cpp
        src/discord/gateway/attachment.cpp
        src/discord/gateway/channel.cpp
        src/discord/gateway/embed.cpp
        src/discord/gateway/emoji.cpp
        src/discord/gateway/guild.cpp
        src/discord/gateway/interaction.cpp
        src/discord/gateway/member.cpp
        src/discord/gateway/message.cpp
        src/discord/gateway/presence.cpp
        src/discord/gateway/reaction.cpp
        src/discord/gateway/role.cpp
        src/discord/gateway/user.cpp
    INCLUDE_DIRS include
    REQUIRES json esp_websocket_client esp_http_client
    PRIV_REQUIRES app_update nvs_flash fatfs
    EMBED_TXTFILES ${CERTS}
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)

component_compile_options(-std=gnu++17)