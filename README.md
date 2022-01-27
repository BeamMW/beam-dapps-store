

# Deploy DAPP store shader:

beam-wallet-masternet.exe -n 127.0.0.1:11001 shader --shader_app_file="dapps_store_app.wasm" --shader_args="action=create" --shader_contract_file="dapps_store_contract.wasm"

# Get cid of shader:

beam-wallet-masternet.exe -n 127.0.0.1:11001 shader --shader_app_file="dapps_store_app.wasm" --shader_args="action=view"

# Create publisher:

beam-wallet-masternet.exe -n 127.0.0.1:11001 shader --shader_app_file="dapps_store_app.wasm" --shader_args="action=add_publisher,cid=72232ccb1cb406e8858a5d80bc9e0471fa70e9c7a49014daffb42437db26e868,name=test publisher"

# Add dapp to store:

beam-wallet-masternet.exe -n 127.0.0.1:11001 shader --shader_app_file="dapps_store_app.wasm" --shader_args="action=add_dapp,cid=72232ccb1cb406e8858a5d80bc9e0471fa70e9c7a49014daffb42437db26e868,id=d766abdb566a43058e93ed9f61f21188,name=test dapp2,ipfs_id=QmQhtVMX8T3tcFu9YbpdgXaBTiPpjdhRk6R53PmV5VCCCC,description=test test test,api_ver=6.3,min_api_ver=6.1"

# Get list of publishers:

beam-wallet-masternet.exe -n 127.0.0.1:11001 shader --shader_app_file="dapps_store_app.wasm" --shader_args="action=view_publishers,cid=72232ccb1cb406e8858a5d80bc9e0471fa70e9c7a49014daffb42437db26e868"

# Get list of dapps:

beam-wallet-masternet.exe -n 127.0.0.1:11001 shader --shader_app_file="dapps_store_app.wasm" --shader_args="action=view_dapps,cid=72232ccb1cb406e8858a5d80bc9e0471fa70e9c7a49014daffb42437db26e868,publisher=54f2a78a412d682716f27a0b305c0dece8ec8f7bca9e0629ed31ab056ef758f200"
