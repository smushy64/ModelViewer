#pragma once

struct DXGI_SWAP_CHAIN_DESC;

namespace Platform {

struct DirectX11InitData {
    DXGI_SWAP_CHAIN_DESC sd;
};

} // namespace Platform

