#include "stdafx.h"
#include "app_base_directory.h"


using namespace sf;

app_base_directory::app_base_directory()
{
  // ベースディレクトリの取得
  wchar_t dir[MAX_PATH];
  ::GetCurrentDirectoryW(MAX_PATH, dir);
  base_directory_.assign(dir);
  base_directory_.append(L"\\");
  shader_directory_ = base_directory_ + L"shaders\\";
  resource_directory_ = base_directory_ + L"resources\\";
}


app_base_directory::~app_base_directory()
{
}
