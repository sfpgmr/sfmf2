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
#include "dpi.h"

namespace sf {

/** window ベースクラス */
struct base_window : boost::noncopyable
{
  typedef boost::signals2::signal< void() > on_closed_t;
  // 生のWindowハンドルを返す。
  operator HWND(){ return hwnd(); }
  virtual HWND hwnd()  const = 0;
  virtual float width() const  = 0;
  virtual float height() const = 0;
  virtual sf::dpi& dpi() = 0;
  virtual bool is_fullscreen() = 0;
  virtual MARGINS& margins() = 0;
  virtual on_closed_t& on_closed() = 0;
protected:
  virtual ~base_window() {};
};

}

