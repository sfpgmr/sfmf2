#pragma once
namespace sf {
  class app_base_directory : public sf::singleton<app_base_directory>
  {
  public:
    app_base_directory();
    virtual ~app_base_directory();
    const std::wstring& base_dir() const { return base_directory_; }
    const std::wstring& shader_dir() const { return shader_directory_; }
    const std::wstring& resource_dir() const { return resource_directory_; }
  private:
    std::wstring base_directory_;
    std::wstring shader_directory_;
    std::wstring resource_directory_;
  };
}

