#pragma once

namespace sf {
	struct widget_base{
		DirectX::SimpleMath::Vector4 position;
		DirectX::SimpleMath::Vector4 color;
		std::string text;
	};

	struct widgets {

		typedef boost::ptr_vector<widget_base> widgets_;
	};

}

