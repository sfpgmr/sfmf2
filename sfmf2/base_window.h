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
namespace sf {
/** window �x�[�X�N���X */

struct rect {
  uint32_t width;
  uint32_t height;
};

struct base_window : boost::noncopyable
{
  typedef boost::signals2::signal<void ()> on_render_type;

  on_render_type on_render;
  
  // ����Window�n���h����Ԃ��B
  virtual void * raw_handle() const = 0;
  // �E�B���h�E�𐶐�����
  virtual void create() = 0;
//  virtual void show(uint32_t show_flag) = 0;
  // �E�B���h�E��\������
  virtual void show() = 0;
  // �E�B���h�E�����\������Ă��邩��Ԃ�
  virtual bool is_show() = 0;
  // �E�B���h�E���B��
  virtual void hide() = 0;
  //virtual void activate() = 0;
  //virtual bool is_activate() = 0;
  //virtual void deactivate() = 0;
  //virtual void 
  //virtual void size(uint32_t width,uint32_t height); 
  //virtual rect size(); 

  virtual void text(std::wstring& text) = 0;
  //virtual std::wstring text() = 0;

  virtual void update() = 0;

protected:
  virtual ~base_window() {};

};

}

