#pragma once
/*
  ==============================================================================

  This file is part of the async
  Copyright 2005-10 by Satoshi Fujiwara.

  async can be redistributed and/or modified under the terms of the
  GNU General Public License, as published by the Free Software Foundation;
  either version 2 of the License, or (at your option) any later version.

  async is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with async; if not, visit www.gnu.org/licenses or write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA 02111-1307 USA

  ==============================================================================
  */
#include "singleton.h"
#include "exception.h"
#include "ring_buffer.h"
#include "wasapi.h"
#include "async_reader.h"
#include "sf_memory.h"
//#include "toplevel_window.h"
//#include "dcomposition_window.h"
#include "fluidcs11_window.h"
#include "Player.h"
#include "reader_agent.h"
#include "output_agent.h"
#include "input_agent.h"
#include "mixer_agent.h"
#include "sequencer.h"
#include "midi_device_manager.h"
#include "graphics.h"
//#include "test_renderer.h"
#include "fft_renderer.h"
#include "fluidcs11_renderer.h"
#include "..\csocean\csocean_renderer.h"

namespace sf {
  class application : public singleton<application>
  {

    typedef boost::signals2::signal<void()> player_init_ok_t;

    static const size_t Q_SIZE = 8;
  public:
    struct exception
      : public sf::win32_error_exception
    {
      exception(uint32_t hr) : win32_error_exception(hr) {};
      exception() : win32_error_exception() {};
    };


    application();
    ~application();

    int execute(HINSTANCE hInstance,
      HINSTANCE hPrevInstance,
      LPTSTR    lpCmdLine,
      int       nCmdShow);


    std::wstring& app_id(){ return app_id_; };
    HINSTANCE instance_handle() { return instance_handle_; };


    // ----------------------------------------
    // Player
    // ----------------------------------------
    //  player_init_ok_t& player_init_ok(){return player_init_ok_;};
    //  void init_player(HWND vhwnd,HWND hwnd);
    //  sf::player::PlayerPtr& Player(){return player_;}
    //  void release_player(){player_.Reset();}
  private:
    //player_init_ok_t player_init_ok_;
    //player::PlayerPtr player_;
    //void wasapi_setup();

    //---------------------------------------
    // ファイル読取り関係
    //---------------------------------------
  public:

    void reader_setup(const std::wstring& file_path);
    void reader_read_file();
    void reader_pause();
    void reader_stop();
    void reader_end();
    uint64_t reader_position() { return reader_agent_.position(); }
    void reader_position(uint64_t pos){ reader_agent_.position(pos); }
    uint64_t reader_data_size(){ return reader_agent_.size(); }
    void reader_repeat_mode(bool v){ reader_agent_.repeat_mode(v); }
    bool reader_repeat_mode(){ return reader_agent_.repeat_mode(); }
    int reader_status(std::memory_order o = std::memory_order_seq_cst) { return reader_agent_.status(o); }
    ringbuffer_t& reader_ringbuffer() { return reader_agent_.ringbuffer(); }
  private:
    reader_agent_t reader_agent_;

    // ----------------------------
    // WASAPIoutput関係
    // ----------------------------
    // outputデバイスの設定・変更
  public:
    void apply_output_device_config(int device_index, wasapi_device_manager::device_info::params_t& params);
    audio_base& output_device() { return output_agent_.device(); }
    ringbuffer_t& output_ringbuffer() { return output_agent_.ringbuffer(); }
    output_agent_t& output_thread(){ return output_agent_; }
  private:
    output_agent_t output_agent_;
    // -----------------------------  
    // WASAPI入力関係
    // -----------------------------  
  public:
    void apply_input_device_config(int device_index, wasapi_device_manager::device_info::params_t& params)
    {
      input_agent_.apply_config(device_index, params);
    };
    audio_base& input_device() { return input_agent_.device(); }
    ringbuffer_t& input_ringbuffer() { return input_agent_.ringbuffer(); }
    input_agent_t& input_thread(){ return input_agent_; }
  private:
    input_agent_t input_agent_;

    // -----------------------------  
    // ミキサー関係
    // -----------------------------  
  public:
    ringbuffer_t& mixer_ringbuffer(){ return mixer_agent_.ringbuffer(); }
    mixer_agent_t& mixer_thread(){ return mixer_agent_; }
  private:
    mixer_agent_t mixer_agent_;

    //--------------------------------
    // シーケンサ
    //--------------------------------
  public:
    sf::sequencer & sequencer() { return seq_; }
  private:
    sf::sequencer seq_;
    static std::wstring app_id_;
    static const int WAIT_TIMEOUT_DEFAULT = 100;


    HINSTANCE instance_handle_;
    int return_code_;

    // -----------------------------  
    // その他
    // -----------------------------  

  private:
    sf::fluidcs11_window_ptr window_;

    // ----------------------------
    // H.264 レンダラー
    // ----------------------------
  public:
    
    std::unique_ptr<csocean_renderer>& renderer(){ return video_renderer_; }

    void renderer_source_path(const std::wstring& path)
    { 
      renderer_source_path_ = path; 

      if (renderer_target_path().size() == 0)
      {
        boost::filesystem::path p(path);
        p = p.replace_extension(boost::filesystem::path(L"mp4"));
        renderer_target_path(p.native());
      }
      renderer_source_path_changed_();
      check_enable();
    }

    std::wstring& renderer_source_path()
    {
      return renderer_source_path_;
    }
    
    void renderer_target_path(const std::wstring& path){
      renderer_target_path_ = path; 
      renderer_target_path_changed_();
      check_enable();
    }

    std::wstring& renderer_target_path()
    {
      return renderer_target_path_;
    }


    void check_enable(){
      if (renderer_source_path_.size() > 0 && renderer_target_path_.size() > 0){
        renderer_enable_ = true;
        renderer_enable_status_changed_(renderer_enable_);
      }
    }

	void execute_rendering(const std::function<void(int)>& progress, const std::function<void(std::chrono::duration<double>&)> & complete);

    typedef boost::signals2::signal<void()> renderer_source_path_changed_t;
    typedef boost::signals2::signal<void()> renderer_target_path_changed_t;
    typedef boost::signals2::signal<void(bool)> renderer_enable_status_changed_t;
 
    renderer_source_path_changed_t& renderer_source_path_changed(){ return renderer_source_path_changed_; }
    renderer_target_path_changed_t& renderer_target_path_changed(){ return renderer_target_path_changed_; }
    renderer_enable_status_changed_t& renderer_enable_status_changed(){ return renderer_enable_status_changed_; }

    std::wstring& renderer_video_title(){ return renderer_video_title_; }
    void video_bitmap(ID2D1Bitmap1Ptr& bitmap);
    concurrency::critical_section&  video_critical_section(){
      return  video_critical_section_;
    }
 private:
   concurrency::critical_section video_critical_section_;
   bool renderer_enable_;
    std::wstring renderer_source_path_;
    std::wstring renderer_target_path_;
    std::unique_ptr<csocean_renderer> video_renderer_;
    renderer_source_path_changed_t renderer_source_path_changed_;
    renderer_target_path_changed_t renderer_target_path_changed_;
    renderer_enable_status_changed_t renderer_enable_status_changed_;
    std::wstring renderer_video_title_;
  };
}

