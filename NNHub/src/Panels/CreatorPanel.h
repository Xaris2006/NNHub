#pragma once

#include <memory>
#include "Walnut/Image.h"

namespace Panels
{
	class CreatorPanel
	{
	public:
		void OnAttach();

		void OnDetach();

		void OnUIRender();
	private:

		std::shared_ptr<Walnut::Image> m_PlayIcon;
		std::shared_ptr<Walnut::Image> m_StopIcon;
		std::shared_ptr<Walnut::Image> m_PauseIcon;
		std::shared_ptr<Walnut::Image> m_ResumeIcon;
	};
}