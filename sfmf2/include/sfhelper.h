#pragma once
#define CHK(statement)	{HRESULT _hr = (statement); if (FAILED(_hr)) { throw sf::win32_error_exception(_hr); };}
#define _WRL_PTR_TYPEDEF(x) typedef Microsoft::WRL::ComPtr<x> x ## Ptr

// Direct Write

_WRL_PTR_TYPEDEF(IDWriteFactory);
_WRL_PTR_TYPEDEF(IDWriteFactory1);
_WRL_PTR_TYPEDEF(IDWriteFactory2);
_WRL_PTR_TYPEDEF(IDWriteGdiInterop);
_WRL_PTR_TYPEDEF(IDWriteFontFace1);
_WRL_PTR_TYPEDEF(IDWriteFontFace);
_WRL_PTR_TYPEDEF(IDWriteFont1);
_WRL_PTR_TYPEDEF(IDWriteFont);
_WRL_PTR_TYPEDEF(IDWriteFontFamily);
_WRL_PTR_TYPEDEF(IDWriteFontCollection);
_WRL_PTR_TYPEDEF(IDWriteLocalizedStrings);
_WRL_PTR_TYPEDEF(IDWriteTextFormat);
_WRL_PTR_TYPEDEF(IDWriteTextAnalyzer1);
_WRL_PTR_TYPEDEF(IDWriteTextLayout);
_WRL_PTR_TYPEDEF(IDWriteTextLayout1);
_WRL_PTR_TYPEDEF(IDWriteFontFile);
//_WRL_PTR_TYPEDEF(IDWriteFontFile);

// Direct2D

_WRL_PTR_TYPEDEF(ID2D1AnalysisTransform);
_WRL_PTR_TYPEDEF(ID2D1Bitmap);
_WRL_PTR_TYPEDEF(ID2D1Bitmap1);
_WRL_PTR_TYPEDEF(ID2D1BitmapBrush);
_WRL_PTR_TYPEDEF(ID2D1BitmapBrush1);
_WRL_PTR_TYPEDEF(ID2D1BitmapRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1BlendTransform);
_WRL_PTR_TYPEDEF(ID2D1BorderTransform);
_WRL_PTR_TYPEDEF(ID2D1BoundsAdjustmentTransform);
_WRL_PTR_TYPEDEF(ID2D1Brush);
_WRL_PTR_TYPEDEF(ID2D1ColorContext);
_WRL_PTR_TYPEDEF(ID2D1CommandList);
_WRL_PTR_TYPEDEF(ID2D1CommandSink);
_WRL_PTR_TYPEDEF(ID2D1ComputeInfo);
_WRL_PTR_TYPEDEF(ID2D1ComputeTransform);
_WRL_PTR_TYPEDEF(ID2D1ConcreteTransform);
_WRL_PTR_TYPEDEF(ID2D1DCRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1Device);
_WRL_PTR_TYPEDEF(ID2D1Device1);
_WRL_PTR_TYPEDEF(ID2D1DeviceContext);
_WRL_PTR_TYPEDEF(ID2D1DeviceContext1);
_WRL_PTR_TYPEDEF(ID2D1DrawInfo);
_WRL_PTR_TYPEDEF(ID2D1DrawingStateBlock);
_WRL_PTR_TYPEDEF(ID2D1DrawingStateBlock1);
_WRL_PTR_TYPEDEF(ID2D1DrawTransform);
_WRL_PTR_TYPEDEF(ID2D1Effect);
_WRL_PTR_TYPEDEF(ID2D1EffectContext);
_WRL_PTR_TYPEDEF(ID2D1EffectImpl);
_WRL_PTR_TYPEDEF(ID2D1EllipseGeometry);
_WRL_PTR_TYPEDEF(ID2D1Factory);
_WRL_PTR_TYPEDEF(ID2D1Factory1);
_WRL_PTR_TYPEDEF(ID2D1Factory2);
_WRL_PTR_TYPEDEF(ID2D1GdiInteropRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1GdiMetafile);
_WRL_PTR_TYPEDEF(ID2D1GdiMetafileSink);
_WRL_PTR_TYPEDEF(ID2D1Geometry);
_WRL_PTR_TYPEDEF(ID2D1GeometryGroup);
_WRL_PTR_TYPEDEF(ID2D1GeometrySink);
_WRL_PTR_TYPEDEF(ID2D1GradientStopCollection);
_WRL_PTR_TYPEDEF(ID2D1GradientStopCollection1);
_WRL_PTR_TYPEDEF(ID2D1HwndRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1Image);
_WRL_PTR_TYPEDEF(ID2D1ImageBrush);
_WRL_PTR_TYPEDEF(ID2D1Layer);
_WRL_PTR_TYPEDEF(ID2D1LinearGradientBrush);
_WRL_PTR_TYPEDEF(ID2D1Mesh);
//_WRL_PTR_TYPEDEF(ID2D1MultiThread);
_WRL_PTR_TYPEDEF(ID2D1OffsetTransform);
_WRL_PTR_TYPEDEF(ID2D1PathGeometry);
_WRL_PTR_TYPEDEF(ID2D1PathGeometry1);
_WRL_PTR_TYPEDEF(ID2D1PrintControl);
_WRL_PTR_TYPEDEF(ID2D1Properties);
_WRL_PTR_TYPEDEF(ID2D1RadialGradientBrush);
_WRL_PTR_TYPEDEF(ID2D1RectangleGeometry);
_WRL_PTR_TYPEDEF(ID2D1RenderInfo);
_WRL_PTR_TYPEDEF(ID2D1RenderTarget);
_WRL_PTR_TYPEDEF(ID2D1Resource);
_WRL_PTR_TYPEDEF(ID2D1ResourceTexture);
_WRL_PTR_TYPEDEF(ID2D1RoundedRectangleGeometry);
_WRL_PTR_TYPEDEF(ID2D1SimplifiedGeometrySink);
_WRL_PTR_TYPEDEF(ID2D1SolidColorBrush);
_WRL_PTR_TYPEDEF(ID2D1SourceTransform);
_WRL_PTR_TYPEDEF(ID2D1StrokeStyle);
_WRL_PTR_TYPEDEF(ID2D1StrokeStyle1);
_WRL_PTR_TYPEDEF(ID2D1TessellationSink);
_WRL_PTR_TYPEDEF(ID2D1Transform);
_WRL_PTR_TYPEDEF(ID2D1TransformedGeometry);
_WRL_PTR_TYPEDEF(ID2D1TransformGraph);
_WRL_PTR_TYPEDEF(ID2D1TransformNode);
_WRL_PTR_TYPEDEF(ID2D1VertexBuffer);

// WIC

_WRL_PTR_TYPEDEF(IWICImagingFactory);
_WRL_PTR_TYPEDEF(IWICImagingFactory2);
_WRL_PTR_TYPEDEF(IWICBitmapDecoder);
_WRL_PTR_TYPEDEF(IWICBitmapFrameDecode);
_WRL_PTR_TYPEDEF(IWICStream);
_WRL_PTR_TYPEDEF(IWICFormatConverter);
_WRL_PTR_TYPEDEF(IWICBitmapScaler);
_WRL_PTR_TYPEDEF(ITaskbarList3);

// DXGI 

_WRL_PTR_TYPEDEF(IDXGIAdapter);
_WRL_PTR_TYPEDEF(IDXGIAdapter1);
_WRL_PTR_TYPEDEF(IDXGIAdapter2);
_WRL_PTR_TYPEDEF(IDXGIDebug);
_WRL_PTR_TYPEDEF(IDXGIDevice);
_WRL_PTR_TYPEDEF(IDXGIDevice1);
_WRL_PTR_TYPEDEF(IDXGIDevice2);
_WRL_PTR_TYPEDEF(IDXGIDevice3);
_WRL_PTR_TYPEDEF(IDXGIDeviceSubObject);
_WRL_PTR_TYPEDEF(IDXGIDisplayControl);
_WRL_PTR_TYPEDEF(IDXGIFactory);
_WRL_PTR_TYPEDEF(IDXGIFactory1);
_WRL_PTR_TYPEDEF(IDXGIFactory2);
_WRL_PTR_TYPEDEF(IDXGIFactory3);
_WRL_PTR_TYPEDEF(IDXGIInfoQueue);
_WRL_PTR_TYPEDEF(IDXGIKeyedMutex);
_WRL_PTR_TYPEDEF(IDXGIObject);
_WRL_PTR_TYPEDEF(IDXGIOutput);
_WRL_PTR_TYPEDEF(IDXGIOutput1);
_WRL_PTR_TYPEDEF(IDXGIOutput2);
_WRL_PTR_TYPEDEF(IDXGIOutputDuplication);
_WRL_PTR_TYPEDEF(IDXGIResource);
_WRL_PTR_TYPEDEF(IDXGIResource1);
_WRL_PTR_TYPEDEF(IDXGISurface);
_WRL_PTR_TYPEDEF(IDXGISurface1);
_WRL_PTR_TYPEDEF(IDXGISurface2);
_WRL_PTR_TYPEDEF(IDXGISwapChain);
_WRL_PTR_TYPEDEF(IDXGISwapChain1);

// Direct3D

_WRL_PTR_TYPEDEF(ID3D11Asynchronous);
_WRL_PTR_TYPEDEF(ID3D11BlendState);
_WRL_PTR_TYPEDEF(ID3D11BlendState1);
_WRL_PTR_TYPEDEF(ID3D11CommandList);
_WRL_PTR_TYPEDEF(ID3D11Counter);
_WRL_PTR_TYPEDEF(ID3D11DepthStencilState);
_WRL_PTR_TYPEDEF(ID3D11Device);
_WRL_PTR_TYPEDEF(ID3D11Device1);
_WRL_PTR_TYPEDEF(ID3D11Device2);
_WRL_PTR_TYPEDEF(ID3D11DeviceChild);
_WRL_PTR_TYPEDEF(ID3D11DeviceContext);
_WRL_PTR_TYPEDEF(ID3D11DeviceContext1);
_WRL_PTR_TYPEDEF(ID3D11DeviceContext2);
_WRL_PTR_TYPEDEF(ID3DDeviceContextState);
_WRL_PTR_TYPEDEF(ID3D11InputLayout);
_WRL_PTR_TYPEDEF(ID3D11Predicate);
_WRL_PTR_TYPEDEF(ID3D11Query);
_WRL_PTR_TYPEDEF(ID3D11RasterizerState);
_WRL_PTR_TYPEDEF(ID3D11RasterizerState1);
_WRL_PTR_TYPEDEF(ID3D11SamplerState);
_WRL_PTR_TYPEDEF(ID3D11Debug);
_WRL_PTR_TYPEDEF(ID3D11InfoQueue);
_WRL_PTR_TYPEDEF(ID3D11RefDefaultTrackingOptions);
_WRL_PTR_TYPEDEF(ID3D11RefTrackingOptions);
_WRL_PTR_TYPEDEF(ID3D11SwitchToRef);
_WRL_PTR_TYPEDEF(ID3D11TracingDevice);
_WRL_PTR_TYPEDEF(ID3D11Buffer);
_WRL_PTR_TYPEDEF(ID3D11DepthStencilView);
_WRL_PTR_TYPEDEF(ID3D11RenderTargetView);
_WRL_PTR_TYPEDEF(ID3D11Resource);
_WRL_PTR_TYPEDEF(ID3D11ShaderResourceView);
_WRL_PTR_TYPEDEF(ID3D11Texture1D);
_WRL_PTR_TYPEDEF(ID3D11Texture2D);
_WRL_PTR_TYPEDEF(ID3D11Texture3D);
_WRL_PTR_TYPEDEF(ID3D11UnorderedAccessView);
_WRL_PTR_TYPEDEF(ID3D11View);
_WRL_PTR_TYPEDEF(ID3D11ClassInstance);
_WRL_PTR_TYPEDEF(ID3D11ClassLinkage);
_WRL_PTR_TYPEDEF(ID3D11ComputeShader);
_WRL_PTR_TYPEDEF(ID3D11DomainShader);
_WRL_PTR_TYPEDEF(ID3D11GeometryShader);
_WRL_PTR_TYPEDEF(ID3D11HullShader);
_WRL_PTR_TYPEDEF(ID3D11PixelShader);
_WRL_PTR_TYPEDEF(ID3D11ShaderReflection);
_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionConstantBuffer);
_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionType);
_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionVariable);
_WRL_PTR_TYPEDEF(ID3D11ShaderTrace);
_WRL_PTR_TYPEDEF(ID3D11ShaderTraceFactory);
_WRL_PTR_TYPEDEF(ID3D11VertexShader);
_WRL_PTR_TYPEDEF(ID3DBlob);
_WRL_PTR_TYPEDEF(ID3DInclude);
_WRL_PTR_TYPEDEF(ID3DUserDefinedAnnotation);

// DirectComposition

_WRL_PTR_TYPEDEF(IDCompositionAnimation);
_WRL_PTR_TYPEDEF(IDCompositionClip);
_WRL_PTR_TYPEDEF(IDCompositionDevice);
_WRL_PTR_TYPEDEF(IDCompositionDevice2);
_WRL_PTR_TYPEDEF(IDCompositionDeviceDebug);
_WRL_PTR_TYPEDEF(IDCompositionDesktopDevice);
_WRL_PTR_TYPEDEF(IDCompositionEffect);
_WRL_PTR_TYPEDEF(IDCompositionEffectGroup);
_WRL_PTR_TYPEDEF(IDCompositionMatrixTransform);
_WRL_PTR_TYPEDEF(IDCompositionMatrixTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionRectangleClip);
_WRL_PTR_TYPEDEF(IDCompositionRotateTransform);
_WRL_PTR_TYPEDEF(IDCompositionRotateTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionScaleTransform);
_WRL_PTR_TYPEDEF(IDCompositionScaleTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionSkewTransform);
_WRL_PTR_TYPEDEF(IDCompositionSurface);
_WRL_PTR_TYPEDEF(IDCompositionSurfaceFactory);
_WRL_PTR_TYPEDEF(IDCompositionTarget);
_WRL_PTR_TYPEDEF(IDCompositionTransform);
_WRL_PTR_TYPEDEF(IDCompositionTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionTranslateTransform);
_WRL_PTR_TYPEDEF(IDCompositionTranslateTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionVirtualSurface);
_WRL_PTR_TYPEDEF(IDCompositionVisual);
_WRL_PTR_TYPEDEF(IDCompositionVisual2);

// WIC

_WRL_PTR_TYPEDEF(IWICImagingFactory);
_WRL_PTR_TYPEDEF(IWICBitmapDecoder);
_WRL_PTR_TYPEDEF(IWICBitmapFrameDecode);
_WRL_PTR_TYPEDEF(IWICStream);
_WRL_PTR_TYPEDEF(IWICFormatConverter);
_WRL_PTR_TYPEDEF(IWICBitmapScaler);
//_WRL_PTR_TYPEDEF(ITaskbarList3);

// DXGI 

//_WRL_PTR_TYPEDEF(IDXGIAdapter);
//_WRL_PTR_TYPEDEF(IDXGIAdapter1);
//_WRL_PTR_TYPEDEF(IDXGIAdapter2);
//_WRL_PTR_TYPEDEF(IDXGIDebug);
//_WRL_PTR_TYPEDEF(IDXGIDevice);
//_WRL_PTR_TYPEDEF(IDXGIDevice1);
//_WRL_PTR_TYPEDEF(IDXGIDevice2);
//_WRL_PTR_TYPEDEF(IDXGIDeviceSubObject);
//_WRL_PTR_TYPEDEF(IDXGIDisplayControl);
//_WRL_PTR_TYPEDEF(IDXGIFactory);
//_WRL_PTR_TYPEDEF(IDXGIFactory1);
//_WRL_PTR_TYPEDEF(IDXGIFactory2);
//_WRL_PTR_TYPEDEF(IDXGIInfoQueue);
//_WRL_PTR_TYPEDEF(IDXGIKeyedMutex);
//_WRL_PTR_TYPEDEF(IDXGIObject);
//_WRL_PTR_TYPEDEF(IDXGIOutput);
//_WRL_PTR_TYPEDEF(IDXGIOutput1);
//_WRL_PTR_TYPEDEF(IDXGIOutputDuplication);
//_WRL_PTR_TYPEDEF(IDXGIResource);
//_WRL_PTR_TYPEDEF(IDXGIResource1);
//_WRL_PTR_TYPEDEF(IDXGISurface);
//_WRL_PTR_TYPEDEF(IDXGISurface1);
//_WRL_PTR_TYPEDEF(IDXGISurface2);
//_WRL_PTR_TYPEDEF(IDXGISwapChain);
//_WRL_PTR_TYPEDEF(IDXGISwapChain1);
//
//// Direct3D
//
//_WRL_PTR_TYPEDEF(ID3D11Asynchronous);
//_WRL_PTR_TYPEDEF(ID3D11BlendState);
//_WRL_PTR_TYPEDEF(ID3D11BlendState1);
//_WRL_PTR_TYPEDEF(ID3D11CommandList);
//_WRL_PTR_TYPEDEF(ID3D11Counter);
//_WRL_PTR_TYPEDEF(ID3D11DepthStencilState);
//_WRL_PTR_TYPEDEF(ID3D11Device);
//_WRL_PTR_TYPEDEF(ID3D11Device1);
//_WRL_PTR_TYPEDEF(ID3D11DeviceChild);
//_WRL_PTR_TYPEDEF(ID3D11DeviceContext);
//_WRL_PTR_TYPEDEF(ID3D11DeviceContext1);
//_WRL_PTR_TYPEDEF(ID3D11DeviceContext2);
//_WRL_PTR_TYPEDEF(ID3DDeviceContextState);
//_WRL_PTR_TYPEDEF(ID3D11InputLayout);
//_WRL_PTR_TYPEDEF(ID3D11Predicate);
//_WRL_PTR_TYPEDEF(ID3D11Query);
//_WRL_PTR_TYPEDEF(ID3D11RasterizerState);
//_WRL_PTR_TYPEDEF(ID3D11RasterizerState1);
//_WRL_PTR_TYPEDEF(ID3D11SamplerState);
//_WRL_PTR_TYPEDEF(ID3D11Debug);
//_WRL_PTR_TYPEDEF(ID3D11InfoQueue);
//_WRL_PTR_TYPEDEF(ID3D11RefDefaultTrackingOptions);
//_WRL_PTR_TYPEDEF(ID3D11RefTrackingOptions);
//_WRL_PTR_TYPEDEF(ID3D11SwitchToRef);
//_WRL_PTR_TYPEDEF(ID3D11TracingDevice);
//_WRL_PTR_TYPEDEF(ID3D11Buffer);
//_WRL_PTR_TYPEDEF(ID3D11DepthStencilView);
//_WRL_PTR_TYPEDEF(ID3D11RenderTargetView);
//_WRL_PTR_TYPEDEF(ID3D11Resource);
//_WRL_PTR_TYPEDEF(ID3D11ShaderResourceView);
//_WRL_PTR_TYPEDEF(ID3D11Texture1D);
//_WRL_PTR_TYPEDEF(ID3D11Texture2D);
//_WRL_PTR_TYPEDEF(ID3D11Texture3D);
//_WRL_PTR_TYPEDEF(ID3D11UnorderedAccessView);
//_WRL_PTR_TYPEDEF(ID3D11View);
//_WRL_PTR_TYPEDEF(ID3D11ClassInstance);
//_WRL_PTR_TYPEDEF(ID3D11ClassLinkage);
//_WRL_PTR_TYPEDEF(ID3D11ComputeShader);
//_WRL_PTR_TYPEDEF(ID3D11DomainShader);
//_WRL_PTR_TYPEDEF(ID3D11GeometryShader);
//_WRL_PTR_TYPEDEF(ID3D11HullShader);
//_WRL_PTR_TYPEDEF(ID3D11PixelShader);
//_WRL_PTR_TYPEDEF(ID3D11ShaderReflection);
//_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionConstantBuffer);
//_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionType);
//_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionVariable);
//_WRL_PTR_TYPEDEF(ID3D11ShaderTrace);
//_WRL_PTR_TYPEDEF(ID3D11ShaderTraceFactory);
//_WRL_PTR_TYPEDEF(ID3D11VertexShader);
//_WRL_PTR_TYPEDEF(ID3DBlob);
//_WRL_PTR_TYPEDEF(ID3DInclude);
//_WRL_PTR_TYPEDEF(ID3DUserDefinedAnnotation);
//
//// DirectComposition
//
//_WRL_PTR_TYPEDEF(IDCompositionClip);
//_WRL_PTR_TYPEDEF(IDCompositionDevice);
//_WRL_PTR_TYPEDEF(IDCompositionEffect);
//_WRL_PTR_TYPEDEF(IDCompositionEffectGroup);
//_WRL_PTR_TYPEDEF(IDCompositionMatrixTransform);
//_WRL_PTR_TYPEDEF(IDCompositionMatrixTransform3D);
//_WRL_PTR_TYPEDEF(IDCompositionRectangleClip);
//_WRL_PTR_TYPEDEF(IDCompositionRotateTransform);
//_WRL_PTR_TYPEDEF(IDCompositionRotateTransform3D);
//_WRL_PTR_TYPEDEF(IDCompositionScaleTransform);
//_WRL_PTR_TYPEDEF(IDCompositionScaleTransform3D);
//_WRL_PTR_TYPEDEF(IDCompositionSkewTransform);
//_WRL_PTR_TYPEDEF(IDCompositionSurface);
//_WRL_PTR_TYPEDEF(IDCompositionTarget);
//_WRL_PTR_TYPEDEF(IDCompositionTransform);
//_WRL_PTR_TYPEDEF(IDCompositionTransform3D);
//_WRL_PTR_TYPEDEF(IDCompositionTranslateTransform);
//_WRL_PTR_TYPEDEF(IDCompositionTranslateTransform3D);
//_WRL_PTR_TYPEDEF(IDCompositionVirtualSurface);
//_WRL_PTR_TYPEDEF(IDCompositionVisual);

// Media Foundation

// Media Foundation Smart Pointers

_WRL_PTR_TYPEDEF(IAdvancedMediaCapture);//Enables advanced media capture.
_WRL_PTR_TYPEDEF(IAdvancedMediaCaptureInitializationSettings);// Provides initialization settings for advanced media capture.
_WRL_PTR_TYPEDEF(IAdvancedMediaCaptureSettings);//Provides settings for advanced media capture.
//_WRL_PTR_TYPEDEF(IDirect3DDeviceManager9);//Enables two threads to share the same Direct3D 9 device, and provides access to the DirectX Video Acceleration (DXVA) features of the device.
//_WRL_PTR_TYPEDEF(IDirectXVideoAccelerationService);//Provides DirectX Video Acceleration (DXVA) services from a Direct3D device.
//_WRL_PTR_TYPEDEF(IDirectXVideoDecoder);//Represents a DirectX Video Acceleration (DXVA) video decoder device.
//_WRL_PTR_TYPEDEF(IDirectXVideoDecoderService);//Provides access to DirectX Video Acceleration (DXVA) decoder services.
//_WRL_PTR_TYPEDEF(IDirectXVideoMemoryConfiguration);//Sets the type of video memory for uncompressed video surfaces.
//_WRL_PTR_TYPEDEF(IDirectXVideoProcessor);//Represents a DirectX Video Acceleration (DXVA) video processor device.
//_WRL_PTR_TYPEDEF(IDirectXVideoProcessorService);//Provides access to DirectX Video Acceleration (DXVA) video processing services.
_WRL_PTR_TYPEDEF(IEVRFilterConfig);//Sets the number of input pins on the DirectShow Enhanced Video Renderer (EVR) filter.
_WRL_PTR_TYPEDEF(IEVRFilterConfigEx);//Configures the DirectShow Enhanced Video Renderer (EVR) filter.
_WRL_PTR_TYPEDEF(IEVRTrustedVideoPlugin);//Enables a plug-in component for the enhanced video renderer (EVR) to work with protected media.
//_WRL_PTR_TYPEDEF(IEVRVideoStreamControl);//This interface is not supported.
_WRL_PTR_TYPEDEF(IMF2DBuffer);//Represents a buffer that contains a two-dimensional surface, such as a video frame. 
_WRL_PTR_TYPEDEF(IMF2DBuffer2);//Represents a buffer that contains a two-dimensional surface, such as a video frame.
_WRL_PTR_TYPEDEF(IMFActivate);//Enables the application to defer the creation of an object.
_WRL_PTR_TYPEDEF(IMFASFContentInfo);//Provides methods to work with the header section of files conforming to the Advanced Systems Format (ASF) specification. 
_WRL_PTR_TYPEDEF(IMFASFIndexer);// Provides methods to work with indexes in Systems Format (ASF) files.
_WRL_PTR_TYPEDEF(IMFASFMultiplexer);//Provides methods to create Advanced Systems Format (ASF) data packets.
_WRL_PTR_TYPEDEF(IMFASFMutualExclusion);//Configures an Advanced Systems Format (ASF) mutual exclusion object, which manages information about a group of streams in an ASF profile that are mutually exclusive.
_WRL_PTR_TYPEDEF(IMFASFProfile);//Manages an Advanced Systems Format (ASF) profile.
_WRL_PTR_TYPEDEF(IMFASFSplitter);//Provides methods to read data from an Advanced Systems Format (ASF) file.
_WRL_PTR_TYPEDEF(IMFASFStreamConfig);//Configures the settings of a stream in an ASF file.
_WRL_PTR_TYPEDEF(IMFASFStreamPrioritization);
_WRL_PTR_TYPEDEF(IMFASFStreamSelector);//Selects streams in an Advanced Systems Format (ASF) file, based on the mutual exclusion information in the ASF header.
_WRL_PTR_TYPEDEF(IMFAsyncCallback);//Callback interface to notify the application when an asynchronous method completes. 
_WRL_PTR_TYPEDEF(IMFAsyncCallbackLogging);//Provides logging information about the parent object the async callback is associated with.
_WRL_PTR_TYPEDEF(IMFAsyncResult);//Provides information about the result of an asynchronous operation. 
_WRL_PTR_TYPEDEF(IMFAttributes);//Provides a generic way to store key/value pairs on an object.
_WRL_PTR_TYPEDEF(IMFAudioMediaType);//IMFAudioMediaType is no longer available for use as of Windows 7.
_WRL_PTR_TYPEDEF(IMFAudioPolicy); // Configures the audio session that is associated with the streaming audio renderer (SAR).
_WRL_PTR_TYPEDEF(IMFAudioStreamVolume); // Controls the volume levels of individual audio channels.
_WRL_PTR_TYPEDEF(IMFByteStream); // Represents a byte stream from some data source, which might be a local file, a network file, or some other source.
_WRL_PTR_TYPEDEF(IMFByteStreamBuffering); // Controls how a byte stream buffers data from a network. 
_WRL_PTR_TYPEDEF(IMFByteStreamCacheControl); // Controls how a network byte stream transfers data to a local cache.
_WRL_PTR_TYPEDEF(IMFByteStreamCacheControl2); // Controls how a network byte stream transfers data to a local cache.
_WRL_PTR_TYPEDEF(IMFByteStreamHandler); // Creates a media source from a byte stream. 
_WRL_PTR_TYPEDEF(IMFByteStreamProxyClassFactory); // Creates a proxy to a byte stream.
_WRL_PTR_TYPEDEF(IMFByteStreamTimeSeek); // Seeks a byte stream by time position.
_WRL_PTR_TYPEDEF(IMFCaptureEngine); // Controls one or more capture device_infos.
_WRL_PTR_TYPEDEF(IMFCaptureEngineClassFactory); // Creates an instance of the capture engine.
_WRL_PTR_TYPEDEF(IMFCaptureEngineOnEventCallback); // Callback interface for receiving events from the capture engine.
_WRL_PTR_TYPEDEF(IMFCaptureEngineOnSampleCallback); // Callback interface to receive data from the capture engine.
_WRL_PTR_TYPEDEF(IMFCapturePhotoSink); // Controls the photo sink.
_WRL_PTR_TYPEDEF(IMFCapturePreviewSink); // Controls the preview sink.
_WRL_PTR_TYPEDEF(IMFCaptureRecordSink); // Controls the recording sink.
_WRL_PTR_TYPEDEF(IMFCaptureSink); // Controls a capture sink, which is an object that receives one or more streams from a capture device.
_WRL_PTR_TYPEDEF(IMFCaptureSource); // Controls the capture source object. The capture source manages the audio and video capture device_infos.
_WRL_PTR_TYPEDEF(IMFClock); // Provides timing information from a clock in Microsoft Media Foundation.
_WRL_PTR_TYPEDEF(IMFClockStateSink); // Receives state-change notifications from the presentation clock. 
_WRL_PTR_TYPEDEF(IMFCollection); // Represents a generic collection of IUnknown pointers. 
_WRL_PTR_TYPEDEF(IMFContentEnabler); // Implements one step that must be performed for the user to access media content.
_WRL_PTR_TYPEDEF(IMFContentProtectionManager); // Enables playback of protected content by providing the application with a pointer to a content enabler object.
_WRL_PTR_TYPEDEF(IMFDesiredSample); // Enables the presenter for the enhanced video renderer (EVR) to request a specific frame from the video mixer.
_WRL_PTR_TYPEDEF(IMFDLNASinkInit); // _WRL_PTR_TYPEDEF(Initializes the Digital Living Network Alliance (DLNA) media sink.); // 
_WRL_PTR_TYPEDEF(IMFDRMNetHelper); // Configures Windows Media Digital Rights Management (DRM) for Network device_infos on a network sink.
_WRL_PTR_TYPEDEF(IMFDXGIBuffer); // Represents a buffer that contains a Microsoft DirectX Graphics Infrastructure (DXGI) surface.
_WRL_PTR_TYPEDEF(IMFDXGIDeviceManager); // Enables two threads to share the same Microsoft Direct3D 11 device.
_WRL_PTR_TYPEDEF(IMFFieldOfUseMFTUnlock); // Enables an application to use a Media Foundation transform (MFT) that has restrictions on its use.
_WRL_PTR_TYPEDEF(IMFFinalizableMediaSink); // Optionally supported by media sinks to perform required tasks before shutdown.
_WRL_PTR_TYPEDEF(IMFGetService); // Queries an object for a specified service interface. 
_WRL_PTR_TYPEDEF(IMFImageSharingEngine); // Enables image sharing.
_WRL_PTR_TYPEDEF(IMFInputTrustAuthority); // Enables other components in the protected media path (PMP) to use the input protection system provided by an input trust authorities (ITA).
_WRL_PTR_TYPEDEF(IMFLocalMFTRegistration); // Registers Media Foundation transforms (MFTs) in the caller's process.
_WRL_PTR_TYPEDEF(IMFMediaBuffer); // Represents a block of memory that contains media data.
_WRL_PTR_TYPEDEF(IMFMediaEngine); // Enables an application to play audio or video files.
_WRL_PTR_TYPEDEF(IMFMediaEngineClassFactory); // Creates an instance of the Media Engine.
_WRL_PTR_TYPEDEF(IMFMediaEngineEx); // Extends the IMFMediaEngine interface.
_WRL_PTR_TYPEDEF(IMFMediaEngineExtension); // Enables an application to load media resources in the Media Engine.
_WRL_PTR_TYPEDEF(IMFMediaEngineNotify); // Callback interface for the IMFMediaEngine interface. 
_WRL_PTR_TYPEDEF(IMFMediaEngineProtectedContent); // Enables the Media Engine to play protected video content.
_WRL_PTR_TYPEDEF(IMFMediaEngineSrcElements); // Provides the Media Engine with a list of media resources.
_WRL_PTR_TYPEDEF(IMFMediaError); // Provides the current error status for the Media Engine.
_WRL_PTR_TYPEDEF(IMFMediaEvent); // Represents an event generated by a Media Foundation object. Use this interface to get information about the event.
_WRL_PTR_TYPEDEF(IMFMediaEventGenerator); // Retrieves events from any Media Foundation object that generates events. 
_WRL_PTR_TYPEDEF(IMFMediaEventQueue); // Provides an event queue for applications that need to implement the IMFMediaEventGenerator interface.
_WRL_PTR_TYPEDEF(IMFMediaSession); // Provides playback controls for protected and unprotected content.
_WRL_PTR_TYPEDEF(IMFMediaSharingEngine); // Enables media sharing.
_WRL_PTR_TYPEDEF(IMFMediaSink); // Implemented by media sink objects.
_WRL_PTR_TYPEDEF(IMFMediaSinkPreroll); // Enables a media sink to receive samples before the presentation clock is started.
_WRL_PTR_TYPEDEF(IMFMediaSource); // Implemented by media source objects.
_WRL_PTR_TYPEDEF(IMFMediaSourceEx); // Extends the IMFMediaSource interface to provide additional capabilities for a media source.
_WRL_PTR_TYPEDEF(IMFMediaSourcePresentationProvider); // Provides notifications to the sequencer source.
_WRL_PTR_TYPEDEF(IMFMediaSourceTopologyProvider); // Enables an application to get a topology from the sequencer source.
_WRL_PTR_TYPEDEF(IMFMediaStream); // Represents one stream in a media source. 
_WRL_PTR_TYPEDEF(IMFMediaTimeRange); // Represents a list of time ranges, where each range is defined by a start and end time.
_WRL_PTR_TYPEDEF(IMFMediaType); // Represents a description of a media format. 
_WRL_PTR_TYPEDEF(IMFMediaTypeHandler); // Gets and sets media types on an object, such as a media source or media sink. 
_WRL_PTR_TYPEDEF(IMFMetadata); // Manages metadata for an object.
_WRL_PTR_TYPEDEF(IMFMetadataProvider); // Gets metadata from a media source or other object.
_WRL_PTR_TYPEDEF(IMFNetCredential); // Sets and retrieves user-name and password information for authentication purposes. 
_WRL_PTR_TYPEDEF(IMFNetCredentialCache); // Gets credentials from the credential cache.
_WRL_PTR_TYPEDEF(IMFNetCredentialManager); // Implemented by applications to provide user credentials for a network source.
_WRL_PTR_TYPEDEF(IMFNetProxyLocator); // Determines the proxy to use when connecting to a server.
_WRL_PTR_TYPEDEF(IMFNetProxyLocatorFactory); // Creates a proxy locator object, which determines the proxy to use.
_WRL_PTR_TYPEDEF(IMFNetResourceFilter); // Notifies the application when a byte stream requests a URL, and enables the application to block URL redirection.
_WRL_PTR_TYPEDEF(IMFNetSchemeHandlerConfig); // Configures a network scheme plug-in. 
_WRL_PTR_TYPEDEF(IMFObjectReferenceStream); // Marshals an interface pointer to and from a stream. Stream objects that support IStream can expose this interface to provide custom marshaling for interface pointers.
_WRL_PTR_TYPEDEF(IMFOutputPolicy); // Encapsulates a usage policy from an input trust authority (ITA).
_WRL_PTR_TYPEDEF(IMFOutputSchema); // Encapsulates information about an output protection system and its corresponding configuration data.
_WRL_PTR_TYPEDEF(IMFOutputTrustAuthority); // Encapsulates the functionality of one or more output protection systems that a trusted output supports.
_WRL_PTR_TYPEDEF(IMFPluginControl); // Controls how media sources and transforms are enumerated in Media Foundation.
_WRL_PTR_TYPEDEF(IMFPluginControl2); // Controls how media sources and transforms are enumerated in Media Foundation.
_WRL_PTR_TYPEDEF(IMFPMPClient); // Enables a media source to receive a pointer to the IMFPMPHost interface. 
_WRL_PTR_TYPEDEF(IMFPMPClientApp); // Provides a mechanism for a media source to implement content protection functionality in a Windows Store apps.
_WRL_PTR_TYPEDEF(IMFPMPHost); // Enables a media source in the application process to create objects in the protected media path (PMP) process.
_WRL_PTR_TYPEDEF(IMFPMPHostApp); // Allows a media source to create a Windows Runtime object in the Protected Media Path (PMP) process. 
_WRL_PTR_TYPEDEF(IMFPMPServer); // Enables two instances of the Media Session to share the same protected media path (PMP) process. 
_WRL_PTR_TYPEDEF(IMFPresentationClock); // Represents a presentation clock, which is used to schedule when samples are rendered and to synchronize multiple streams.
_WRL_PTR_TYPEDEF(IMFPresentationDescriptor); // Describes the details of a presentation. A presentation is a set of related media streams that share a common presentation time. 
_WRL_PTR_TYPEDEF(IMFPresentationTimeSource); // Provides the clock times for the presentation clock. 
_WRL_PTR_TYPEDEF(IMFProtectedEnvironmentAccess); // Provides a method that allows content protection systems to perform a handshake with the protected environment. This is needed because the CreateFile and DeviceIoControl APIs are not available to Windows Store apps.
_WRL_PTR_TYPEDEF(IMFQualityAdvise); // Enables the quality manager to adjust the audio or video quality of a component in the pipeline.
_WRL_PTR_TYPEDEF(IMFQualityAdvise2); // Enables a pipeline object to adjust its own audio or video quality, in response to quality messages.
_WRL_PTR_TYPEDEF(IMFQualityAdviseLimits); // Queries an object for the number of quality modes it supports.
_WRL_PTR_TYPEDEF(IMFQualityManager); // Adjusts playback quality. This interface is exposed by the quality manager. 
_WRL_PTR_TYPEDEF(IMFRateControl); // Gets or sets the playback rate. 
_WRL_PTR_TYPEDEF(IMFRateSupport); // Queries the range of playback rates that are supported, including reverse playback.
_WRL_PTR_TYPEDEF(IMFReadWriteClassFactory); // Creates an instance of either the sink writer or the source reader.
_WRL_PTR_TYPEDEF(IMFRealTimeClient); // Notifies a pipeline object to register itself with the Multimedia Class Scheduler Service (MMCSS).
_WRL_PTR_TYPEDEF(IMFRealTimeClientEx); // Notifies a pipeline object to register itself with the Multimedia Class Scheduler Service (MMCSS). 
_WRL_PTR_TYPEDEF(IMFRemoteAsyncCallback); // Used by the Media Foundation proxy/stub DLL to marshal certain asynchronous method calls across process boundaries.Applications do not use or implement this interface.
_WRL_PTR_TYPEDEF(IMFRemoteDesktopPlugin); // Modifies a topology for use in a Terminal Services environment. 
_WRL_PTR_TYPEDEF(IMFRemoteProxy); // Exposed by objects that act as a proxy for a remote object.
_WRL_PTR_TYPEDEF(IMFSAMIStyle); // Sets and retrieves Synchronized Accessible Media Interchange (SAMI) styles on the SAMI Media Source. 
_WRL_PTR_TYPEDEF(IMFSample); // Represents a media sample, which is a container object for media data.
_WRL_PTR_TYPEDEF(IMFSampleGrabberSinkCallback); // Callback interface to get media data from the sample-grabber sink. 
_WRL_PTR_TYPEDEF(IMFSampleGrabberSinkCallback2); // Extends the IMFSampleGrabberSinkCallback interface.
_WRL_PTR_TYPEDEF(IMFSampleOutputStream); // Writes media samples to a byte stream.
_WRL_PTR_TYPEDEF(IMFSampleProtection); // Provides encryption for media data inside the protected media path (PMP). 
_WRL_PTR_TYPEDEF(IMFSaveJob); // Persists media data from a source byte stream to an application-provided byte stream.
_WRL_PTR_TYPEDEF(IMFSchemeHandler); // Creates a media source or a byte stream from a URL. 
_WRL_PTR_TYPEDEF(IMFSecureChannel); // Establishes a one-way secure channel between two objects. 
_WRL_PTR_TYPEDEF(IMFSeekInfo); // For a particular seek position, gets the two nearest key frames.
_WRL_PTR_TYPEDEF(IMFSequencerSource); // Implemented by the Sequencer Source.
_WRL_PTR_TYPEDEF(IMFSharingEngineClassFactory); // Creates an instance of the media sharing engine.
_WRL_PTR_TYPEDEF(IMFShutdown); // Exposed by some Media Foundation objects that must be explicitly shut down. 
_WRL_PTR_TYPEDEF(IMFSignedLibrary); // Provides a method that allows content protection systems to get the procedure address of a function in the signed library. This method provides the same functionality as GetProcAddress which is not available to Windows Store apps.
_WRL_PTR_TYPEDEF(IMFSimpleAudioVolume); // Controls the master volume level of the audio session associated with the streaming audio renderer (SAR) and the audio capture source.
_WRL_PTR_TYPEDEF(IMFSinkWriter); // Implemented by the Media Foundation sink writer object.
_WRL_PTR_TYPEDEF(IMFSinkWriterCallback); // Callback interface for the Media Foundation sink writer. 
_WRL_PTR_TYPEDEF(IMFSinkWriterEx); // Extends the IMFSinkWriter interface.
_WRL_PTR_TYPEDEF(IMFSourceOpenMonitor); // Callback interface to receive notifications from a network source on the progress of an asynchronous open operation.
_WRL_PTR_TYPEDEF(IMFSourceReader); // Implemented by the Media Foundation source reader object.
_WRL_PTR_TYPEDEF(IMFSourceReaderCallback); // Callback interface for the Media Foundation source reader.
_WRL_PTR_TYPEDEF(IMFSourceReaderEx); // Extends the IMFSourceReader interface.
_WRL_PTR_TYPEDEF(IMFSourceResolver); // Creates a media source from a URL or a byte stream.
_WRL_PTR_TYPEDEF(IMFSSLCertificateManager); // _WRL_PTR_TYPEDEF(Implemented by a client and called by Media Foundation to get the client Secure Sockets Layer (SSL) certificate requested by the server.); // 
_WRL_PTR_TYPEDEF(IMFStreamDescriptor); // Gets information about one stream in a media source. 
_WRL_PTR_TYPEDEF(IMFStreamingSinkConfig); // Passes configuration information to the media sinks that are used for streaming the content.
_WRL_PTR_TYPEDEF(IMFStreamSink); // Represents a stream on a media sink object.
_WRL_PTR_TYPEDEF(IMFSystemId); // Provides a method that retireves system id data.
_WRL_PTR_TYPEDEF(IMFTimecodeTranslate); // Converts between Society of Motion Picture and Television Engineers (SMPTE) time codes and 100-nanosecond time units.
_WRL_PTR_TYPEDEF(IMFTimer); // Provides a timer that invokes a callback at a specified time.
_WRL_PTR_TYPEDEF(IMFTopoLoader); // Converts a partial topology into a full topology.
_WRL_PTR_TYPEDEF(IMFTopology); // Represents a topology. A topology describes a collection of media sources, sinks, and transforms that are connected in a certain order.
_WRL_PTR_TYPEDEF(IMFTopologyNode); // Represents a node in a topology.
_WRL_PTR_TYPEDEF(IMFTopologyNodeAttributeEditor); // Updates the attributes of one or more nodes in the Media Session's current topology.
_WRL_PTR_TYPEDEF(IMFTopologyServiceLookup); // Enables a custom video mixer or video presenter to get interface pointers from the Enhanced Video Renderer (EVR).
_WRL_PTR_TYPEDEF(IMFTopologyServiceLookupClient); // Initializes a video mixer or presenter.
_WRL_PTR_TYPEDEF(IMFTrackedSample); // Tracks the reference counts on a video media sample.
_WRL_PTR_TYPEDEF(IMFTranscodeProfile); // Implemented by the transcode profile object.
_WRL_PTR_TYPEDEF(IMFTranscodeSinkInfoProvider); // Implemented by the transcode sink activation object.
_WRL_PTR_TYPEDEF(IMFTransform); // Implemented by all Media Foundation Transforms (MFTs).
_WRL_PTR_TYPEDEF(IMFTrustedInput); // _WRL_PTR_TYPEDEF(Implemented by components that provide input trust authorities (ITAs). This interface is used to get the ITA for each of the component's streams.); // 
_WRL_PTR_TYPEDEF(IMFTrustedOutput); // Implemented by components that provide output trust authorities (OTAs).
_WRL_PTR_TYPEDEF(IMFVideoDeviceID); // Returns the device identifier supported by a video renderer component.
_WRL_PTR_TYPEDEF(IMFVideoDisplayControl); // Controls how the Enhanced Video Renderer (EVR) displays video.
_WRL_PTR_TYPEDEF(IMFVideoMediaType); // Represents a description of a video format.
//_WRL_PTR_TYPEDEF(IMFVideoMixerBitmap); // Alpha-blends a static bitmap image with the video displayed by the Enhanced Video Renderer (EVR).
_WRL_PTR_TYPEDEF(IMFVideoMixerControl); // Controls how the Enhanced Video Renderer (EVR) mixes video substreams.
_WRL_PTR_TYPEDEF(IMFVideoMixerControl2); // Controls preferences for video deinterlacing.
_WRL_PTR_TYPEDEF(IMFVideoPositionMapper); // Maps a position on an input video stream to the corresponding position on an output video stream.
_WRL_PTR_TYPEDEF(IMFVideoPresenter); // Represents a video presenter. A video presenter is an object that receives video frames, typically from a video mixer, and presents them in some way, typically by rendering them to the display.
//_WRL_PTR_TYPEDEF(IMFVideoProcessor); // Controls video processing in the Enhanced Video Renderer (EVR).
_WRL_PTR_TYPEDEF(IMFVideoProcessorControl); // Configures the Video Processor MFT.
_WRL_PTR_TYPEDEF(IMFVideoRenderer); // Sets a new mixer or presenter for the Enhanced Video Renderer (EVR).
_WRL_PTR_TYPEDEF(IMFVideoSampleAllocator); // Allocates video samples for a video media sink.
_WRL_PTR_TYPEDEF(IMFVideoSampleAllocatorCallback); // Enables an application to track video samples allocated by the enhanced video renderer (EVR).
_WRL_PTR_TYPEDEF(IMFVideoSampleAllocatorEx); // Allocates video samples that contain Direct3D 11 texture surfaces.
_WRL_PTR_TYPEDEF(IMFVideoSampleAllocatorNotify); // The callback for the IMFVideoSampleAllocatorCallback interface.
_WRL_PTR_TYPEDEF(IMFWorkQueueServices); // Controls the work queues created by the Media Session.
_WRL_PTR_TYPEDEF(IMFWorkQueueServicesEx); // Extends the IMFWorkQueueServices interface.
_WRL_PTR_TYPEDEF(IPlayToControl); // Enables the PlayToConnection object to connect to a media element.
_WRL_PTR_TYPEDEF(IPlayToSourceClassFactory); // Creates an instance of the PlayToSource object.
_WRL_PTR_TYPEDEF(IWMCodecLeakyBucket); // Configures the "leaky bucket" parameters on a video encoder.
_WRL_PTR_TYPEDEF(IWMCodecOutputTimestamp); // Gets the time stamp of the next video frame to be decoded.
_WRL_PTR_TYPEDEF(IWMCodecPrivateData); // Gets the private codec data that must be appended to the output media type. This codec data is required for properly decoding Windows Media Video content.
_WRL_PTR_TYPEDEF(IWMCodecProps); // Provides methods that retrieve format-specific codec properties.
_WRL_PTR_TYPEDEF(IWMCodecStrings); // Retrieves names and descriptive strings for codecs and formats.
_WRL_PTR_TYPEDEF(IWMColorConvProps); // Sets properties on the color converter DSP. 
_WRL_PTR_TYPEDEF(IWMResamplerProps); // Sets properties on the audio resampler DSP. 
_WRL_PTR_TYPEDEF(IWMResizerProps); // Sets properties on the video resizer DSP.
_WRL_PTR_TYPEDEF(IWMSampleExtensionSupport); // Configures codec support for sample extensions. 
_WRL_PTR_TYPEDEF(IWMVideoDecoderHurryup); // Controls the speed of the video decoder.
_WRL_PTR_TYPEDEF(IWMVideoForceKeyFrame); // Forces the encoder to encode the current frame as a key frame. 

_WRL_PTR_TYPEDEF(ICodecAPI);
_WRL_PTR_TYPEDEF(IPropertyStore);



namespace sf {
  // プロパティ用バリアントのラッパ
  struct prop_variant 
  {
    prop_variant()
    {
      PropVariantInit(&value_);// 
    }

    ~prop_variant()
    {
      PropVariantClear(&value_);
    }

    PROPVARIANT* get(){ return &value_;};

    PROPVARIANT* operator &(){return get();}

    operator PROPVARIANT*() {return get();}

    PROPVARIANT& value() { return value_; }

  private:
    PROPVARIANT value_;
  };

  template <class Q>
  HRESULT GetEventObject(IMFMediaEvent *pEvent, Q **ppObject)
  {
    *ppObject = NULL;   // zero output

    PROPVARIANT var;
    HRESULT hr = pEvent->GetValue(&var);
    if (SUCCEEDED(hr))
    {
      if (var.vt == VT_UNKNOWN)
      {
        hr = var.punkVal->QueryInterface(ppObject);
      }
      else
      {
        hr = MF_E_INVALIDTYPE;
      }
      PropVariantClear(&var);
    }
    return hr;
  }

  //--------------------------------------------------------------------------------------
  // Helper for creating constant buffers
  //--------------------------------------------------------------------------------------
  template <class T>
  inline HRESULT CreateConstantBuffer(ID3D11Device* d3d_device, ID3D11Buffer** ppCB)
  {
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC Desc;
    Desc.Usage = D3D11_USAGE_DEFAULT;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = 0;
    Desc.MiscFlags = 0;
    Desc.ByteWidth = sizeof(T);
    hr = d3d_device->CreateBuffer(&Desc, NULL, ppCB);

    return hr;
  }

  //--------------------------------------------------------------------------------------
  // Helper for creating structured buffers with an SRV and UAV
  //--------------------------------------------------------------------------------------
  template <class T, class Device>
  inline HRESULT CreateStructuredBuffer(Device& d3d_device, UINT iNumElements, ID3D11BufferPtr& ppBuffer, ID3D11ShaderResourceViewPtr& ppSRV, ID3D11UnorderedAccessViewPtr& ppUAV, const T* pInitialData = NULL)
  {
    HRESULT hr = S_OK;

    // Create SB
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.ByteWidth = iNumElements * sizeof(T);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufferDesc.StructureByteStride = sizeof(T);

    D3D11_SUBRESOURCE_DATA bufferInitData;
    ZeroMemory(&bufferInitData, sizeof(bufferInitData));
    bufferInitData.pSysMem = pInitialData;
    CHK(d3d_device->CreateBuffer(&bufferDesc, (pInitialData) ? &bufferInitData : NULL, ppBuffer.ReleaseAndGetAddressOf()));

    // Create SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementWidth = iNumElements;
    CHK(d3d_device->CreateShaderResourceView(ppBuffer.Get(), &srvDesc, ppSRV.ReleaseAndGetAddressOf()));

    // Create UAV
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    ZeroMemory(&uavDesc, sizeof(uavDesc));
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.NumElements = iNumElements;
    CHK(d3d_device->CreateUnorderedAccessView(ppBuffer.Get(), &uavDesc, ppUAV.ReleaseAndGetAddressOf()));

    return hr;
  }

  //--------------------------------------------------------------------------------------
  // Helper for compiling shaders with D3DX11
  //--------------------------------------------------------------------------------------
  inline void create_shader_blob_from_file(const std::wstring& path, const std::string& entry_point, const std::string& shader_model, ID3DBlobPtr& blob_out, const D3D_SHADER_MACRO* defines = nullptr)
  {
    HRESULT hr = S_OK;

    DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    shader_flags |= D3DCOMPILE_DEBUG;
#endif

    // compile the shader
    ID3DBlobPtr error_blob;
    //hr = D3DCompileFromFilepath.c_str(), defines, NULL, entry_point.c_str(), shader_model.c_str(), shader_flags, NULL, NULL, &blob_out, &error_blob, NULL);
    hr = D3DCompileFromFile(path.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point.c_str(), shader_model.c_str(),
      shader_flags, 0, &blob_out, &error_blob);
#if defined( DEBUG ) || defined( _DEBUG )
    if (error_blob != NULL)
      OutputDebugStringA((char*) error_blob->GetBufferPointer());
#endif
    if (FAILED(hr))
    {
      if (error_blob != NULL)
        OutputDebugStringA((char*) error_blob->GetBufferPointer());
      throw sf::win32_error_exception(hr);
    }
  }

  template <typename DevicePtr>
  inline void create_shader(DevicePtr& device, const void * blob_ptr, SIZE_T s, ID3D11ClassLinkage * l, ID3D11VertexShaderPtr& shader_ptr)
  {
    CHK(device->CreateVertexShader(blob_ptr, s, l, &shader_ptr));
  };

  template <typename DevicePtr>
  inline void create_shader(DevicePtr& device, const void * blob_ptr, SIZE_T s, ID3D11ClassLinkage * l, ID3D11GeometryShaderPtr& shader_ptr)
  {
    CHK(device->CreateGeometryShader(blob_ptr, s, l, &shader_ptr));
  };

  template <typename DevicePtr>
  inline void create_shader(DevicePtr& device, const void * blob_ptr, SIZE_T s, ID3D11ClassLinkage * l, ID3D11PixelShaderPtr& shader_ptr)
  {
    CHK(device->CreatePixelShader(blob_ptr, s, l, &shader_ptr));
  };

  template <typename DevicePtr>
  void create_shader(DevicePtr& device, const void * blob_ptr, SIZE_T s, ID3D11ClassLinkage * l, ID3D11ComputeShaderPtr& shader_ptr)
  {
    CHK(device->CreateComputeShader(blob_ptr, s, l, &shader_ptr));
  };

  template<typename Device, typename ShaderPtr>
  inline void create_shader_from_file(Device& d3d_device, const std::wstring& path, const std::string& entry_point, const std::string& shader_model, ShaderPtr& shader, const D3D_SHADER_MACRO* defines = nullptr)
  {
    ID3DBlobPtr blob;
    create_shader_blob_from_file(path, entry_point, shader_model, blob, defines);
    create_shader(d3d_device, blob->GetBufferPointer(), blob->GetBufferSize(), NULL, shader);
  }


  inline double power(const double re, const double im)
  {
    return sqrt(re*re + im*im);
  }

  inline double han_window(const int i, const int size)
  {
    return 0.5 - 0.5 * cos(2.0 * M_PI * i / size);
  }

  inline D2D1_COLOR_F hsv2rgb(double h, double s, double v, double alpha = 1.0)
  {
    if (h < 0)
    {
      h += 360;
    }
    else if (h >= 360)
    {
      h -= 360;
    }

    int H_i = (int) floor(h / 60);
    double fl = (h / 60) - H_i;
    if (!(H_i & 1)) fl = 1 - fl; // if i is even
    double m = v * (1 - s);
    double n = v * (1 - s * fl);

    double r = 0, g = 0, b = 0;
    switch (H_i)
    {
    case 0: r = v; g = n; b = m; break;
    case 1: r = n; g = v; b = m; break;
    case 2: r = m; g = v; b = n; break;
    case 3: r = m; g = n; b = v; break;
    case 4: r = n; g = m; b = v; break;
    case 5: r = v; g = m; b = n; break;
    }

    return D2D1::ColorF(r, g, b, alpha);
  }

  template <typename D3DContextPtr = ID3D11DeviceContext2Ptr,typename TexturePtr = ID3D11Texture2DPtr>
  struct map {
	  map(D3DContextPtr& context, TexturePtr & texture, uint32_t subresource_index, D3D11_MAP type, uint32_t flags)
		  : context_(context), texture_(texture)
	  {
		  CHK(context->Map(texture.Get(), subresource_index, type, flags, &map_));
	  
	  }

	  ~map()
	  {
		  context_->Unmap(texture_.Get(), 0);
	  }

	  void * data(){ return map_.pData; }
	  uint32_t row_pitch() { return map_.RowPitch; }
	  uint32_t depth_pitch() { return map_.DepthPitch; }
  private:
	  D3DContextPtr& context_;
	  TexturePtr& texture_;
	  D3D11_MAPPED_SUBRESOURCE map_;
  };

  template <typename MediaBufferPtr = IMFMediaBufferPtr>
  struct media_buffer_lock
  {
	  media_buffer_lock(MediaBufferPtr& buffer, DWORD curent_length = 0, DWORD max_length = 0)
		  : buffer_(buffer)
	  {
		  CHK(buffer->Lock(&byte_buffer_,&curent_length,&max_length));
	  }

	  ~media_buffer_lock()
	  {
		  buffer_->Unlock();
	  }

	  uint8_t * buffer(){ return byte_buffer_; }
  private:
	  uint8_t *byte_buffer_;
	  MediaBufferPtr& buffer_;
  };
}

