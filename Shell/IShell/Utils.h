#pragma once

namespace IShell
{
	class SHELL_API IFinalCallback
	{
	public:
		virtual void OnFinalCallback() = 0;
	};
}