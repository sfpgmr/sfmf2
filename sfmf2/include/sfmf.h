#pragma once
// Media Foundation headers
//#include <d3d9.h>

namespace sf {


#define VIDEO_WIDTH  1280
#define VIDEO_HEIGHT  720

	//extern HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex);
	//extern HRESULT WriteFrame(
	//	IMFSinkWriter *pWriter,
	//	DWORD streamIndex,
	//	const LONGLONG& rtStart,        // Time stamp.
	//	const LONGLONG& rtDuration      // Frame duration.
	//	);
	//extern Windows::Foundation::IAsyncActionWithProgress<double>^ WriteAsync(Windows::Storage::Streams::IRandomAccessStream^ stream);

	// Class to start and shutdown Media Foundation
	class auto_mf
	{
	public:
		auto_mf()
			: _bInitialized(false)
		{
			CHK(MFStartup(MF_VERSION));
			_bInitialized = true;
		}

		~auto_mf()
		{
			if (_bInitialized)
			{
				(void) MFShutdown();
			}
		}

	private:
		bool _bInitialized;
	};

	class  video_writer
	{
	public:
		video_writer(std::wstring& target_path, IMFMediaTypePtr& audio_media_type, ID3D11DeviceContext2Ptr& context,ID3D11Texture2DPtr& texture/*, unsigned int width = VIDEO_WIDTH, unsigned int height = VIDEO_HEIGHT*/);

		virtual ~video_writer()
		{
			finalize();
			texture_.Reset();
			context_.Reset();
		};

		virtual void finalize()
		{
			if (sink_writer_)
			{
				sink_writer_->Finalize();
			}
		}

		void write_audio_sample(IMFSample* sample);
		void write_video_sample();
		void set_texture_to_sample();

		UINT samples_per_second;
		UINT average_bytes_per_second;
		UINT channel_count;
		UINT bits_per_sample;
		LONGLONG video_sample_time(){ return video_sample_time_; }

	private:
		struct copy_image : Xbyak::CodeGenerator {
			copy_image(uint32_t width, uint32_t height, uint32_t pitch);
			void mov16();
			void mov32();
			void mov64();
		};
		std::unique_ptr<copy_image> copy_image_;
		typedef void(*copy_func_t) (void *, void *);
		copy_func_t copy_func_;

		std::wstring target_path_;
		IMFSinkWriterExPtr sink_writer_;
		IMFByteStreamPtr byte_stream_;
		IMFAttributesPtr attr_;
		IMFMediaTypePtr media_type_out_;
		IMFMediaTypePtr media_type_in_;
		IMFMediaTypePtr audio_media_type_;
		IMFMediaTypePtr media_type_in_audio_;
		IMFMediaTypePtr media_type_out_audio_;
		DWORD stream_index_;
		DWORD stream_index_audio_;

		IMFSamplePtr sample_;
		IMFMediaBufferPtr buffer_;
		LONGLONG video_sample_time_;
		LONGLONG audio_sample_time_;

		ID3D11DeviceContext2Ptr context_;
		ID3D11Texture2DPtr texture_;


		unsigned int width_, height_;
	};


	class audio_reader_callback : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IMFSourceReaderCallback >
	{
	public:
		typedef boost::signals2::signal<void(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample *pSample)> OnReadSampleSignal;

		audio_reader_callback()
		{
		}

		~audio_reader_callback()
		{
		}

		// IMFSourceReaderCallback methods
		STDMETHODIMP OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex,
			DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample *pSample)
		{
			onReadSampleSignal_(hrStatus, dwStreamIndex,
				dwStreamFlags, llTimestamp, pSample);
			return S_OK;

		}

		STDMETHODIMP OnEvent(DWORD, IMFMediaEvent *)
		{
			return S_OK;
		}

		STDMETHODIMP OnFlush(DWORD)
		{
			return S_OK;
		}

		OnReadSampleSignal& SignalOnReadSample(){ return onReadSampleSignal_; }
	private:
		OnReadSampleSignal onReadSampleSignal_;
	};

	typedef Microsoft::WRL::ComPtr<audio_reader_callback> AudioReaderCallBackPtr;

	class audio_reader {
	public:
		audio_reader(std::wstring& source);

		const UINT samples_per_second = 44100;
		const UINT average_bytes_per_second = 24000;
		const UINT channel_count = 2;
		const UINT bits_per_sample = 16;

		LONGLONG size() { return fileSize_; }
		DWORD read_sample(IMFSamplePtr& sample);
		LONGLONG sample_time() { return video_sample_time_; }
		IMFMediaTypePtr& native_media_type(){ return native_media_type_; }
		IMFMediaTypePtr& current_media_type(){ return current_media_type_; }

	private:
		IMFByteStreamPtr reader_stream_;
		IMFSourceReaderExPtr reader_;
		IMFByteStreamPtr byte_stream_;
		IMFMediaTypePtr native_media_type_;
		IMFMediaTypePtr current_media_type_;
		LONGLONG video_sample_time_;
		LONGLONG fileSize_;

	};

}
