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

#include "StdAfx.h"
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#include "sf_windows.h"
#include "dpi.h"

namespace sf {
  void dpi::init()
  {
    if (!init_)
    {
      sf::get_dc hdc(0);
      if (hdc.get())
      {
        dpi_x_ = GetDeviceCaps(hdc.get(), LOGPIXELSX);
        dpi_y_ = GetDeviceCaps(hdc.get(), LOGPIXELSY);
        ReleaseDC(NULL, hdc.get());
      }
      init_ = true;
    }
  }

}