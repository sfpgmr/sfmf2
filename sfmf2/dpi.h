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

namespace sf{
  template <typename T>
  class dpi_t
  {
  public:
    dpi_t() : init_(false), dpi_x_(96), dpi_y_(96) { }
    // ��ʂ� DPI ���擾���܂��B
    T dpix() { init(); return dpi_x_; }
    T dpiy() { init(); return dpi_y_; }

    // ��΃s�N�Z���Ƒ��΃s�N�Z���Ԃ̕ϊ����s���܂��B
    T scale_x(T x) { init(); return MulDiv(x, dpi_x_, 96); }
    T scale_y(T y) { init(); return MulDiv(y, dpi_y_, 96); }
    T unscale_x(T x) { init(); return MulDiv(x, 96, dpi_x_); }
    T unscale_y(T y) { init(); return MulDiv(y, 96, dpi_y_); }

    // ��ʃT�C�Y (���΃s�N�Z���P��) �����߂܂��B
    T scaled_screen_width() { return scaled_system_metrix_x(SM_CXSCREEN); }
    T scaled_screen_height() { return scaled_system_metrix_y(SM_CYSCREEN); }

    // �l�p�`�̃T�C�Y���΃s�N�Z�����瑊�΃s�N�Z���ɕύX���܂��B
    void scale_rect(RECT *rect_ptr)
    {
      rect_ptr->left = scale_x(rect_ptr->left);
      rect_ptr->right = scale_x(rect_ptr->right);
      rect_ptr->top = scale_y(rect_ptr->top);
      rect_ptr->bottom = scale_y(rect_ptr->bottom);
    }

    // ��ʉ𑜓x���Œ�value (���΃s�N�Z���P��) �𖞂����Ă��邩�ǂ�����
    // �m�F���܂��B
    bool is_resolution_at_least(T xmin, T ymin) 
    { 
      return (scaled_screen_width() >= xmin) && (scaled_screen_height() >= ymin); 
    }
    // �|�C���g �T�C�Y (1/72 �C���`) ���΃s�N�Z���ɕϊ����܂��B
    T point_to_pixels(int pt) { return MulDiv(pt, dpi_y_, 72); }
    // �L���b�V�����ꂽ���g���b�N�����ׂĖ����ɂ��܂��B
    void invalidate() { init_ = false; }
  private:
    void init();

    T scaled_system_metrix_x(int nIndex) 
    { 
      init(); 
      return MulDiv(GetSystemMetrics(nIndex), 96, dpi_x_); 
    }

    T scaled_system_metrix_y(int nIndex) 
    { 
      init(); 
      return MulDiv(GetSystemMetrics(nIndex), 96, dpi_y_); 
    }

  private:
    bool init_;
    T dpi_x_;
    T dpi_y_;
  };

  typedef dpi_t<int> dpi;
}
