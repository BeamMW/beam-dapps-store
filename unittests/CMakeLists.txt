cmake_minimum_required(VERSION 3.17)

add_test_snippet(shader_test bvm beam)
target_include_directories(shader_test PUBLIC ${PROJECT_SOURCE_DIR}/shaders)

add_shader(dapps_store_contract ${PROJECT_SOURCE_DIR}/shaders)
add_shader(dapps_store_app ${PROJECT_SOURCE_DIR}/shaders)
add_shader(dapps_store_admin_app ${PROJECT_SOURCE_DIR}/shaders)

add_custom_target(make_dapps_store_header
    ALL
    COMMAND generate-sid ${PROJECT_SOURCE_DIR}/shaders/dapps_store_contract.wasm > ${PROJECT_SOURCE_DIR}/shaders/dapps_store_contract_sid.i
    COMMENT "Generating SID ..."
    DEPENDS generate-sid dapps_store_contract_target
)

add_dependencies(shader_test make_dapps_store_header)
add_dependencies(dapps_store_app_target make_dapps_store_header)
add_dependencies(dapps_store_admin_app_target make_dapps_store_header)
