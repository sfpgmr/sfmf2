#pragma once
#include "sf_windows_base.h"

namespace sf{
  class graphics : public sf::singleton<graphics>
  {
    friend struct sf::singleton<graphics>;
  public:
    virtual ~graphics();
    typedef boost::signals2::signal<void(graphics&) > render_event_t;
    void init();
    void create_device_independent_resources();
    void discard_device_independant_resources();
    void create_device();
    void discard_device();
    void get_dxgi_information();

    IDXGIFactory2Ptr& dxgi_factory(){ return dxgi_factory_; }
    IDXGIOutput2Ptr& dxgi_output(){ return dxgi_output_; }

    ID2D1Factory2Ptr& d2d_factory(){ return d2d_factory_; }
    ID2D1DevicePtr& d2d_device(){ return d2d_device_; }
    ID2D1DeviceContext1Ptr& d2d_context(){ return d2d_context_; }

    ID3D11Device2Ptr& d3d_device(){ return d3d_device_; }
    ID3D11DeviceContext2Ptr& d3d_context(){ return d3d_context_; }

    IDWriteFactory2Ptr& write_factory(){ return write_factory_; }

    IDCompositionDesktopDevicePtr& dcomp_desktop_device(){ return dcomp_desktop_device_; }
    IDCompositionDevice2Ptr& dcomp_device2(){ return dcomp_device2_; }

  private:
    graphics();
    render_event_t device_created_;
    render_event_t before_device_discarded_;
    render_event_t after_device_discarded_;

    ID2D1Factory2Ptr d2d_factory_;
    ID2D1DevicePtr d2d_device_;
    ID2D1DeviceContext1Ptr d2d_context_;

    IDWriteFactory2Ptr write_factory_;
    IWICImagingFactoryPtr wic_imaging_factory_;

    IDXGIFactory2Ptr dxgi_factory_;
    IDXGIAdapter2Ptr dxgi_adapter_;
    IDXGIOutput2Ptr dxgi_output_;
    IDXGIDevice3Ptr dxgi_device_;
    std::wstring dxgi_info_;

    ID3D11Device2Ptr d3d_device_;
    ID3D11DeviceContext2Ptr d3d_context_;

    IDCompositionDesktopDevicePtr dcomp_desktop_device_;
    IDCompositionDevice2Ptr dcomp_device2_;

  };
}

