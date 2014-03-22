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
#include "sf_memory.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

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


	//---------------------------------------------------------------------
	const unsigned int RATE_NUM = 30000;
	const unsigned int RATE_DENOM = 1000;
	const long long hnsSampleDuration = 10000000 * (long long) RATE_DENOM / (long long) RATE_NUM;

	video_writer::copy_image::copy_image(uint32_t width, uint32_t height, uint32_t pitch)
	{
	
		uint32_t w_byte = width * 4 / 16;
		uint32_t index = 0;

		std::function<void()> funcs[3] =
		{
			[this] { mov16(); },
			[this] { mov32(); },
			[this] { mov64(); }
		};

		for (; index < 2; ++index){
			if ((w_byte & 1) == 1)
			{
				break;
			}
			else {
				w_byte = w_byte / 2;
			}
		}

		mov(r8, w_byte);
		mov(r9, height);
		mov(r10, pitch * 2);
		L("L2");
		mov(rax, r8);
		L("L1");
		funcs[index]();
		sub(rax, 1);
		jnz("L1");
		sub(rcx, r10);
		sub(r9, 1);
		jnz("L2");
		vzeroupper();
		ret();

	}

	void video_writer::copy_image::mov16()
	{
		vmovdqa(xmm0, ptr[rcx]);
		vmovdqa(ptr[rdx], xmm0);
		add(rcx, 16);
		add(rdx, 16);
	}

	void video_writer::copy_image::mov32()
	{
		vmovdqa(xmm0, ptr[rcx]);
		vmovdqa(xmm1, ptr[rcx + 16]);
		vmovdqa(ptr[rdx], xmm0);
		vmovdqa(ptr[rdx + 16], xmm1);

		add(rcx, 32);
		add(rdx, 32);
	}

	void video_writer::copy_image::mov64()
	{
		vmovdqa(xmm0, ptr[rcx]);
		vmovdqa(xmm1, ptr[rcx + 16]);
		vmovdqa(ptr[rdx], xmm0);
		vmovdqa(xmm2, ptr[rcx + 32]);
		vmovdqa(ptr[rdx + 16], xmm1);
		vmovdqa(xmm3, ptr[rcx + 48]);
		vmovdqa(ptr[rdx + 32], xmm2);
		add(rcx, 64);
		vmovdqa(ptr[rdx + 48], xmm3);
		add(rdx, 64);

	}

	video_writer::video_writer(
		std::wstring& target_path, IMFMediaTypePtr& audio_media_type, ID3D11DeviceContext2Ptr& context, ID3D11Texture2DPtr& texture
		/*, unsigned int width, unsigned int height*/) : target_path_(target_path), audio_media_type_(audio_media_type), context_(context), texture_(texture)
	{
		D3D11_TEXTURE2D_DESC desc = {};
		texture->GetDesc(&desc);
		width_ = desc.Width;
		height_ = desc.Height;

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

		CHK(MFCreateFile(MF_FILE_ACCESSMODE::MF_ACCESSMODE_WRITE, MF_FILE_OPENMODE::MF_OPENMODE_DELETE_IF_EXIST, MF_FILE_FLAGS::MF_FILEFLAGS_NONE, target_path.c_str(), &byte_stream_));

		CHK(MFCreateAttributes(&attr_, 10));
		CHK(attr_->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, true));
		CHK(attr_->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, false));
		CHK(attr_->SetUINT32(MF_SINK_WRITER_DISABLE_THROTTLING, true));




		IMFSinkWriterPtr sinkWriter;

		CHK(MFCreateSinkWriterFromURL(L".mp4", byte_stream_.Get(), attr_.Get(), &sinkWriter));
		CHK(sinkWriter.As(&sink_writer_));
		//CHK(MFCreateSinkWriterFromURL(L".mp4", byte_stream_.Get(), attr_.Get(), &sink_writer_));



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
		//prop_variant prop;
		//IPropertyStorePtr pPropertyStore;
		//IMFAttributesPtr pEncoderParameters;

		//CHK(PSCreateMemoryPropertyStore(__uuidof(IPropertyStore), (void**) &pPropertyStore));

		//prop.value().vt = VT_BOOL;
		//prop.value().boolVal = VARIANT_FALSE;
		//CHK(pPropertyStore->SetValue(MFPKEY_VBRENABLED, prop.value()));
		//prop.value().vt = VT_I4;
		//prop.value().lVal = 100;
		//CHK(pPropertyStore->SetValue(MFPKEY_VBRQUALITY, prop.value()));

		//CHK(MFCreateAttributes(&pEncoderParameters, 5));
		//CHK(attr_->SetUnknown(MF_SINK_WRITER_ENCODER_CONFIG, pPropertyStore.Get()));

		CHK(sink_writer_->SetInputMediaType(stream_index_, media_type_in_.Get(), nullptr /*pEncoderParameters.Get()*/));

		// オーディオ

		CHK(MFCreateMediaType(&media_type_in_audio_));
		//CHK(media_type_in_audio_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio));
		//CHK(media_type_in_audio_->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM));
		//CHK(media_type_in_audio_->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, bits_per_sample));
		//CHK(media_type_in_audio_->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, samples_per_second));
		//CHK(media_type_in_audio_->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channel_count));
		audio_media_type_->CopyAllItems(media_type_in_audio_.Get());
		CHK(sink_writer_->SetInputMediaType(stream_index_audio_, media_type_in_audio_.Get(), NULL));

		// ハードウェアエンコーダが使われているかの確認

		{
			IMFTransformPtr transform;
			ICodecAPIPtr codec;
			GUID guid;

			CHK(sink_writer_->GetServiceForStream(stream_index_, GUID_NULL, IID_IMFTransform, &transform));

			IMFAttributesPtr attributes;
			CHK(transform->GetAttributes(&attributes));
			UINT32 l = 0;
			std::wstring str;
			bool use_hw = false;
			HRESULT hr = attributes->GetStringLength(MFT_ENUM_HARDWARE_URL_Attribute, &l);
			if (SUCCEEDED(hr))
			{
				str.reserve(l + 1);
				hr = attributes->GetString(MFT_ENUM_HARDWARE_URL_Attribute, (LPWSTR) str.data(), l + 1, &l);
				if (SUCCEEDED(hr)){
					use_hw = true;
					DOUT2(L"/////// HARDWARE ENCODE IS USED. ////\n");
				}
			}
		}

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

		CHK(MFCreateAlignedMemoryBuffer(cbMaxLength, MF_16_BYTE_ALIGNMENT, &buffer_));// 16 byteアラインメント
		CHK(buffer_->SetCurrentLength(cbMaxLength));
		CHK(sample_->AddBuffer(buffer_.Get()));

		//
		// 読み込みテクスチャをマップ
		sf::map<> map(context,texture, 0, D3D11_MAP_READ, 0);
		copy_image_.reset(new video_writer::copy_image(width_, height_, map.row_pitch()));
		copy_func_ = (copy_func_t)copy_image_->getCode();

	}

#define SF_AVX 
	// テクスチャをメディアバッファに書き込む
	void video_writer::set_texture_to_sample()
	{

		// タイムスタンプの設定
		CHK(sample_->SetSampleTime(video_sample_time_));

		// 書き込み先バッファのロック
		sf::media_buffer_lock<> buffer(buffer_);

		// 読み込みテクスチャをマップ
		sf::map<> map(context_, texture_, 0, D3D11_MAP_READ, 0);

		//CHK(MFCopyImage(pbBuffer, width_ * 4, reinterpret_cast<BYTE*>(mapped.pData), mapped.RowPitch, width_ * 4, height_));
#ifndef SF_AVX
		// Xbyak で書く前のコード
		DWORD *dest = (DWORD*) buffer.buffer();
		const UINT pitch = map.row_pitch() / sizeof(uint32_t);
		DWORD *src = (DWORD*) map.data() + (height_ - 1) * pitch;

		for (UINT i = 0; i < height_; ++i){
			for (UINT j = 0; j < width_; ++j){
				*dest++ = *src++;
			}
			src -= pitch * 2;
		}
#else
		// Xbyakで書き換えた後のコード
		void * src = (uint8_t*)map.data() + (height_ - 1) * map.row_pitch();
		(copy_func_)(src,buffer.buffer());

#endif
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
		//    DOUT(boost::wformat(L"%10x \n") % audio_sample_time_);
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
			MFGetAttributeUINT32(native_media_type_.Get(), MF_MT_AUDIO_BITS_PER_SAMPLE, bits_per_sample)
			)
			);
		CHK(current_media_type_->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND,
			MFGetAttributeUINT32(native_media_type_.Get(), MF_MT_AUDIO_SAMPLES_PER_SECOND, samples_per_second)
			));
		CHK(current_media_type_->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS,
			MFGetAttributeUINT32(native_media_type_.Get(), MF_MT_AUDIO_NUM_CHANNELS, channel_count)
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
		LONGLONG time;
		CHK(reader_->ReadSample(0, 0, &streamIndex, &flags, &time, sample.ReleaseAndGetAddressOf()));
		if (!(flags & MF_SOURCE_READERF_ENDOFSTREAM)){
			video_sample_time_ = time;
		}
		return flags;
	}

	void print_mft_(const GUID& guid, std::wfstream& out, uint32_t flags = MFT_ENUM_FLAG_ALL)
	{
		co_task_memory<IMFActivate*> activate;
		
		UINT32 count = 0;

		HRESULT hr = MFTEnumEx(guid, flags, NULL, NULL, &activate, &count);

		if (SUCCEEDED(hr) && count > 0)
		{
			for (int i = 0; i < count; ++i)
			{
				UINT32 l = 0;
				UINT32 l1 = 0;
				activate.get()[i]->GetStringLength(MFT_FRIENDLY_NAME_Attribute, &l);
				std::unique_ptr<wchar_t[]> name(new wchar_t[l + 1]);
				memset(name.get(), 0, l + 1);
				hr = activate.get()[i]->GetString(MFT_FRIENDLY_NAME_Attribute, name.get(), l + 1, &l1);
				out << name.get() << std::endl;
				activate.get()[i]->Release();
			}
			//CoTaskMemFree(activate);
		}
	}

	void print_mft()
	{
		std::wfstream out(L"MFT.txt", std::ios_base::out | std::ios_base::trunc);

		out << std::endl << "**" << L"MFT_CATEGORY_AUDIO_DECODER" << L"**" << std::endl << std::endl;

		print_mft_(MFT_CATEGORY_AUDIO_DECODER, out);

		out << std::endl << L"**" << L"MFT_CATEGORY_AUDIO_EFFECT" << L"**" << std::endl << std::endl;
		print_mft_(MFT_CATEGORY_AUDIO_EFFECT, out);

		out << std::endl << L"**" << L"MFT_CATEGORY_AUDIO_ENCODER" << L"**" << std::endl << std::endl;
		print_mft_(MFT_CATEGORY_AUDIO_ENCODER, out);

		out << std::endl << L"**" << L"MFT_CATEGORY_DEMULTIPLEXER" << L"**" << std::endl << std::endl;
		print_mft_(MFT_CATEGORY_DEMULTIPLEXER, out);

		out << std::endl << L"**" << L"MFT_CATEGORY_MULTIPLEXER" << L"**" << std::endl << std::endl;
		print_mft_(MFT_CATEGORY_MULTIPLEXER, out);

		out << std::endl << L"**" << L"MFT_CATEGORY_OTHER" << L"**" << std::endl << std::endl;
		print_mft_(MFT_CATEGORY_OTHER, out);

		out << std::endl << L"**" << L"MFT_CATEGORY_VIDEO_DECODER" << L"**" << std::endl << std::endl;
		print_mft_(MFT_CATEGORY_VIDEO_DECODER, out);

		out << std::endl << L"**" << L"MFT_CATEGORY_VIDEO_EFFECT" << L"**" << std::endl << std::endl;
		print_mft_(MFT_CATEGORY_VIDEO_EFFECT, out);

		out << std::endl << L"**" << L"MFT_CATEGORY_VIDEO_ENCODER" << L"**" << std::endl << std::endl;
		print_mft_(MFT_CATEGORY_VIDEO_ENCODER, out, MFT_ENUM_FLAG_HARDWARE);

		out << std::endl << L"**" << L"MFT_CATEGORY_VIDEO_PROCESSOR" << L"**" << std::endl << std::endl;
		print_mft_(MFT_CATEGORY_VIDEO_PROCESSOR, out, MFT_ENUM_FLAG_HARDWARE);

		out.close();
	}


}




