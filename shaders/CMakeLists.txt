cmake_minimum_required(VERSION 3.17)

include(GenerateSID)

add_executable(dapps_store_contract dapps_store_contract.cpp)
copy_shader(dapps_store_contract)
generate_sid_header(dapps_store_contract)

add_executable(dapps_store_app dapps_store_app.cpp)

add_dependencies(dapps_store_app dapps_store_contract_header)
copy_shader(dapps_store_app)
