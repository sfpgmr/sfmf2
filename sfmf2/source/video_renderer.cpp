#include "stdafx.h"
#include "graphics.h"
#include "video_renderer.h"
#include "test_renderer.h"
#include "fft_renderer.h"
#include "fft_renderer2.h"
#include "wavelet_renderer.h"
#include "fluidcs11_renderer.h"
#include "sf_windows_base.h"
#include "control_base.h"
#include "application.h"
#include "..\csocean\csocean_renderer.h"


#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace sf;
using namespace Concurrency;

struct VertexVideo {
  DirectX::XMFLOAT2 pos;	//x,y,z
  DirectX::XMFLOAT2 uv;	//u,v
};

template <typename Renderer>
struct h264_renderer<Renderer>::impl 
{
	impl(std::wstring& source, std::wstring& destination, int width, int height) : source_(source), destination_(destination), width_(width), height_(height)
  {}
  ~impl(){}

  void run()
  {
    sf::com_initialize com_init;
    sf::auto_mf mf;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    init();

    concurrency::call<int> preview_timer_func([this](int v)-> void {
      preview_updated_();
      }
    );

    concurrency::timer<int> preview_timer(100,0,&preview_timer_func,true);
    preview_timer.start();
    render();
    preview_timer.stop();
    clean_up();
    end = std::chrono::system_clock::now();
    compute_time_ = end - start;

    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    DOUT(boost::wformat(L"compute time: %f \n") % compute_time_.count());
    complete_(compute_time_);
  }

  void run2()
  {
    sf::com_initialize com_init;
    sf::auto_mf mf;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    init();

    concurrency::call<int> preview_timer_func([this](int v)-> void {
      preview_updated_();
    }
    );

    concurrency::timer<int> preview_timer(100, 0, &preview_timer_func, true);
    preview_timer.start();
    render2();
    preview_timer.stop();
    clean_up();
    end = std::chrono::system_clock::now();
    compute_time_ = end - start;

    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    DOUT(boost::wformat(L"compute time: %f \n") % compute_time_.count());
    complete_(compute_time_);
  }

  typename sf::h264_renderer<Renderer>::progress_t& progress()
  {
    return progress_;
  }

  typename sf::h264_renderer<Renderer>::complete_t& complete()
  {
    return complete_;
  }

  typename sf::h264_renderer<Renderer>::preview_updated_t& preview_updated()
  {
    return preview_updated_;
  }

  typename sf::h264_renderer<Renderer>::video_bitmap_t& video_bitmap_created()
  {
    return video_bitmap_created_;
  }

  std::chrono::duration<double>& compute_time() { return compute_time_; }
  const std::wstring& title(){ return title_; }
  void title(const std::wstring& t){ title_ = t; }

  typename Renderer::init_params_t& init_params(){ return init_params_; }

  void terminate()
  {
    terminate_ = true;
  }
private:

  void init()
  {
    init_graphics();
    renderer_.reset(new Renderer(video_renderer_resources(width_, height_, d3d_context_, video_resource_view_, video_render_target_view_, video_depth_view_, video_texture_, video_depth_texture_, video_bitmap_, d2d_context_), init_params_));
    audio_reader_.reset(new audio_reader(source_));
    video_writer_.reset(new video_writer(destination_,audio_reader_->current_media_type(),width_,height_));
  }

  void init_graphics(){
    auto& d3d_device(graphics::instance()->d3d_device());
    d3d_device->GetImmediateContext2(&d3d_context_);

    {
      ID2D1DeviceContextPtr context;
      CHK(graphics::instance()->d2d_device()->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &context));
      context.As(&d2d_context_);
    }

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

    // �`��p�e�N�X�`���̍쐬
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.MiscFlags = 0;//D3D11_RESOURCE_MISC_SHARED;// ���̃X���b�h����Q�Ƃ����\������
    desc.CPUAccessFlags = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.Width = width_;
    desc.Height = height_;

    CHK(d3d_device->CreateTexture2D(&desc, nullptr, &video_texture_));

    // �����e�N�X�`���[�p��SRV
    CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(
      video_texture_.Get(),
      D3D11_SRV_DIMENSION_TEXTURE2D
      );
    CHK(d3d_device->CreateShaderResourceView(video_texture_.Get(), &shaderResourceViewDesc, &video_resource_view_));

    // RTV�̍쐬
    CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(
      video_texture_.Get(),
      D3D11_RTV_DIMENSION_TEXTURE2D,
      desc.Format);

    CHK(
      d3d_device->CreateRenderTargetView(
      video_texture_.Get(),
      &renderTargetViewDesc,
      &video_render_target_view_
      )
      );

    D3D11_RASTERIZER_DESC1 RasterizerDesc = {
      D3D11_FILL_SOLID,
      D3D11_CULL_NONE,	//�|���S���̗��\�𖳂���
      FALSE,
      0,
      0.0f,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE
    };

    // �[�x�o�b�t�@�E�r���[�̍쐬
    // �K�v�ȏꍇ�� 3D �����_�����O�Ŏg�p����[�x�X�e���V�� �r���[���쐬���܂��B
    CD3D11_TEXTURE2D_DESC depthStencilDesc(
      DXGI_FORMAT_D24_UNORM_S8_UINT,
      static_cast<UINT>(width_),
      static_cast<UINT>(height_),
      1, // ���̐[�x�X�e���V�� �r���[�ɂ́A1 �̃e�N�X�`����������܂���B
      1, // 1 �� MIPMAP ���x�����g�p���܂��B
      D3D11_BIND_DEPTH_STENCIL
      );

    CHK(
      d3d_device->CreateTexture2D(
      &depthStencilDesc,
      nullptr,
      &video_depth_texture_
      )
      );

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    CHK(
      d3d_device->CreateDepthStencilView(
      video_depth_texture_.Get(),
      &depthStencilViewDesc,
      &video_depth_view_
      )
      );

    // CPU����ǂݎ��\�ȃT�[�t�F�[�X�𐶐�
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;

    CHK(d3d_device->CreateTexture2D(&desc, 0, &video_stage_texture_));

    // �r�f�I�e�N�X�`����Direct2D����A�N�Z�X���邽�߂�ID2D1Bitmap�̍쐬
    IDXGISurfacePtr surface;
    video_texture_.As(&surface);
    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
      D2D1::BitmapProperties1(
      D2D1_BITMAP_OPTIONS_TARGET  /*| D2D1_BITMAP_OPTIONS_CANNOT_DRAW*/,
      D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
      );
    CHK(d2d_context_->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties, &video_bitmap_));
    // �`��^�[�Q�b�g�̐ݒ�
    d3d_context_->OMSetRenderTargets(1, video_render_target_view_.GetAddressOf(), video_depth_view_.Get());
  }

  void render(){
    DWORD status = 0;
    video_time_ = 0;
    LONGLONG read_size = 0;

    int progress = 0;
    int progress_bkp = 0;

    // �I�[�f�B�I�T���v���̓ǂݏ���

    while (true)
    {
      IMFSamplePtr sample;
      // �I�[�f�B�I�T���v����ǂݍ���
      status = audio_reader_->read_sample(sample);

      if ((status & MF_SOURCE_READERF_ENDOFSTREAM) || terminate_) {
        // EOF�������͒��f������t�@�C�i���C�Y�������s��
        video_writer_->finalize();
        progress_(100);
        break;
      }
      DWORD size = 0;
      sample->GetTotalLength(&size);
      read_size += size;
      LONGLONG time;
      sample->GetSampleTime(&time);

      video_step_time_ = time - video_time_;
      video_time_ = time;

      video_writer_->write_audio_sample(sample.Get());

      render_to_video(sample);
      progress = (int) (read_size * (LONGLONG) 100 / audio_reader_->size());
      if (progress > progress_bkp){
        progress_(progress);
        progress_bkp = progress;
      }
    }
  }

  // ���ǔ� �r�f�I�E�I�[�f�B�I�����_���[
  void render2(){
    DWORD status = 0;
    video_time_ = 0;
    LONGLONG read_size = 0;

    int progress = 0;
    int progress_bkp = 0;

    // �I�[�f�B�I�T���v���̓ǂݏ���
    audio_samples_[0].clear();
    audio_samples_[1].clear();

    while (true)
    {
      IMFSamplePtr sample;
      // �I�[�f�B�I�T���v����ǂݍ���
      status = audio_reader_->read_sample(sample);

      if ((status & MF_SOURCE_READERF_ENDOFSTREAM)) {
        // EOF�������͒��f������t�@�C�i���C�Y�������s��
        progress_(50);
        break;
      }

      if (terminate_){
        progress_(0);
        video_writer_->finalize();
        return;
      }

      // �o�b�t�@�ւ̏�������
      {
        IMFMediaBufferPtr buffer;
        CHK(sample->GetBufferByIndex(0, &buffer));
        INT16* waveBuffer;
        DWORD startPos = 0;
        CHK(buffer->Lock((BYTE**) &waveBuffer, nullptr, nullptr));
        DWORD totalLength;
        CHK(buffer->GetCurrentLength(&totalLength));
        totalLength /= 4;
        for (int i = 0; i < totalLength; ++i){
          audio_samples_[0].push_back(double(*waveBuffer++) / 32768.0);
          audio_samples_[1].push_back(double(*waveBuffer++) / 32768.0);
        }
        CHK(buffer->Unlock());
      }


      DWORD size = 0;
      sample->GetTotalLength(&size);
      read_size += size;

      video_writer_->write_audio_sample(sample.Get());
      progress = (int) (read_size * (LONGLONG) 100 / audio_reader_->size());
      if (progress > progress_bkp){
        progress_(progress/2);
        progress_bkp = progress;
      }
    }

    // �r�f�I�E�T���v���̏�������
    video_time_ = lengthTick  / 4;
    video_step_time_ = lengthTick / 2;

    size_t sample_pos = 0;
    size_t end = audio_samples_[0].size();
    LONGLONG end_time = audio_reader_->sample_time();
    progress_bkp = 0;
    while (end_time > video_writer_->video_sample_time() && sample_pos < end)
    {
      if (terminate_){
        break;
      }
      render_to_video2(sample_pos);
      progress = (int) (video_writer_->video_sample_time() * (LONGLONG) 100 / end_time);
      if (progress > progress_bkp){
        progress_(progress);
        progress_bkp = progress;
      }
      sample_pos += 44100 / 30;//lengthTick / 2;
    }
    video_writer_->finalize();
  }

  void render_to_video2(int samplepos)
  {
    // Direct3D�ŃI�t�X�N���[���Ƀ����_�����O���A���̃f�[�^����������

    // �R���e�L�X�g�̋�����������邽�߂Ƀ��b�N����
    critical_section::scoped_lock lock(application::instance()->video_critical_section());

    renderer_->render(video_writer_->video_sample_time(), samplepos, audio_samples_);
    // �`�悵���e�N�X�`���f�[�^���X�e�[�W�e�N�X�`���ɃR�s�[����
    d3d_context_->CopyResource(video_stage_texture_.Get(), video_texture_.Get());
    // �r�f�I�f�[�^���쐬���A�T���v���Ɏ��߂�
    video_writer_->set_texture_to_sample(d3d_context_.Get(), video_stage_texture_.Get());
    // �r�f�I�f�[�^����������
    video_writer_->write_video_sample();

  }

  void render_to_video(IMFSamplePtr& sample)
  {
    // Direct3D�ŃI�t�X�N���[���Ƀ����_�����O���A���̃f�[�^����������
    IMFMediaBufferPtr buffer;
    CHK(sample->GetBufferByIndex(0, &buffer));
    INT16* waveBuffer;
    DWORD startPos = 0;
    CHK(buffer->Lock((BYTE**) &waveBuffer, nullptr, nullptr));
    DWORD totalLength;
    CHK(buffer->GetCurrentLength(&totalLength));
    totalLength /= 2;
    DWORD length = lengthTick;

    // �R���e�L�X�g�̋�����������邽�߂Ƀ��b�N����
    critical_section::scoped_lock lock(application::instance()->video_critical_section());

    while (video_time_ > video_writer_->video_sample_time())
    {

      {
        // Direct3D11�ɂ�郌���_�����O
        renderer_->render(video_writer_->video_sample_time(),waveBuffer + startPos,length);
        startPos += lengthTick;
        if ((totalLength - startPos) > lengthTick)
        {
          length = lengthTick;
        }
        else {
          length = totalLength - startPos;
        }
        // �`�悵���e�N�X�`���f�[�^���X�e�[�W�e�N�X�`���ɃR�s�[����
        d3d_context_->CopyResource(video_stage_texture_.Get(),video_texture_.Get());
        // �r�f�I�f�[�^���쐬���A�T���v���Ɏ��߂�
        video_writer_->set_texture_to_sample(d3d_context_.Get(), video_stage_texture_.Get());
      }

      // �r�f�I�f�[�^����������
      video_writer_->write_video_sample();
    }

    CHK(buffer->Unlock());

  }

  void clean_up()
  {
    renderer_.reset();
    video_writer_.reset();
    audio_reader_.reset();

    video_render_target_view_.Reset();
    video_resource_view_.Reset();
    video_stage_texture_.Reset();
    video_depth_texture_.Reset();
    video_depth_view_.Reset();
    video_texture_.Reset();
    d2d_context_.Reset();
    d3d_context_.Reset();
  }


  private:

    bool terminate_ = false;

  std::wstring source_;
  std::wstring destination_;
  unsigned int width_, height_;
  std::wstring title_;
  // Direct2D/3D���\�[�X
  ID3D11DeviceContext2Ptr d3d_context_;// �R���e�L�X�g
  ID3D11ShaderResourceViewPtr    video_resource_view_;// �v���r���[�Ƃ��ĉ�ʂɕ\�����邽�߂ɗp����
  ID3D11RenderTargetViewPtr video_render_target_view_;// �e�N�X�`���p�`��^�[�Q�b�g�Ƃ���
  ID3D11DepthStencilViewPtr video_depth_view_;// �e�N�X�`���p�[�x�r���[
  ID3D11Texture2DPtr video_texture_;// �r�f�I�����_�����O�e�N�X�`��
  ID3D11Texture2DPtr video_depth_texture_;// �[�x�o�b�t�@�e�N�X�`��
  ID3D11Texture2DPtr video_stage_texture_;// MF�̃r�f�I�t���[���Ƃ���CPU�Ɉ����n�����߂̃e�N�X�`��
  float clear_color_[4];
  ID2D1Bitmap1Ptr video_bitmap_;// Direct2D�p�r�f�I�r�b�g�}�b�v
  ID2D1DeviceContext1Ptr d2d_context_;// �R���e�L�X�g
  std::unique_ptr<Renderer> renderer_;//���ۂɕ`�揈�����s�������_��
  typename Renderer::init_params_t init_params_;
  std::unique_ptr<video_writer> video_writer_;
  std::unique_ptr<audio_reader> audio_reader_;

  LONGLONG video_time_;
  LONGLONG video_step_time_;

  std::chrono::duration<double> compute_time_;

  typename sf::h264_renderer<Renderer>::progress_t progress_;
  typename sf::h264_renderer<Renderer>::complete_t complete_;
  typename sf::h264_renderer<Renderer>::preview_updated_t preview_updated_;
  typename sf::h264_renderer<Renderer>::video_bitmap_t video_bitmap_created_;


  // �X�e���I�T���v����ۑ�����
  audio_samples_t audio_samples_;

};

template <typename Renderer>
h264_renderer<Renderer>::h264_renderer(std::wstring& source, std::wstring& destination, unsigned int width, unsigned int height) : impl_(new h264_renderer<Renderer>::impl(source, destination, width, height))
{

}

template <typename Renderer>
h264_renderer<Renderer>::~h264_renderer()
{
}

template <typename Renderer>
void h264_renderer<Renderer>::run()
{
  impl_->run();
  agent::done();
}

template <typename Renderer>
typename h264_renderer<Renderer>::progress_t& h264_renderer<Renderer>::progress()
{
  return impl_->progress();
}

template <typename Renderer>
std::chrono::duration<double>& h264_renderer<Renderer>::compute_time()
{
  return impl_->compute_time();
}
template <typename Renderer>
typename h264_renderer<Renderer>::complete_t& h264_renderer<Renderer>::complete(){
  return impl_->complete();
};

template <typename Renderer>
typename h264_renderer<Renderer>::preview_updated_t& h264_renderer<Renderer>::preview_updated(){
  return impl_->preview_updated();
};

template <typename Renderer>
typename h264_renderer<Renderer>::video_bitmap_t& h264_renderer<Renderer>::video_bitmap_created(){
  return impl_->video_bitmap_created();
};


template <typename Renderer>
typename Renderer::init_params_t& h264_renderer<Renderer>::init_params(){
	return impl_->init_params();
};

template <typename Renderer>
void h264_renderer<Renderer>::terminate()
{
  impl_->terminate();
  Concurrency::agent::wait(this);
}


/////////////////////////////////////////////////////////////
// h264_renderer2 
/////////////////////////////////////////////////////////////

template <typename Renderer>
h264_renderer2<Renderer>::h264_renderer2(std::wstring& source, std::wstring& destination, unsigned int width, unsigned int height) : 
  h264_renderer<Renderer>(source,destination,  width,  height)
{

}

template <typename Renderer>
void h264_renderer2<Renderer>::run()
{
  impl_->run2();
  agent::done();
}


template  class h264_renderer<test_renderer_base>;
template  class h264_renderer<fft_renderer_base>;
template  class h264_renderer2<fft_renderer_base>;
template  class h264_renderer2<fft_renderer2_base>;
template  class h264_renderer<wavelet_renderer_base>;
template  class h264_renderer2<fluidcs11_renderer_base>;
template  class h264_renderer<fluidcs11_renderer_base>;
template  class h264_renderer2<csocean_renderer_base>;
template  class h264_renderer<csocean_renderer_base>;


