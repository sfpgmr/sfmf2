#include "stdafx.h"
#include "media_foundation.h"



template <class T> void SafeRelease(T **ppT)
{
  if (*ppT)
  {
    (*ppT)->Release();
    *ppT = NULL;
  }
}

namespace sf 
{
  namespace media {

    //コンストラクタ
    session_::session_() 
      : close_event_(CreateEvent(NULL, FALSE, FALSE, NULL))
    {
      create_session();
    }
  
    // デストラクタ
    session_::~session_()
    {
      close_session();
      media_session_.Reset();
    }
  
    //  メディアセッションを作成する。
    void session_::create_session()
    {
      // Close the old session, if any.
      close_session();

      // assert(m_state == Closed);

      // Create the media session.
      THROW_IF_ERR(MFCreateMediaSession(NULL, &media_session_));

      // Start pulling events from the media session
      THROW_IF_ERR(media_session_->BeginGetEvent((IMFAsyncCallback*)this, NULL));
      // m_state = Ready;
    }

    //  メディアセッションを閉じる 
    void session_::close_session()
    {

      //m_pVideoDisplay.Reset();

      // close the media session.
      if (media_session_)
      {
        DWORD result = 0;

        //m_state = Closing;

        THROW_IF_ERR(media_session_->Close());
        // Wait for the close operation to complete
        result = WaitForSingleObject(close_event_.get(), 5000);
        if (result == WAIT_TIMEOUT)
        {
          assert(FALSE);
        }
        // Now there will be no more events from this session.
      }

    }

    HRESULT session_::Invoke(IMFAsyncResult *pResult)
    {
      MediaEventType meType = MEUnknown;  // Event type

      IMFMediaEventPtr pEvent;

      try {
        // Get the event from the event queue.
        THROW_IF_ERR(media_session_->EndGetEvent(pResult, pEvent.GetAddressOf()));

        // Get the event type. 
        THROW_IF_ERR(pEvent->GetType(&meType));

        if (meType == MESessionClosed)
        {
          // The session was closed. 
          // The application is waiting on the m_hCloseEvent event handle. 
          SetEvent(close_event_.get());
        }
        else
        {
          // For all other events, get the next event in the queue.
          THROW_IF_ERR(media_session_->BeginGetEvent(this, NULL));
        }

        // Check the application state. 

        // If a call to IMFMediaSession::Close is pending, it means the 
        // application is waiting on the m_hCloseEvent event and
        // the application's message loop is blocked. 

        // Otherwise, post a private window message to the application. 

        //if (m_state != Closing)
        //{
        //  // Leave a reference count on the event.

        //  //PostMessage(m_hwndEvent, WM_APP_PLAYER_EVENT, 
        //  //  (WPARAM) pEvent.Detach(), (LPARAM)meType);
        //}

        HandleEvent((UINT_PTR)pEvent.Detach());

        return S_OK;
      } catch (win32_error_exception& e) 
      {
        return e.hresult();
      }
    }

    void session_::HandleEvent(UINT_PTR pEventPtr)
    {
      MediaEventType meType = MEUnknown;  

      IMFMediaEventPtr pEvent;
      //    pEvent.Attach((IMFMediaEvent*)pEventPtr);
      pEvent.Attach(reinterpret_cast<IMFMediaEvent*>(pEventPtr));

      if (!pEvent)
      {
        throw win32_error_exception(E_POINTER);
      }

      // Get the event type.
      THROW_IF_ERR(pEvent->GetType(&meType));

      // Get the event status. If the operation that triggered the event 
      // did not succeed, the status is a failure code.
      HRESULT hrStatus = S_OK;
      THROW_IF_ERR(pEvent->GetStatus(&hrStatus));
      // Check if the async operation succeeded.
      THROW_IF_ERR(hrStatus);

      switch(meType)
      {
      case MESessionTopologyStatus:
        //OnTopologyStatus(pEvent);
        break;

      case MEEndOfPresentation:
        //OnPresentationEnded(pEvent);
        break;

      case MENewPresentation:
        //OnNewPresentation(pEvent);
        break;

      default:
        //OnSessionEvent(pEvent, meType);
        break;
      }
    }


    // Format constants
    const UINT32 VIDEO_WIDTH = 640;
    const UINT32 VIDEO_HEIGHT = 480;
    const UINT32 VIDEO_FPS = 25;
    const UINT32 VIDEO_BIT_RATE = 800000;
    const GUID   VIDEO_ENCODING_FORMAT = MFVideoFormat_H264;
    const GUID   VIDEO_INPUT_FORMAT = MFVideoFormat_RGB32;
    const UINT32 VIDEO_PELS = VIDEO_WIDTH * VIDEO_HEIGHT;
    const UINT32 VIDEO_FRAME_COUNT = 20 * VIDEO_FPS;

    // Buffer to hold the video frame data.
    DWORD videoFrameBuffer[VIDEO_PELS];

    HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex)
    {
      *ppWriter = NULL;
      *pStreamIndex = NULL;

      IMFSinkWriter   *pSinkWriter = NULL;
      IMFMediaType    *pMediaTypeOut = NULL;   
      IMFMediaType    *pMediaTypeIn = NULL;   
      DWORD           streamIndex;     

      HRESULT hr = MFCreateSinkWriterFromURL(L"output.wmv", NULL, NULL, &pSinkWriter);

      // Set the output media type.
      if (SUCCEEDED(hr))
      {
        hr = MFCreateMediaType(&pMediaTypeOut);   
      }
      if (SUCCEEDED(hr))
      {
        hr = pMediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);     
      }
      if (SUCCEEDED(hr))
      {
        hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, VIDEO_ENCODING_FORMAT);   
      }
      if (SUCCEEDED(hr))
      {
        hr = pMediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, VIDEO_BIT_RATE);   
      }
      if (SUCCEEDED(hr))
      {
        hr = pMediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);   
      }
      if (SUCCEEDED(hr))
      {
        hr = MFSetAttributeSize(pMediaTypeOut, MF_MT_FRAME_SIZE, VIDEO_WIDTH, VIDEO_HEIGHT);   
      }
      if (SUCCEEDED(hr))
      {
        hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_FRAME_RATE, VIDEO_FPS, 1);   
      }
      if (SUCCEEDED(hr))
      {
        hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);   
      }
      if (SUCCEEDED(hr))
      {
        hr = pSinkWriter->AddStream(pMediaTypeOut, &streamIndex);   
      }

      // Set the input media type.
      if (SUCCEEDED(hr))
      {
        hr = MFCreateMediaType(&pMediaTypeIn);   
      }
      if (SUCCEEDED(hr))
      {
        hr = pMediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);   
      }
      if (SUCCEEDED(hr))
      {
        hr = pMediaTypeIn->SetGUID(MF_MT_SUBTYPE, VIDEO_INPUT_FORMAT);     
      }
      if (SUCCEEDED(hr))
      {
        hr = pMediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);   
      }
      if (SUCCEEDED(hr))
      {
        hr = MFSetAttributeSize(pMediaTypeIn, MF_MT_FRAME_SIZE, VIDEO_WIDTH, VIDEO_HEIGHT);   
      }
      if (SUCCEEDED(hr))
      {
        hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_FRAME_RATE, VIDEO_FPS, 1);   
      }
      if (SUCCEEDED(hr))
      {
        hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);   
      }
      if (SUCCEEDED(hr))
      {
        hr = pSinkWriter->SetInputMediaType(streamIndex, pMediaTypeIn, NULL);   
      }

      // Tell the sink writer to start accepting data.
      if (SUCCEEDED(hr))
      {
        hr = pSinkWriter->BeginWriting();
      }

      // Return the pointer to the caller.
      if (SUCCEEDED(hr))
      {
        *ppWriter = pSinkWriter;
        (*ppWriter)->AddRef();
        *pStreamIndex = streamIndex;
      }

      SafeRelease(&pSinkWriter);
      SafeRelease(&pMediaTypeOut);
      SafeRelease(&pMediaTypeIn);
      return hr;
    }

    HRESULT WriteFrame(
      IMFSinkWriter *pWriter, 
      DWORD streamIndex, 
      const LONGLONG& rtStart,        // Time stamp.
      const LONGLONG& rtDuration      // Frame duration.
      )
    {
      IMFSample *pSample = NULL;
      IMFMediaBuffer *pBuffer = NULL;

      const LONG cbWidth = 4 * VIDEO_WIDTH;
      const DWORD cbBuffer = cbWidth * VIDEO_HEIGHT;

      BYTE *pData = NULL;

      // Create a new memory buffer.
      HRESULT hr = MFCreateMemoryBuffer(cbBuffer, &pBuffer);

      // Lock the buffer and copy the video frame to the buffer.
      if (SUCCEEDED(hr))
      {
        hr = pBuffer->Lock(&pData, NULL, NULL);
      }
      if (SUCCEEDED(hr))
      {
        hr = MFCopyImage(
          pData,                      // Destination buffer.
          cbWidth,                    // Destination stride.
          (BYTE*)videoFrameBuffer,    // First row in source image.
          cbWidth,                    // Source stride.
          cbWidth,                    // Image width in bytes.
          VIDEO_HEIGHT                // Image height in pixels.
          );
      }
      if (pBuffer)
      {
        pBuffer->Unlock();
      }

      // Set the data length of the buffer.
      if (SUCCEEDED(hr))
      {
        hr = pBuffer->SetCurrentLength(cbBuffer);
      }

      // Create a media sample and add the buffer to the sample.
      if (SUCCEEDED(hr))
      {
        hr = MFCreateSample(&pSample);
      }
      if (SUCCEEDED(hr))
      {
        hr = pSample->AddBuffer(pBuffer);
      }

      // Set the time stamp and the duration.
      if (SUCCEEDED(hr))
      {
        hr = pSample->SetSampleTime(rtStart);
      }
      if (SUCCEEDED(hr))
      {
        hr = pSample->SetSampleDuration(rtDuration);
      }

      // Send the sample to the Sink Writer.
      if (SUCCEEDED(hr))
      {
        hr = pWriter->WriteSample(streamIndex, pSample);
      }

      SafeRelease(&pSample);
      SafeRelease(&pBuffer);
      return hr;
    }
  }
}
//void main()
//{
//    // Set all pixels to green
//    for (DWORD i = 0; i < VIDEO_PELS; ++i)
//    {
//        videoFrameBuffer[i] = 0x0000FF00;
//    }
//
//    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
//    if (SUCCEEDED(hr))
//    {
//        hr = MFStartup(MF_VERSION);
//        if (SUCCEEDED(hr))
//        {
//            IMFSinkWriter *pSinkWriter = NULL;
//            DWORD stream;
//
//            hr = InitializeSinkWriter(&pSinkWriter, &stream);
//            if (SUCCEEDED(hr))
//            {
//                // Send frames to the sink writer.
//                LONGLONG rtStart = 0;
//                UINT64 rtDuration;
//
//                MFFrameRateToAverageTimePerFrame(VIDEO_FPS, 1, &rtDuration);
//
//                for (DWORD i = 0; i < VIDEO_FRAME_COUNT; ++i)
//                {
//                    hr = WriteFrame(pSinkWriter, stream, rtStart, rtDuration);
//                    if (FAILED(hr))
//                    {
//                        break;
//                    }
//                    rtStart += rtDuration;
//                }
//            }
//            if (SUCCEEDED(hr))
//            {
//                hr = pSinkWriter->Finalize();
//            }
//            SafeRelease(&pSinkWriter);
//            MFShutdown();
//        }
//        CoUninitialize();
//    }
//}