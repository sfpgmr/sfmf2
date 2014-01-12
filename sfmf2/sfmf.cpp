#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <wrl\client.h>
#include <ppltasks.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>  
//#include "DirectXBase.h"
#include "sfmf.h"

#pragma comment(lib, "shlwapi")
#pragma comment(lib,"Mfplat.lib")
#pragma comment(lib,"Mf.lib")
#pragma comment(lib,"Mfuuid.lib")
#pragma comment(lib,"Strmiids.lib")
#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")


// Format constants
const UINT32 VIDEO_FPS = 30;
const UINT32 VIDEO_BIT_RATE = 800000;
const GUID   VIDEO_ENCODING_FORMAT = MFVideoFormat_H264;
const GUID   VIDEO_INPUT_FORMAT = MFVideoFormat_RGB32;
const UINT32 VIDEO_PELS = VIDEO_WIDTH * VIDEO_HEIGHT;
const UINT32 VIDEO_FRAME_COUNT = 20 * VIDEO_FPS;



//template <class T> void SafeRelease(T **ppT)
//{
//  if (*ppT)
//  {
//    (*ppT)->Release();
//    *ppT = NULL;
//  }
//};
//
//// Buffer to hold the video frame data.
//DWORD videoFrameBuffer[VIDEO_PELS];
//
//HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex)
//{
//  *ppWriter = NULL;
//  *pStreamIndex = NULL;
//
//  IMFSinkWriter   *pSinkWriter = NULL;
//  IMFMediaType    *pMediaTypeOut = NULL;
//  IMFMediaType    *pMediaTypeIn = NULL;
//  DWORD           streamIndex;
//
//  HRESULT hr = MFCreateSinkWriterFromURL(L"output.m4v", NULL, NULL, &pSinkWriter);
//
//  // Set the output media type.
//  if (SUCCEEDED(hr))
//  {
//    hr = MFCreateMediaType(&pMediaTypeOut);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = pMediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, VIDEO_ENCODING_FORMAT);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = pMediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, VIDEO_BIT_RATE);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = pMediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = MFSetAttributeSize(pMediaTypeOut, MF_MT_FRAME_SIZE, VIDEO_WIDTH, VIDEO_HEIGHT);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_FRAME_RATE, VIDEO_FPS, 1);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = pSinkWriter->AddStream(pMediaTypeOut, &streamIndex);
//  }
//
//  // Set the input media type.
//  if (SUCCEEDED(hr))
//  {
//    hr = MFCreateMediaType(&pMediaTypeIn);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = pMediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = pMediaTypeIn->SetGUID(MF_MT_SUBTYPE, VIDEO_INPUT_FORMAT);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = pMediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = MFSetAttributeSize(pMediaTypeIn, MF_MT_FRAME_SIZE, VIDEO_WIDTH, VIDEO_HEIGHT);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_FRAME_RATE, VIDEO_FPS, 1);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = pSinkWriter->SetInputMediaType(streamIndex, pMediaTypeIn, NULL);
//  }
//
//  // Tell the sink writer to start accepting data.
//  if (SUCCEEDED(hr))
//  {
//    hr = pSinkWriter->BeginWriting();
//  }
//
//  // Return the pointer to the caller.
//  if (SUCCEEDED(hr))
//  {
//    *ppWriter = pSinkWriter;
//    (*ppWriter)->AddRef();
//    *pStreamIndex = streamIndex;
//  }
//
//  SafeRelease(&pSinkWriter);
//  SafeRelease(&pMediaTypeOut);
//  SafeRelease(&pMediaTypeIn);
//  return hr;
//}
//
//HRESULT WriteFrame(
//  IMFSinkWriter *pWriter,
//  DWORD streamIndex,
//  const LONGLONG& rtStart,        // Time stamp.
//  const LONGLONG& rtDuration      // Frame duration.
//  )
//{
//  IMFSample *pSample = NULL;
//  IMFMediaBuffer *pBuffer = NULL;
//
//  const LONG cbWidth = 4 * VIDEO_WIDTH;
//  const DWORD cbBuffer = cbWidth * VIDEO_HEIGHT;
//
//  BYTE *pData = NULL;
//
//  // Create a new memory buffer.
//  HRESULT hr = MFCreateMemoryBuffer(cbBuffer, &pBuffer);
//
//  // Lock the buffer and copy the video frame to the buffer.
//  if (SUCCEEDED(hr))
//  {
//    hr = pBuffer->Lock(&pData, NULL, NULL);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = MFCopyImage(
//      pData,                      // Destination buffer.
//      cbWidth,                    // Destination stride.
//      (BYTE*) videoFrameBuffer,    // First row in source image.
//      cbWidth,                    // Source stride.
//      cbWidth,                    // Image width in bytes.
//      VIDEO_HEIGHT                // Image height in pixels.
//      );
//  }
//  if (pBuffer)
//  {
//    pBuffer->Unlock();
//  }
//
//  // Set the data length of the buffer.
//  if (SUCCEEDED(hr))
//  {
//    hr = pBuffer->SetCurrentLength(cbBuffer);
//  }
//
//  // Create a media sample and add the buffer to the sample.
//  if (SUCCEEDED(hr))
//  {
//    hr = MFCreateSample(&pSample);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = pSample->AddBuffer(pBuffer);
//  }
//
//  // Set the time stamp and the duration.
//  if (SUCCEEDED(hr))
//  {
//    hr = pSample->SetSampleTime(rtStart);
//  }
//  if (SUCCEEDED(hr))
//  {
//    hr = pSample->SetSampleDuration(rtDuration);
//  }
//
//  // Send the sample to the Sink Writer.
//  if (SUCCEEDED(hr))
//  {
//    hr = pWriter->WriteSample(streamIndex, pSample);
//  }
//
//  SafeRelease(&pSample);
//  SafeRelease(&pBuffer);
//  return hr;
//}
//
//void testout()
//{
//  // Set all pixels to green
//  for (DWORD i = 0; i < VIDEO_PELS; ++i)
//  {
//    videoFrameBuffer[i] = 0x0000FF00;
//  }
//
//  //HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
//  //if (SUCCEEDED(hr))
//  {
//    HRESULT  hr = MFStartup(MF_VERSION);
//    if (SUCCEEDED(hr))
//    {
//      IMFSinkWriter *pSinkWriter = NULL;
//      DWORD stream;
//
//      hr = InitializeSinkWriter(&pSinkWriter, &stream);
//      if (SUCCEEDED(hr))
//      {
//        // Send frames to the sink writer.
//        LONGLONG rtStart = 0;
//        UINT64 rtDuration = 333333;
//
//        //MFFrameRateToAverageTimePerFrame(VIDEO_FPS, 1, &rtDuration);
//
//        for (DWORD i = 0; i < VIDEO_FRAME_COUNT; ++i)
//        {
//          hr = WriteFrame(pSinkWriter, stream, rtStart, rtDuration);
//          if (FAILED(hr))
//          {
//            break;
//          }
//          rtStart += rtDuration;
//        }
//      }
//      if (SUCCEEDED(hr))
//      {
//        hr = pSinkWriter->Finalize();
//      }
//      SafeRelease(&pSinkWriter);
//      MFShutdown();
//    }
//    //  CoUninitialize();
//  }
//}
//----------------------------------------------------------------------

//#include "MediaWriter.h"

using namespace Microsoft::WRL;
using namespace concurrency;
//using namespace Extensions;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;


namespace sf {



  //Windows::Foundation::IAsyncActionWithProgress<double>^ sf::WriteAsync(Windows::Storage::Streams::IRandomAccessStream^ stream)
  //{
  //  return create_async([stream]
  //    (progress_reporter<double> reporter, cancellation_token token) {

  //    // some parameters   

  //    //auto_mf mf;

  //    //
  //    // Sink Writer の作成
  //    //

  //    ComPtr<IMFByteStream> spByteStream;
  //    CHK(MFCreateMFByteStreamOnStreamEx((IUnknown*) stream, &spByteStream));

  //    ComPtr<IMFAttributes> spAttr;
  //    CHK(MFCreateAttributes(&spAttr, 10));
  //    CHK(spAttr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, true));

  //    ComPtr<IMFSinkWriter> spSinkWriter;
  //    CHK(MFCreateSinkWriterFromURL(L".mp4", spByteStream.Get(), spAttr.Get(), &spSinkWriter));

  //    //   
  //    // 出力メディアタイプのセットアップ   
  //    //   

  //    ComPtr<IMFMediaType> spTypeOut;
  //    CHK(MFCreateMediaType(&spTypeOut));
  //    CHK(spTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
  //    CHK(spTypeOut->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264));
  //    CHK(spTypeOut->SetUINT32(MF_MT_AVG_BITRATE, BITRATE));
  //    CHK(spTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive));
  //    CHK(MFSetAttributeSize(spTypeOut.Get(), MF_MT_FRAME_SIZE, WIDTH, HEIGHT));
  //    CHK(MFSetAttributeRatio(spTypeOut.Get(), MF_MT_FRAME_RATE, RATE_NUM, RATE_DENOM));
  //    CHK(MFSetAttributeRatio(spTypeOut.Get(), MF_MT_PIXEL_ASPECT_RATIO, ASPECT_NUM, ASPECT_DENOM));

  //    DWORD streamIndex;
  //    CHK(spSinkWriter->AddStream(spTypeOut.Get(), &streamIndex));

  //    //   
  //    // 入力メディアタイプのセットアップ  
  //    //   

  //    ComPtr<IMFMediaType> spTypeIn;
  //    CHK(MFCreateMediaType(&spTypeIn));
  //    CHK(spTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
  //    CHK(spTypeIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32));
  //    CHK(spTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive));
  //    CHK(MFSetAttributeSize(spTypeIn.Get(), MF_MT_FRAME_SIZE, WIDTH, HEIGHT));
  //    CHK(MFSetAttributeRatio(spTypeIn.Get(), MF_MT_FRAME_RATE, RATE_NUM, RATE_DENOM));
  //    CHK(MFSetAttributeRatio(spTypeIn.Get(), MF_MT_PIXEL_ASPECT_RATIO, ASPECT_NUM, ASPECT_DENOM));

  //    CHK(spSinkWriter->SetInputMediaType(streamIndex, spTypeIn.Get(), nullptr));

  //    //   
  //    //    
  //    //   

  //    CHK(spSinkWriter->BeginWriting());

  //    double progress = 0.;
  //    LONGLONG hnsSampleTime = 0;
  //    for (unsigned int nFrame = 0; nFrame < FRAME_NUM; nFrame++)
  //    {
  //      if (token.is_canceled())
  //      {
  //        break;
  //      }

  //      double newProgress = 100. * (double) nFrame / (double) FRAME_NUM;
  //      if (newProgress - progress >= 1.)
  //      {
  //        progress = newProgress;
  //        reporter.report(progress);
  //      }

  //      //   
  //      // Create a media sample   
  //      //   

  //      ComPtr<IMFSample> spSample;
  //      CHK(MFCreateSample(&spSample));
  //      CHK(spSample->SetSampleDuration(hnsSampleDuration));
  //      CHK(spSample->SetSampleTime(hnsSampleTime));
  //      hnsSampleTime += hnsSampleDuration;

  //      //   
  //      // Add a media buffer filled with random data   
  //      //   

  //      ComPtr<IMFMediaBuffer> spBuffer;
  //      CHK(MFCreateMemoryBuffer(cbMaxLength, &spBuffer));
  //      CHK(spBuffer->SetCurrentLength(cbMaxLength));
  //      CHK(spSample->AddBuffer(spBuffer.Get()));

  //      // Draw a bouncing white rectangle over black background
  //      unsigned char *pbBuffer = nullptr;
  //      CHK(spBuffer->Lock(&pbBuffer, nullptr, nullptr));
  //      for (unsigned int i = 0; i < HEIGHT; i++)
  //      {
  //        for (unsigned int j = 0; j < WIDTH; j++)
  //        {
  //          unsigned int pos = 4 * (i * WIDTH + j);
  //          unsigned char val = 255 * (
  //            (abs((int) WIDTH / 2 - (int) j) < (WIDTH / 4)) &&
  //            (abs(HEIGHT * (.5 + .1 * sin(2. * M_PI * (double) nFrame / (double) ONE_SECOND)) - (int) i) < (HEIGHT / 4))
  //            );
  //          pbBuffer[pos] = val;
  //          pbBuffer[pos + 1] = val;
  //          pbBuffer[pos + 2] = val;
  //          pbBuffer[pos + 3] = val;
  //        }
  //      }
  //      CHK(spBuffer->Unlock());

  //      //   
  //      // Write the media sample   
  //      //   

  //      CHK(spSinkWriter->WriteSample(streamIndex, spSample.Get()));
  //    }

  //    if (!token.is_canceled())
  //    {
  //      CHK(spSinkWriter->Finalize());

  //      reporter.report(100.);
  //    }
  //  });
  //}


  //---------------------------------------------------------------------
const unsigned int RATE_NUM = 30000;
const unsigned int RATE_DENOM = 1000;
const long long hnsSampleDuration = 10000000 * (long long) RATE_DENOM / (long long) RATE_NUM;

video_writer::video_writer(
    std::wstring& target_path,IMFMediaTypePtr& audio_media_type
    , unsigned int width, unsigned int height) : target_path_(target_path), audio_media_type_(audio_media_type), width_(width), height_(height)
  {
    const unsigned int WIDTH = width_;
    const unsigned int HEIGHT = height_;
    const unsigned int BITRATE = 3000000;
    const unsigned int ASPECT_NUM = 1;
    const unsigned int ASPECT_DENOM = 1;
    const unsigned long  BPP_IN = 32;
    const unsigned long cbMaxLength = WIDTH * HEIGHT * BPP_IN / 8;
    const unsigned int ONE_SECOND = RATE_NUM / RATE_DENOM;
    const unsigned int FRAME_NUM = 10 * ONE_SECOND;

    samples_per_second = 44100;
    average_bytes_per_second = 24000;
    channel_count = 2;
    bits_per_sample = 16;

    // 入力ストリームから SinkWriterを生成する

    CHK(MFCreateFile(MF_FILE_ACCESSMODE::MF_ACCESSMODE_WRITE,MF_FILE_OPENMODE::MF_OPENMODE_DELETE_IF_EXIST,MF_FILE_FLAGS::MF_FILEFLAGS_NONE,target_path.c_str(),&byte_stream_));

    CHK(MFCreateAttributes(&attr_, 10));
    CHK(attr_->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, true));
    CHK(attr_->SetUINT32(MF_SINK_WRITER_DISABLE_THROTTLING, true));


    

    IMFSinkWriterPtr sinkWriter;

    CHK(MFCreateSinkWriterFromURL(L".mp4", byte_stream_.Get(), attr_.Get(), &sinkWriter));
    CHK(sinkWriter.As(&sink_writer_));

    //   
    // 出力メディアタイプのセットアップ   
    //   

    // ビデオ

    CHK(MFCreateMediaType(&media_type_out_));
    CHK(media_type_out_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
    CHK(media_type_out_->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264));
    CHK(media_type_out_->SetUINT32(MF_MT_MPEG2_PROFILE, eAVEncH264VProfile_Main));
    //CHK(media_type_out_->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32));   
    CHK(media_type_out_->SetUINT32(MF_MT_AVG_BITRATE, BITRATE));
    CHK(media_type_out_->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive));
    CHK(MFSetAttributeSize(media_type_out_.Get(), MF_MT_FRAME_SIZE, WIDTH, HEIGHT));
    CHK(MFSetAttributeRatio(media_type_out_.Get(), MF_MT_FRAME_RATE, RATE_NUM, RATE_DENOM));
    CHK(MFSetAttributeRatio(media_type_out_.Get(), MF_MT_PIXEL_ASPECT_RATIO, ASPECT_NUM, ASPECT_DENOM));

    CHK(sink_writer_->AddStream(media_type_out_.Get(), &stream_index_));
    IMFTransformPtr mft;
    //IMFRateSupportPtr ptr;

    //CHK(sink_writer_->GetServiceForStream(stream_index_, MF_RATE_CONTROL_SERVICE, __uuidof(IMFRateSupport), &ptr));

    // オーディオ

    CHK(MFCreateMediaType(&media_type_out_audio_));
    CHK(media_type_out_audio_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio));
    CHK(media_type_out_audio_->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC));
    CHK(media_type_out_audio_->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, samples_per_second));
    CHK(media_type_out_audio_->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, bits_per_sample));
    CHK(media_type_out_audio_->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channel_count));
    CHK(media_type_out_audio_->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, average_bytes_per_second));
    CHK(media_type_out_audio_->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, 1));
    CHK(sink_writer_->AddStream(media_type_out_audio_.Get(), &stream_index_audio_));

    //   
    // 入力メディアタイプのセットアップ  
    //   

    // ビデオ

    CHK(MFCreateMediaType(&media_type_in_));
    CHK(media_type_in_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
    CHK(media_type_in_->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32));
    CHK(media_type_in_->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive));
    CHK(MFSetAttributeSize(media_type_in_.Get(), MF_MT_FRAME_SIZE, WIDTH, HEIGHT));
    CHK(MFSetAttributeRatio(media_type_in_.Get(), MF_MT_FRAME_RATE, RATE_NUM, RATE_DENOM));
    CHK(MFSetAttributeRatio(media_type_in_.Get(), MF_MT_PIXEL_ASPECT_RATIO, ASPECT_NUM, ASPECT_DENOM));

    // エンコーダーのセットアップ
    prop_variant prop;
    IPropertyStorePtr pPropertyStore;
    IMFAttributesPtr pEncoderParameters;

    CHK(PSCreateMemoryPropertyStore(__uuidof(IPropertyStore), (void**) &pPropertyStore));

    prop.value().vt = VT_BOOL;
    prop.value().boolVal = VARIANT_TRUE;
    CHK(pPropertyStore->SetValue(MFPKEY_VBRENABLED, prop.value()));
    prop.value().vt = VT_I4;
    prop.value().lVal = 100;
    CHK(pPropertyStore->SetValue(MFPKEY_VBRQUALITY, prop.value()));

    CHK(MFCreateAttributes(&pEncoderParameters, 5));
    CHK(attr_->SetUnknown(MF_SINK_WRITER_ENCODER_CONFIG, pPropertyStore.Get()));

    CHK(sink_writer_->SetInputMediaType(stream_index_, media_type_in_.Get(), pEncoderParameters.Get()));

    // オーディオ

    CHK(MFCreateMediaType(&media_type_in_audio_));
    //CHK(media_type_in_audio_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio));
    //CHK(media_type_in_audio_->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM));
    //CHK(media_type_in_audio_->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, bits_per_sample));
    //CHK(media_type_in_audio_->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, samples_per_second));
    //CHK(media_type_in_audio_->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channel_count));
    audio_media_type_->CopyAllItems(media_type_in_audio_.Get());
    CHK(sink_writer_->SetInputMediaType(stream_index_audio_, media_type_in_audio_.Get(), NULL));

    //   
    // 出力開始  
    //   

    CHK(sink_writer_->BeginWriting());

    //   
    // メディア・サンプルの作成   
    //   

    CHK(MFCreateSample(&sample_));
    video_sample_time_ = 0;
    CHK(sample_->SetSampleDuration(hnsSampleDuration));

    //   
    // メディア・バッファの生成と、メディア・サンプルへの追加    
    //   

    CHK(MFCreateMemoryBuffer(cbMaxLength, &buffer_));
    CHK(buffer_->SetCurrentLength(cbMaxLength));
    CHK(sample_->AddBuffer(buffer_.Get()));

  }

  // テクスチャをメディアバッファに書き込む
  void video_writer::set_texture_to_sample(ID3D11DeviceContext1* context, ID3D11Texture2D* texture)
  {

    // タイムスタンプの設定
    CHK(sample_->SetSampleTime(video_sample_time_));

    // 書き込み先バッファのロック
    unsigned char *pbBuffer = nullptr;
    CHK(buffer_->Lock(&pbBuffer, nullptr, nullptr));

    // 読み込みテクスチャをマップ
    D3D11_MAPPED_SUBRESOURCE mapped;
    CHK(context->Map(texture, 0, D3D11_MAP_READ, 0, &mapped));

    CHK(MFCopyImage(pbBuffer, width_ * 4, reinterpret_cast<BYTE*>(mapped.pData), mapped.RowPitch, width_ * 4, height_));

    // 書き込み先バッファのアンロック
    CHK(buffer_->Unlock());
    // テクスチャをアンマップ
    context->Unmap(texture, 0);
    video_sample_time_ += hnsSampleDuration;
  }

  void video_writer::write_video_sample()
  {

    CHK(sink_writer_->WriteSample(stream_index_, sample_.Get()));
  }

  void video_writer::write_audio_sample(IMFSample* sample)
  {
    CHK(sink_writer_->WriteSample(stream_index_audio_, sample));
    CHK(sample->GetSampleTime(&audio_sample_time_));
    DOUT(boost::wformat(L"%10x \n") % audio_sample_time_);
  }

  //***********************************************
  // Audio Reader
  //***********************************************

  audio_reader::audio_reader(std::wstring& source)
  {
    {
      IMFByteStreamPtr stream;
      CHK(MFCreateFile(MF_FILE_ACCESSMODE::MF_ACCESSMODE_READ, MF_FILE_OPENMODE::MF_OPENMODE_FAIL_IF_NOT_EXIST, MF_FILE_FLAGS::MF_FILEFLAGS_NONE, source.c_str(), &stream));

      IMFAttributesPtr attr;
      CHK(MFCreateAttributes(&attr, 10));
      CHK(attr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, true));
      IMFSourceReaderPtr reader;
      CHK(MFCreateSourceReaderFromByteStream(stream.Get(), attr.Get(), &reader));

//      CHK(MFCreateSourceReaderFromURL(source.c_str(),attr.Get(), &reader));
      CHK(reader.As(&reader_));
      QWORD length;
      CHK(stream->GetLength(&length));
      fileSize_ = length;
    }

    //CHK(reader_->GetServiceForStream(0, MF_WRAPPED_OBJECT, __uuidof(IMFByteStream), &stream));

    CHK(reader_->GetNativeMediaType(0, 0, &native_media_type_));
    CHK(MFCreateMediaType(&current_media_type_));
    CHK(current_media_type_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio));
    CHK(current_media_type_->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM));
    CHK(current_media_type_->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 
      MFGetAttributeUINT32(native_media_type_.Get(),MF_MT_AUDIO_BITS_PER_SAMPLE, bits_per_sample)
      )
    );
    CHK(current_media_type_->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND,
      MFGetAttributeUINT32(native_media_type_.Get(), MF_MT_AUDIO_SAMPLES_PER_SECOND, samples_per_second)
      ));
    CHK(current_media_type_->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS,
      MFGetAttributeUINT32(native_media_type_.Get(), MF_MT_AUDIO_NUM_CHANNELS,channel_count)
      ));
    //DWORD blockAlign;
    //CHK(native_media_type_->GetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, &blockAlign));
    //CHK(current_media_type_->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT,blockAlign ));
    CHK(reader_->SetCurrentMediaType(0, nullptr, current_media_type_.Get()));
    CHK(reader_->GetCurrentMediaType(0, current_media_type_.ReleaseAndGetAddressOf()));
    UINT32 blockAlign;
    CHK(current_media_type_->GetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, &blockAlign));

    DOUT(boost::wformat(L"Block Align: %10d %10x") % blockAlign % blockAlign);

  }

  DWORD audio_reader::read_sample(IMFSamplePtr& sample)
  {
    DWORD streamIndex, flags;
    CHK(reader_->ReadSample(0, 0, &streamIndex, &flags, &video_sample_time_, sample.ReleaseAndGetAddressOf()));
    return flags;
  }
}




