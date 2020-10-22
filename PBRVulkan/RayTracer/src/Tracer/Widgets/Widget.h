#pragma once

namespace Tracer
{
	struct Settings;
}

namespace Interface
{
	class Widget
	{
	public:
		virtual ~Widget() = default;
		virtual void Render(Tracer::Settings& settings) = 0;
	};
}
