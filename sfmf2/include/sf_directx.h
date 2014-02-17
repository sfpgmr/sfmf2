#pragma once
#include "sf_windows_base.h"

namespace sf {

  struct dxfactories;

  namespace d3d {

    struct device {

    };

    template <bool Deferred = false, D3D_DRIVER_TYPE DriverType = D3D_DRIVER_TYPE_HARDWARE, UINT Flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT>
    struct device_impl : public sf::d3d::device
    {
      friend struct dxfactories;
      device_impl(const device_impl& src)
        : device_(src.device_), context_(src.context_), feature_level_(src.feature_level_), deferred_(Deferred)
      {}
      device_impl(device_impl&& src)
      {
        device_.Swap(src.device_);
        context_.Swap(src.context_);
        feature_level_ = src.feature_level_;
      }
    private:
      device_impl(ID3D11Device2Ptr& d, ID3D11DeviceContext2Ptr& c, D3D_FEATURE_LEVEL level);
      ID3D11Device2Ptr device_;
      ID3D11DeviceContext2Ptr context_;
      ID3D11DeviceContext2Ptr deferred_context_;
      D3D_FEATURE_LEVEL feature_level_;
      bool deferred_;
    };
  }

  struct dxfactories : public singleton<dxfactories> {

    // IDXGIOutputの抽象化
    struct output {
      explicit output(IDXGIOutput2Ptr& output) : output_(output)
      {
      }
      IDXGIOutput2Ptr& get(){ return output_; }
    private:
      IDXGIOutput2Ptr output_;
    };

    typedef std::vector<output> outputs_t;

    // IDXGIAdapterの抽象化
    struct adapter{

      explicit adapter(IDXGIAdapter2Ptr& dxgi_adapter)
      {
         adapter_ = dxgi_adapter;
         CHK(adapter_->GetDesc2(&desc_));
         device_id_ = desc_.DeviceId;
         description_ = desc_.Description;

         {
           int i = 0;
           IDXGIOutputPtr out;
           while (dxgi_adapter->EnumOutputs(i, &out) != DXGI_ERROR_NOT_FOUND)
           {
             IDXGIOutput2Ptr out2;
             CHK(out.As(&out2));
             outputs_.push_back(output(out2));
             ++i;
           }
         }
      }

      std::wstring& description(){ return description_; }
      DXGI_ADAPTER_DESC2& desc(){ return desc_; }
      uint32_t device_id(){ return device_id_; }
      IDXGIAdapter2Ptr& get() { return adapter_; }

    private:
      std::wstring description_;
      uint32_t device_id_;
      DXGI_ADAPTER_DESC2 desc_;
      IDXGIAdapter2Ptr adapter_;
      outputs_t outputs_;
    };

    typedef std::vector<adapter> adapters_t;

    // dxgi コンストラクタ
    dxfactories()
    {
      IDXGIFactory2Ptr factory;
      CHK(CreateDXGIFactory2(0, __uuidof(IDXGIFactory2), &factory));
      CHK(factory.As(&factory_));

      // ディスプレイアダプターの列挙
      {
        IDXGIAdapter1Ptr adapter1;
        int i = 0;
        while (factory_->EnumAdapters1(i, adapter1.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND){
          IDXGIAdapter2Ptr adapter2;
          CHK(adapter1.As(&adapter2));
          adapters_.push_back(adapter(adapter2));
          ++i;
        }
      }

    }
    
    adapters_t & adapters(){ return adapters_; }

    d3d::device create_d3d11device
      (D3D_DRIVER_TYPE driver_type = D3D_DRIVER_TYPE_HARDWARE, UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT);
    d3d::device create_d3d11device
      (adapter& adapter, D3D_DRIVER_TYPE driver_type = D3D_DRIVER_TYPE_UNKNOWN, UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT);
  private:
    IDXGIFactory3Ptr factory_;
    IDWriteFactory2Ptr d2d_factory_;
    IDWriteFactory2Ptr dw_factory_;
    adapters_t adapters_;
    static std::vector<D3D_FEATURE_LEVEL> d3d_feature_level_;
  };
}

